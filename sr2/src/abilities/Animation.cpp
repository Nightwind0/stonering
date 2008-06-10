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

std::string SpriteStub::GetName() const
{
    return m_name;
}

SpriteStub::eBindTo SpriteStub::GetBindTo() const
{
    return m_eBindTo;
}


void SpriteStub::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    m_name = get_required_string("name",pAttributes);

    if(has_attribute("bindTo",pAttributes))
    {
        m_eBindTo = static_cast<eBindTo>(get_required_int("bindTo",pAttributes));
    }
    else m_eBindTo = NONE;
}


eWho 
AlterSprite::who_from_string ( const std::string &str )
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
AlterSprite::eAlter AlterSprite::alter_from_string(const std::string &str)
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
:m_pSpriteRef(NULL),
 m_pStub(NULL),
 m_pMovement(NULL),
 m_pAlterSprite(NULL)
{
}

SpriteAnimation::~SpriteAnimation()
{
}

void SpriteAnimation::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    m_name = get_required_string("name",pAttributes);
}

bool SpriteAnimation::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESPRITESTUB:
        m_pStub = dynamic_cast<SpriteStub*>(pElement);
        break;
    case ESPRITEREF:
        m_pSpriteRef = dynamic_cast<SpriteRef*>(pElement);
        break;
    case ESPRITEMOVEMENT:
        m_pMovement = dynamic_cast<SpriteMovement*>(pElement);
        break;
    case EALTERSPRITE:
        m_pAlterSprite = dynamic_cast<AlterSprite*>(pElement);
        break;
    default: return false;
    }

    return false;
}

void SpriteAnimation::load_finished()
{
    if(!m_pStub && !m_pSpriteRef)
        throw CL_Error("Missing sprite stub or sprite ref on spriteAnimation");
}


SpriteMovement::SpriteMovement()
:m_bEndFocus(false)
{
}


void SpriteMovement::load_attributes(CL_DomNamedNodeMap * pAttributes)
{
    m_initial_focus.meFocusType = focusTypeFromString(get_required_string("initialFocus",pAttributes));

    if(has_attribute("initialFocusX",pAttributes))
        m_initial_focus.meFocusX = focusXFromString( get_string("initialFocusX",pAttributes));
    else m_initial_focus.meFocusX = X_CENTER;

    if(has_attribute("initialFocusY",pAttributes))
        m_initial_focus.meFocusY = focusYFromString( get_string("initialFocusY",pAttributes));
    else m_initial_focus.meFocusY = Y_CENTER;

    if(has_attribute("initialFocusZ",pAttributes))
        m_initial_focus.meFocusZ = focusZFromString( get_string("initialFocusZ",pAttributes));
    else m_initial_focus.meFocusZ = FRONT;

    if(has_attribute("endFocus",pAttributes))
    {
        m_bEndFocus = true;
        m_end_focus.meFocusType = focusTypeFromString(get_required_string("endFocus",pAttributes));

        if(has_attribute("endFocusX",pAttributes))
            m_end_focus.meFocusX = focusXFromString( get_string("endFocusX",pAttributes));
        else m_end_focus.meFocusX = X_CENTER;

        if(has_attribute("endFocusY",pAttributes))
            m_end_focus.meFocusY = focusYFromString( get_string("endFocusY",pAttributes));
        else m_end_focus.meFocusY = Y_CENTER;

        if(has_attribute("endFocusZ",pAttributes))
            m_end_focus.meFocusZ = focusZFromString( get_string("endFocusZ",pAttributes));
        else m_end_focus.meFocusZ = FRONT;
    }

    if(has_attribute("movementDirection",pAttributes))
        m_eMovementDirection = movementDirectionFromString( get_string("movementDirection",pAttributes));
    else m_eMovementDirection = STILL;

    if(has_attribute("movementStyle",pAttributes))
        m_eMovementStyle = movementStyleFromString ( get_string("movementStyle",pAttributes ));
    else m_eMovementStyle = STRAIGHT;
}

bool SpriteMovement::HasEndFocus() const
{
    return m_bEndFocus;
}

CL_DomElement 
SpriteMovement::CreateDomElement(CL_DomDocument &doc) const
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


SpriteMovement::Focus SpriteMovement::GetInitialFocus() const
{
    return m_initial_focus;
}

SpriteMovement::Focus SpriteMovement::GetEndFocus() const
{
    return m_end_focus;
}


SpriteMovement::eMovementDirection 
SpriteMovement::GetMovementDirection() const
{
    return m_eMovementDirection;
}

SpriteMovement::eMovementStyle SpriteMovement::GetMovementStyle() const
{
    return m_eMovementStyle;
}

bool Phase::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESCRIPT:
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case ESPRITEANIMATION:
        m_sprite_animations.push_back( dynamic_cast<SpriteAnimation*>(pElement));
        break;
    default:
        return false;
    }

    return true;
}

void Phase::load_attributes(CL_DomNamedNodeMap * pAttributes)
{

    m_nDuration = get_required_int("duration", pAttributes);   
    m_bParallel = get_required_bool("parallel",pAttributes);

}

Phase::Phase():m_pScript(NULL)
{
}

Phase::~Phase()
{
    delete m_pScript;

    std::for_each(m_sprite_animations.begin(),m_sprite_animations.end(),del_fun<SpriteAnimation>());
}

    
CL_DomElement 
Phase::CreateDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "Phase");
}

uint Phase::GetDurationMs() const
{
    return m_nDuration;
}

void Phase::Execute() 
{
    if(m_pScript)
        m_pScript->ExecuteScript();
}

std::list<SpriteAnimation*>::const_iterator 
Phase::GetSpriteAnimationsBegin() const 
{
    return m_sprite_animations.begin();
}

std::list<SpriteAnimation*>::const_iterator 
Phase::GetSpriteAnimationsEnd() const
{
    return m_sprite_animations.end();
}


    

Animation::Animation()
{
}


void Animation::load_attributes(CL_DomNamedNodeMap *pAttributes)
{
    m_name = get_required_string("name",pAttributes);

    std::string type = get_required_string("type",pAttributes);

    if(type == "battle") m_eType = BATTLE;
    else if (type == "world") m_eType = WORLD;
    else throw CL_Error("Bogus animation type: " + type );

}

bool Animation::handle_element(Element::eElement element, Element * pElement)
{
    if(element == EPHASE )
    {
        m_phases.push_back ( dynamic_cast<Phase*>(pElement) );
        return true;
    }
    else return false;
}


Animation::~Animation()
{
    std::for_each(m_phases.begin(),m_phases.end(),del_fun<Phase>());
}

CL_DomElement 
Animation::CreateDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "animation");
}

std::string 
Animation::GetName() const
{
    return m_name;
}



Animation::eType 
Animation::GetType() const
{
    return m_eType;
}


std::list<Phase*>::const_iterator Animation::GetPhasesBegin() const
{
    return m_phases.begin();
}

std::list<Phase*>::const_iterator Animation::GetPhasesEnd() const
{
    return m_phases.end();
}




    








