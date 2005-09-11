#include <ClanLib/core.h>
#include <list>
#include "Animation.h"
#include "LevelFactory.h"
#include "IApplication.h"


using namespace StoneRing;




AnimationSpriteRef::AnimationSpriteRef()
{
}

AnimationSpriteRef::AnimationSpriteRef(CL_DomElement *pElement, const std::string &animation_name )
{
    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    if(!attributes.get_named_item("initialFocus").is_null())
	meInitialFocus = initialFocusFromString(attributes.get_named_item("initialFocus").get_node_value());
    else throw CL_Error("Initial Focus attribute is required on animation sprite ref.");

    if(!attributes.get_named_item("initialFocusType").is_null())
	meInitialFocusType = initialFocusTypeFromString ( attributes.get_named_item("initialFocusType").get_node_value());
    else meInitialFocusType = CENTER;


    if(!attributes.get_named_item("movementDirection").is_null())
	meMovementDirection = movementDirectionFromString ( attributes.get_named_item("movementDirection").get_node_value());
    else meMovementDirection = STILL;


    mRef = pElement->get_text();

    

    
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






Par::Par()
{
}

Par::Par(CL_DomElement * pElement, const std::string &animation_name ):mpPlaySound(NULL)
{
    LevelFactory * pFactory = IApplication::getInstance()->getLevelFactory();

    CL_DomNamedNodeMap attributes = pElement->get_attributes();
    
    if(!attributes.get_named_item("duration").is_null())
	mnDuration = atoi(attributes.get_named_item("duration").get_node_value().c_str());
    else throw CL_Error("Duration is required on par elements.");

    meHide = NONE;


    if(!attributes.get_named_item("hide").is_null())
	meHide = hideFromString ( attributes.get_named_item("hide").get_node_value());
    
    CL_DomElement child = pElement->get_first_child().to_element();
    
    while(!child.is_null())
    {
	std::string childName = child.get_node_name();

	if(childName == "playSound")
	{
	    mpPlaySound = pFactory->createPlaySound ( &child );
	}
	else if ( childName == "animationSpriteRef")
	{
	    mAnimationSpriteRefs.push_back ( new AnimationSpriteRef ( &child, animation_name ) );
	}

	child = child.get_next_sibling().to_element();
    }
    
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

Animation::Animation( CL_DomElement * pElement)
{
    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    if(!attributes.get_named_item("name").is_null())
	mName = attributes.get_named_item("duration").get_node_value();
    else throw CL_Error("Name is required on animation elements.");

    if(!attributes.get_named_item("type").is_null())
    {
	std::string type = attributes.get_named_item("type").get_node_value();

	if(type == "battle") meType = BATTLE;
	else if (type == "world") meType = WORLD;
	else throw CL_Error("Bogus animation type: " + type );
    }
    else throw CL_Error("Type is required on animation elements.");

    
    CL_DomElement child = pElement->get_first_child().to_element();

    while(!child.is_null())
    {
	if(child.get_node_name() == "par")
	{
	    mPars.push_back ( new Par( &child, mName ) );
	}
	else throw CL_Error("Wacky child found in animation : " + child.get_node_name());

	child = child.get_next_sibling().to_element();

    }

	
}

Animation::~Animation()
{
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




    




