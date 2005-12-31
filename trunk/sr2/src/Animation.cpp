#include <ClanLib/core.h>
#include <list>
#include "Animation.h"
#include "LevelFactory.h"
#include "IApplication.h"
#include "Level.h"

using namespace StoneRing;




AnimationSpriteRef::AnimationSpriteRef()
{
}


void AnimationSpriteRef::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
		meInitialFocus = initialFocusFromString(getRequiredString("initialFocus",pAttributes));

		if(hasAttr("initialFocusType",pAttributes))
			meInitialFocusType = initialFocusTypeFromString( getString("initialFocusType",pAttributes));
		 else meInitialFocusType = CENTER;

		if(hasAttr("movementDirection",pAttributes))
			meMovementDirection = movementDirectionFromString( getString("movementDirection",pAttributes));
		else meMovementDirection = STILL;
}

void AnimationSpriteRef::handleText( const std::string &text )
{
	mRef = text;
}


CL_DomElement 
AnimationSpriteRef::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "animationSpriteRef");
}

AnimationSpriteRef::eInitialFocus 
AnimationSpriteRef::initialFocusFromString(const std::string &str)
{
    if(str == "screen") return SCREEN;
    else if (str == "caster") return CASTER;
    else if (str == "target") return TARGET;
    else throw CL_Error("Bad initialFocus: " + str );
}

AnimationSpriteRef::eInitialFocusType 
AnimationSpriteRef::initialFocusTypeFromString ( const std::string &str )
{
    if(str == "center") return CENTER;
    else if (str == "above") return ABOVE;
    else if (str == "below") return BELOW;
    else if (str == "left")  return LEFT;
    else if (str == "right") return RIGHT;
    else if (str == "below_right") return BELOW_RIGHT;
    else if (str == "below_left") return BELOW_LEFT;
    else if (str == "above_right") return ABOVE_RIGHT;
    else if (str == "above_left") return ABOVE_LEFT;
    else throw CL_Error("Bad initial focus type: " + str );
}

AnimationSpriteRef::eMovementDirection 
AnimationSpriteRef::movementDirectionFromString ( const std::string &str )
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
    else if (str == "toTarget") return TO_TARGET;
    else if (str == "toCaster") return TO_CASTER;
}

AnimationSpriteRef::~AnimationSpriteRef()
{
}

std::string AnimationSpriteRef::getRef() const
{
    return "Sprites/Animations/" + mAnimationName + "/" + mRef; //@todo: optimize
}


AnimationSpriteRef::eInitialFocus 
AnimationSpriteRef::getInitialFocus() const
{
    return meInitialFocus;
}

AnimationSpriteRef::eInitialFocusType 
AnimationSpriteRef::getInitialFocusType() const
{
    return meInitialFocusType;
}


AnimationSpriteRef::eMovementDirection 
AnimationSpriteRef::getMovementDirection() const
{
    return meMovementDirection;
}


void Par::handleElement(eElement element, Element * pElement)
{
		switch(element)
		{
		case EPLAYSOUND:
			mpPlaySound = dynamic_cast<StoneRing::PlaySound*>(pElement);
			break;
		case EANIMATIONSPRITEREF:
			mAnimationSpriteRefs.push_back( dynamic_cast<AnimationSpriteRef*>(pElement));
			break;
		}
}

void Par::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
	mnDuration = getRequiredInt("duration", pAttributes);	
	
	if(hasAttr("hide",pAttributes))
	{
		meHide = hideFromString(getString("hide",pAttributes));
	}else meHide = NONE;


}

Par::Par():mpPlaySound(NULL)
{
}

Par::eHide 
Par::hideFromString ( const std::string &str )
{
    if(str == "none") return NONE;
    else if (str == "caster") return CASTER;
    else if (str == "caster_group") return CASTER_GROUP;
    else if (str == "target") return TARGET;
    else if (str == "target_group") return TARGET_GROUP;
    else if (str == "all") return ALL;
    else throw CL_Error("Bad Par Hide = " + str );

}

Par::~Par()
{
	delete mpPlaySound;

	std::for_each(mAnimationSpriteRefs.begin(),mAnimationSpriteRefs.end(),del_fun<AnimationSpriteRef>());
}

	
CL_DomElement 
Par::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc, "par");
}

uint Par::getDurationMs() const
{
    return mnDuration;
}



Par::eHide 
Par::getHide() const
{
    return meHide;
}

PlaySound * Par::getPlaySound() const
{
    return mpPlaySound;
}

std::list<AnimationSpriteRef*>::const_iterator 
Par::getAnimationSpriteRefsBegin() const 
{
    return mAnimationSpriteRefs.begin();
}

std::list<AnimationSpriteRef*>::const_iterator 
Par::getAnimationSpriteRefsEnd() const
{
    return mAnimationSpriteRefs.end();
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

void Animation::handleElement(eElement element, Element * pElement)
{
	if(element == EPAR )
	{
		mPars.push_back ( dynamic_cast<Par*>(pElement) );
	}
}


Animation::~Animation()
{
	std::for_each(mPars.begin(),mPars.end(),del_fun<Par>());
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


std::list<Par*>::const_iterator Animation::getParsBegin() const
{
    return mPars.begin();
}

std::list<Par*>::const_iterator Animation::getParsEnd() const
{
    return mPars.end();
}




    




