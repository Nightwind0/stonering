#ifndef SR_ATTRIBUTE_EFFECT_H
#define SR_ATTRIBUTE_EFFECT_H

#include "Element.h"
#include "ScriptElement.h"
#include "NamedScript.h"
#include "Character.h"
#include <map>

namespace StoneRing{

    class StatusEffect : public Element
    {
    public:
        StatusEffect();
        virtual ~StatusEffect();
        typedef std::multimap<ICharacter::eCharacterAttribute,AttributeModifier*> AttributeModifierSet;
        virtual eElement WhichElement() const{ return ESTATUSEFFECT; }

        OnInvoke * GetOnInvoke() const;
        OnRound * GetOnRound() const;
        OnCountdown * GetOnCountdown() const;
        OnRemove * GetOnRemove() const;

        std::string GetName() const;
        enum eLast { ROUND_COUNT, BATTLE, PERMANENT };
        eLast GetLast() const;
        uint GetRoundCount() const;

        // Multiply the magic power of the user by this using an algorithm to get length..
        float GetLengthMultiplier() const;

        // Mainly for display, as these should be automatically invoked on equip
        AttributeModifierSet::const_iterator GetAttributeModifiersBegin() const;
        AttributeModifierSet::const_iterator GetAttributeModifiersEnd() const;

        double GetAttributeMultiplier(ICharacter::eCharacterAttribute attr) const;
        double GetAttributeAdd(ICharacter::eCharacterAttribute attr)const;

    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap attributes) ;
        std::string m_name;
        OnInvoke * m_pOnInvoke;
        OnRound * m_pOnRound;
        OnCountdown * m_pOnCountdown;
        OnRemove * m_pOnRemove;
        eLast m_eLast;
        uint m_nRoundCount;
        float m_fLengthMultiplier;
        AttributeModifierSet m_attribute_modifiers;
    };

    inline StatusEffect::AttributeModifierSet::const_iterator StatusEffect::GetAttributeModifiersBegin() const
    {
        return m_attribute_modifiers.begin();
    }

    inline StatusEffect::AttributeModifierSet::const_iterator StatusEffect::GetAttributeModifiersEnd() const
    {
        return m_attribute_modifiers.end();
    }
}

#endif




