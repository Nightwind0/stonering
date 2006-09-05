#ifndef SR_ATTRIBUTE_EFFECT_H
#define SR_ATTRIBUTE_EFFECT_H

#include "Effect.h"
#include "Element.h"

namespace StoneRing{

	class AttributeEffect : public Effect, public Element
	{
	public:
		AttributeEffect();
		~AttributeEffect();

		Effect::eType getEffectType() const { return ATTRIBUTE_EFFECT; }
		virtual eElement whichElement() const{ return EATTRIBUTEEFFECT; }	
		CL_DomElement createDomElement(CL_DomDocument &doc) const ;

		std::string getAttribute() const;
		int getAdd() const;
		float getMultiplier() const;

		enum eChangeTo { MIN, MAX, ADD, MULTIPLIER, MULTIPLIER_ADD } ;

		eChangeTo getChangeTo() const;

		int getDelta() const { return mnDelta; }
	private:
		virtual void loadAttributes(CL_DomNamedNodeMap*);
		std::string mAttribute;
		int mnAdd; 
		eChangeTo meChangeTo;
		float mfMultiplier;
		int mnDelta;
	};

	class StatusEffectActions: public Element
	{
	public:
		StatusEffectActions();
		~StatusEffectActions();
		virtual eElement whichElement() const{ return ESTATUSEFFECTACTIONS; }	
		CL_DomElement createDomElement(CL_DomDocument &doc) const;

		std::list<Effect*>::const_iterator getEffectsBegin() const;
		std::list<Effect*>::const_iterator getEffectsEnd() const;

	private:
		virtual bool handleElement(eElement element, Element * pElement );
		std::list<Effect*> mEffects;

	};

	class StatusEffect : public Element
	{
	public:
		StatusEffect();
		virtual ~StatusEffect();
		virtual eElement whichElement() const{ return ESTATUSEFFECT; }	
		CL_DomElement createDomElement(CL_DomDocument &doc) const;

		StatusEffectActions * getOnInvoke() const;
		StatusEffectActions * getOnRound() const;
		StatusEffectActions * getOnCountdown() const;
		StatusEffectActions * getOnRemove() const;

		std::string getName() const;

		enum eLast { ROUND_COUNT, BATTLE, PERMANENT };

		eLast getLast() const;

		uint getRoundCount() const; 

		// Multiply the magic power of the user by this using an algorithm to get length..
		float getLengthMultiplier() const;


	private:
		virtual bool handleElement(eElement element, Element * pElement );
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
		std::string mName;
		StatusEffectActions * mpOnInvoke;
		StatusEffectActions * mpOnRound;
		StatusEffectActions * mpOnCountdown;
		StatusEffectActions * mpOnRemove;
		eLast meLast;
		uint mnRoundCount;
		float mfLengthMultiplier;
	};
}

#endif


