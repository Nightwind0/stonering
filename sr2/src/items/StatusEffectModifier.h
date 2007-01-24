#ifndef SR_STATUS_EFFECT_MODIFIER_H
#define SR_STATUS_EFFECT_MODIFIER_H

#include "Element.h"
#include "StatusEffect.h"

namespace StoneRing{
    class StatusEffectModifier : public Element
	{
	public:
	    StatusEffectModifier();
	    StatusEffectModifier(CL_DomElement *pElement);
	    virtual ~StatusEffectModifier();
		virtual eElement whichElement() const{ return ESTATUSEFFECTMODIFIER; }	
	    StatusEffect * getStatusEffect() const;
	    float getModifier() const;
	    

	    CL_DomElement createDomElement(CL_DomDocument&) const;

	private:
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    StatusEffect *mpStatusEffect;
	    float mfModifier;
	};
};

#endif

