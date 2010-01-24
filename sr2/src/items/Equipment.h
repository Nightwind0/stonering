#ifndef SR_EQUIPMENT_H
#define SR_EQUIPMENT_H

#include "Item.h"
#include <list>
#include <map>

namespace StoneRing{

    class ICharacter;
    class AttributeModifier;

    class Equipment : public virtual Item
    {
    public:
        Equipment();
        virtual ~Equipment();

        enum eSlot
        {
            EHAND=1,
            EOFFHAND=2,
            EHEAD=4,
            EHANDS=8,
            EBODY=16,
            EFINGER1=32,
            EFINGER2=64,
            EFEET=128,
            EANYFINGER = (EFINGER1 | EFINGER2),
            EANYHAND = (EHAND | EOFFHAND),
            EANYARMOR = (EHEAD | EHANDS | EBODY | EANYFINGER | EFEET),
            EANY = (EANYHAND | EANYARMOR)
        };

        SpellRef * GetSpellRef() const;
        RuneType * GetRuneType() const;
        bool HasSpell() const ;
        bool HasRuneType() const;

        // True for armor, false for weapon
        virtual bool IsArmor() const=0;
        bool IsWeapon() const { return !IsArmor(); }

        virtual void Equip(ICharacter *);
        virtual void Unequip(ICharacter *);
        virtual void Invoke()=0;
        virtual bool EquipCondition()=0;
        typedef std::multimap<uint,AttributeModifier*> AttributeModifierSet;
        // Mainly for display, as these should be automatically invoked on equip
        AttributeModifierSet::const_iterator GetAttributeModifiersBegin() const;
        AttributeModifierSet::const_iterator GetAttributeModifiersEnd() const;

        std::list<StatusEffectModifier*>::const_iterator GetStatusEffectModifiersBegin() const { return m_status_effect_modifiers.begin(); }
        std::list<StatusEffectModifier*>::const_iterator GetStatusEffectModifiersEnd() const { return m_status_effect_modifiers.end(); }

        double GetAttributeMultiplier(uint attr) const;
        double GetAttributeAdd(uint attr)const;
    protected:
        virtual void OnEquipScript()=0;
        virtual void OnUnequipScript()=0;

        void Clear_Attribute_Modifiers();
        void Add_Attribute_Modifier( AttributeModifier * pAttr );
        void Set_Spell_Ref ( SpellRef * pRef );
        void Set_Rune_Type ( RuneType * pType );
        void Add_Status_Effect_Modifier(StatusEffectModifier *pModifier) { m_status_effect_modifiers.push_back ( pModifier ) ; }

    private:
        AttributeModifierSet m_attribute_modifiers;
        SpellOrRuneRef  m_SpellOrRuneRef;
        enum eMagic { NONE, SPELL, RUNE };
        eMagic m_eMagic;
        std::list<StatusEffectModifier*> m_status_effect_modifiers;

    };
}

#endif



