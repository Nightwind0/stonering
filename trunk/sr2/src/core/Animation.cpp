#include <ClanLib/core.h>
#include <list>
#include <cassert>
#include "Animation.h"
#include "LevelFactory.h"
#include "IApplication.h"
#include "Level.h"

using namespace StoneRing;


SpriteStub::SpriteStub()
{
}

SpriteStub::~SpriteStub()
{
}

std::string SpriteStub::getName() const
{
    return mName;
}

SpriteStub::eBindTo SpriteStub::getBindTo() const
{
    return meBindTo;
}


void SpriteStub::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes);

    if(hasAttr("bindTo",pAttributes))
    {
        meBindTo = static_cast<eBindTo>(getRequiredInt("bindTo",pAttributes));
    }
    else meBindTo = NONE;
}


eWho 
AlterSprite::whoFromString ( const std::string &str )
{
    if(str == "none") return NONE;
    else if (str == "caster") return CASTER;
    else if (str == "caster_group") return CASTER_GROUP;
    else if (str == "target") return TARGET;
    else if (str == "target_group") return TARGET_GROUP;
    else if (str == "all") return ALL;
    else throw CL_Error("Bad Phase Hide = " + str );

}
/*
            HIDE, SMALLER_SIZE, LARGER_SIZE, HALF_SIZE, DOUBLE_SIZE, NEGATIVE,
            X_FLIP, Y_FLIP, GRAYSCALE, GREENSCALE, REDSCALE, BLUESCALE 
*/
AlterSprite::eAlter AlterSprite::alterFromString(const std::string &str)
{
    if(str == "hide") return HIDE;
    else if(str == "smaller_size") return SMALLER_SIZE;
    else if(str == "larger_size") return LARGER_SIZE;
    else if(str == "half_size") return HALF_SIZE;
    else if(str == "double_size") return DOUBLE_SIZE;
    else if(str == "negative") return NEGATIVE;
    else if(str == "xflip") return X_FLIP;
    else if(str == "yflip") return Y_FLIP;
    else if(str == "grayscale") return GRAYSCALE;
    else if(str == "greenscale") return GREENSCALE;
    else if(str == "redscale") return REDSCALE;
    else if(str == "bluescale") return BLUESCALE;
    else throw CL_Error("Bad alter: " + str);
}


SpriteAnimation::SpriteAnimation()
:mpSpriteRef(NULL),
 mpStub(NULL),
 mpMovement(NULL),
 mpAlterSprite(NULL)
{
}

SpriteAnimation::~SpriteAnimation()
{
}

void SpriteAnimation::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes);
}

bool SpriteAnimation::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESPRITESTUB:
        mpStub = dynamic_cast<SpriteStub*>(pElement);
        break;
    case ESPRITEREF:
        mpSpriteRef = dynamic_cast<SpriteRef*>(pElement);
        break;
    case ESPRITEMOVEMENT:
        mpMovement = dynamic_cast<SpriteMovement*>(pElement);
        break;
    case EALTERSPRITE:
        mpAlterSprite = dynamic_cast<AlterSprite*>(pElement);
        break;
    default: return false;
    }

    return false;
}

void SpriteAnimation::loadFinished()
{
    if(!mpStub && !mpSpriteRef)
        throw CL_Error("Missing sprite stub or sprite ref on spriteAnimation");
}


SpriteMovement::SpriteMovement()
:mbEndFocus(false)
{
}


void SpriteMovement::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mInitialFocus.meFocusType = focusTypeFromString(getRequiredString("initialFocus",pAttributes));

    if(hasAttr("initialFocusX",pAttributes))
        mInitialFocus.meFocusX = focusXFromString( getString("initialFocusX",pAttributes));
    else mInitialFocus.meFocusX = X_CENTER;

    if(hasAttr("initialFocusY",pAttributes))
        mInitialFocus.meFocusY = focusYFromString( getString("initialFocusY",pAttributes));
    else mInitialFocus.meFocusY = Y_CENTER;

    if(hasAttr("initialFocusZ",pAttributes))
        mInitialFocus.meFocusZ = focusZFromString( getString("initialFocusZ",pAttributes));
    else mInitialFocus.meFocusZ = FRONT;

    if(hasAttr("endFocus",pAttributes))
    {
        mbEndFocus = true;
        mEndFocus.meFocusType = focusTypeFromString(getRequiredString("endFocus",pAttributes));

        if(hasAttr("endFocusX",pAttributes))
            mEndFocus.meFocusX = focusXFromString( getString("endFocusX",pAttributes));
        else mEndFocus.meFocusX = X_CENTER;

        if(hasAttr("endFocusY",pAttributes))
            mEndFocus.meFocusY = focusYFromString( getString("endFocusY",pAttributes));
        else mEndFocus.meFocusY = Y_CENTER;

        if(hasAttr("endFocusZ",pAttributes))
            mEndFocus.meFocusZ = focusZFromString( getString("endFocusZ",pAttributes));
        else mEndFocus.meFocusZ = FRONT;
    }

    if(hasAttr("movementDirection",pAttributes))
        meMovementDirection = movementDirectionFromString( getString("movementDirection",pAttributes));
    else meMovementDirection = STILL;

    if(hasAttr("movementStyle",pAttributes))
        meMovementStyle = movementStyleFromString ( getString("movementStyle",pAttributes ));
    else meMovementStyle = STRAIGHT;
}

bool SpriteMovement::hasEndFocus() const
{
    return mbEndFocus;
}

CL_DomElement 
SpriteMovement::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "SpriteMovement");
}

SpriteMovement::eFocus 
SpriteMovement::focusTypeFromString(const std::string &str)
{
    if(str == "screen") return SCREEN;
    else if (str == "caster") return CASTER;
    else if (str == "target") return TARGET;
    else if (str == "caster_group") return CASTER_GROUP;
    else if (str == "target_group") return TARGET_GROUP;
    else throw CL_Error("Bad focus: " + str );
}

SpriteMovement::eFocusX
SpriteMovement::focusXFromString ( const std::string &str )
{
    if(str == "center") return X_CENTER;
    else if (str == "towards") return TOWARDS;
    else if (str == "away") return AWAY;
    else if (str == "left")  return LEFT;
    else if (str == "right") return RIGHT;
    else throw CL_Error("Bad focus x type: " + str );
}

SpriteMovement::eFocusY
SpriteMovement::focusYFromString ( const std::string &str )
{
    if(str == "center") return Y_CENTER;
    else if (str == "top") return TOP;
    else if (str == "bottom") return BOTTOM;
    else throw CL_Error("Bad focus y type: " + str );
}

SpriteMovement::eFocusZ
SpriteMovement::focusZFromString ( const std::string &str )
{
    if(str == "front") return FRONT;
    else if (str == "back") return BACK;
    else throw CL_Error("Bad focus z type: " + str );
}


SpriteMovement::eMovementDirection 
SpriteMovement::movementDirectionFromString ( const std::string &str )
{
    if(str == "still") return STILL;
    else if (str == "n") return N;
    else if (str == "s") return S;
    else if (str == "e") return E;
    else if (str == "w") return W;
    else if (str == "ne") return NE;
    else if (str == "nw") return NW;
    else if (str == "se") return SE;
    else if (str == "sw") return SW;
    else if (str == "away") return MOVE_AWAY;
    else if (str == "towards") return MOVE_TOWARDS;
    else if (str == "endFocus") return END_FOCUS;

    throw CL_Error("Bad movement Direction: " + str);
    return STILL;
}

SpriteMovement::eMovementStyle    
SpriteMovement::movementStyleFromString( const std::string &str )
{
    if(str == "straight") return STRAIGHT;
    else if (str == "arc_over") return ARC_OVER;
    else if (str == "arc_under") return ARC_UNDER;
    else if (str == "sine") return SINE;
    else throw CL_Error("Bad movementStyle " + str );
}

SpriteMovement::~SpriteMovement()
{
}


SpriteMovement::Focus SpriteMovement::getInitialFocus() const
{
    return mInitialFocus;
}

SpriteMovement::Focus SpriteMovement::getEndFocus() const
{
    return mEndFocus;
}


SpriteMovement::eMovementDirection 
SpriteMovement::getMovementDirection() const
{
    return meMovementDirection;
}

SpriteMovement::eMovementStyle SpriteMovement::getMovementStyle() const
{
    return meMovementStyle;
}

bool Phase::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESCRIPT:
        mpScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case ESPRITEANIMATION:
        mSpriteAnimations.push_back( dynamic_cast<SpriteAnimation*>(pElement));
        break;
    default:
        return false;
    }

    return true;
}

void Phase::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{

    mnDuration = getRequiredInt("duration", pAttributes);   
    mbParallel = getRequiredBool("parallel",pAttributes);

}

Phase::Phase():mpScript(NULL)
{
}

Phase::~Phase()
{
    delete mpScript;

    std::for_each(mSpriteAnimations.begin(),mSpriteAnimations.end(),del_fun<SpriteAnimation>());
}

    
CL_DomElement 
Phase::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "Phase");
}

uint Phase::getDurationMs() const
{
    return mnDuration;
}

void Phase::execute() 
{
    if(mpScript)
        mpScript->executeScript();
}

std::list<SpriteAnimation*>::const_iterator 
Phase::getSpriteAnimationsBegin() const 
{
    return mSpriteAnimations.begin();
}

std::list<SpriteAnimation*>::const_iterator 
Phase::getSpriteAnimationsEnd() const
{
    return mSpriteAnimations.end();
}


    

Animation::Animation()
{
}


void Animation::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mName = getRequiredString("name",pAttributes);

    std::string type = getRequiredString("type",pAttributes);

    if(type == "battle") meType = BATTLE;
    else if (type == "world") meType = WORLD;
    else throw CL_Error("Bogus animation type: " + type );

}

bool Animation::handleElement(eElement element, Element * pElement)
{
    if(element == EPHASE )
    {
        mPhases.push_back ( dynamic_cast<Phase*>(pElement) );
        return true;
    }
    else return false;
}


Animation::~Animation()
{
    std::for_each(mPhases.begin(),mPhases.end(),del_fun<Phase>());
}

CL_DomElement 
Animation::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "animation");
}

std::string 
Animation::getName() const
{
    return mName;
}



Animation::eType 
Animation::getType() const
{
    return meType;
}


std::list<Phase*>::const_iterator Animation::getPhasesBegin() const
{
    return mPhases.begin();
}

std::list<Phase*>::const_iterator Animation::getPhasesEnd() const
{
    return mPhases.end();
}




    








