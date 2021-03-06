#ifndef SR_ATTRIBUTE_EFFECT_H
#define SR_ATTRIBUTE_EFFECT_H

#include "Element.h"
#include "ScriptElement.h"
#include "NamedScript.h"
#include "Character.h"
#include <map>

namespace StoneRing{

    class StatusEffect : public Element, public SteelType::IHandle
    {
    public:
        StatusEffect();
        virtual ~StatusEffect();
		enum eLast { ROUND_COUNT, BATTLE, PERMANENT };
        typedef std::multimap<ICharacter::eCharacterAttribute,AttributeModifier*> AttributeModifierSet;
        virtual eElement WhichElement() const{ return ESTATUSEFFECT; }
        
        void Invoke(const ParameterList& params);
        void Round(const ParameterList& params);
        void Remove(const ParameterList& params);
        void Countdown(const ParameterList& params); // WTF is countdown?

        std::string GetName() const;
		clan::Sprite GetIcon() const;
        eLast GetLast() const;
        uint GetRoundCount() const;

        // Multiply the magic power of the user by this using an algorithm to get length..
        float GetLengthMultiplier() const;

        // Mainly for display, as these should be automatically invoked on equip
        AttributeModifierSet::const_iterator GetAttributeModifiersBegin() const;
        AttributeModifierSet::const_iterator GetAttributeModifiersEnd() const;

        double GetAttributeMultiplier(ICharacter::eCharacterAttribute attr) const;
        double GetAttributeAdd(ICharacter::eCharacterAttribute attr)const;
        bool GetAttributeToggle(ICharacter::eCharacterAttribute attr, bool current)const;
        
        //StatusEffectModifiers
        double GetStatusEffectModifier(const std::string &statuseffect)const;
		virtual std::string GetDebugId() const { return m_name; }				
    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(clan::DomNamedNodeMap attributes) ;
        std::string m_name;
		clan::Sprite m_icon;
        OnInvoke * m_pOnInvoke;
        OnRound * m_pOnRound;
        OnCountdown * m_pOnCountdown;
        OnRemove * m_pOnRemove;
        eLast m_eLast;
        uint m_nRoundCount;
        float m_fLengthMultiplier;
        AttributeModifierSet m_attribute_modifiers;
        // These are for modifying your chance to be afflicted by OTHER status effects
        // In other words, a status effect could make you immune to poison, for instance
        std::map<std::string,StatusEffectModifier*> m_statuseffect_modifiers;
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




