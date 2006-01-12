#ifndef SR_ANIMATION_H
#define SR_ANIMATION_H


#include "Element.h"
#include <ClanLib/core.h>
#include <list>
#include "sr_defines.h"
#include "Effect.h"


// MS has a "PlaySound" so I nuke it
#ifdef _MSC_VER
#undef PlaySound
#endif

namespace StoneRing{

    class PlaySound;

 
    class AnimationSpriteRef : public Element
    {
    public:
	AnimationSpriteRef();
	virtual ~AnimationSpriteRef();

	std::string getRef() const;

	enum eInitialFocus { SCREEN, CASTER, TARGET };
	enum eInitialFocusType {CENTER, ABOVE, BELOW, LEFT, RIGHT, BELOW_RIGHT, BELOW_LEFT, ABOVE_RIGHT, ABOVE_LEFT };
	enum eMovementDirection { STILL, N, E, S, W, NE, NW, SE, SW, TO_TARGET, TO_CASTER };
	enum eMovementStyle {STRAIGHT, ARC_OVER, ARC_UNDER, SINE };

	eInitialFocus getInitialFocus() const;
	eInitialFocusType getInitialFocusType() const;
	eMovementDirection getMovementDirection() const;
	eMovementStyle getMovementStyle() const;
	CL_DomElement createDomElement(CL_DomDocument &doc) const;

	private:
	virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
//	virtual void handleElement(eElement element, Element * pElement);
	virtual void handleText(const std::string &text);

    private:

	eInitialFocus initialFocusFromString(const std::string &str);
	eInitialFocusType initialFocusTypeFromString ( const std::string &str );
	eMovementDirection movementDirectionFromString ( const std::string &str );
	eMovementStyle     movementStyleFromString( const std::string &str );
	std::string mAnimationName;
	std::string mRef;
	eInitialFocus meInitialFocus;
	eInitialFocusType meInitialFocusType;
	eMovementDirection meMovementDirection;
	eMovementStyle meMovementStyle;
    };

    class Par : public Element
    {
    public:
	Par();

	virtual ~Par();
	
	CL_DomElement createDomElement(CL_DomDocument &) const;
	
	uint getDurationMs() const;

	enum eHide {NONE, CASTER, CASTER_GROUP, TARGET, TARGET_GROUP, ALL };

	eHide getHide() const;
	
	PlaySound * getPlaySound() const;

	std::list<AnimationSpriteRef*>::const_iterator getAnimationSpriteRefsBegin() const ;
	std::list<AnimationSpriteRef*>::const_iterator getAnimationSpriteRefsEnd() const;

    private:
	virtual void handleElement(eElement element, Element * pElement );
	virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
	eHide hideFromString(const std::string &str);
	uint mnDuration;
	eHide meHide;
	PlaySound * mpPlaySound;
	std::list<AnimationSpriteRef*> mAnimationSpriteRefs;
    };

    
    class Animation : public Element, public Effect
    {
    public:
	Animation();
	virtual ~Animation();
	
	CL_DomElement createDomElement(CL_DomDocument &) const;
	
	std::string getName() const;
	
	enum eType { BATTLE, WORLD };

	eType getType() const;

	Effect::eType getEffectType() const { return Effect::ANIMATION; }


	std::list<Par*>::const_iterator getParsBegin() const;
	std::list<Par*>::const_iterator getParsEnd() const;

    private:
	virtual void handleElement(eElement element, Element * pElement );
	virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
	std::list<Par*> mPars;
	eType meType;
	std::string mName;
    };
    




};

#endif
