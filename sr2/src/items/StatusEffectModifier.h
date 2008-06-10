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
        virtual eElement WhichElement() const{ return ESTATUSEFFECTMODIFIER; }  
        StatusEffect * GetStatusEffect() const;
        float GetModifier() const;
        
    private:
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes) ;
        StatusEffect *m_pStatusEffect;
        float m_fModifier;
    };
};

#endif



