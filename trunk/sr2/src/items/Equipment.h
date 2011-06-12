#ifndef SR_EQUIPMENT_H
#define SR_EQUIPMENT_H

#include "Item.h"
#include "steel/SteelInterpreter.h"
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

        RuneType * GetRuneType() const;
        bool HasRuneType() const;

        // True for armor, false for weapon
        virtual bool IsArmor() const=0;
        bool IsWeapon() const { return !IsArmor(); }

        virtual void Equip(ICharacter *);
        virtual void Unequip(ICharacter *);
        virtual bool EquipCondition(const ParameterList& params)=0;
     
        typedef std::multimap<uint,AttributeModifier*> AttributeModifierSet;
        typedef std::map<std::string,StatusEffectModifier*> StatusEffectModifierSet;
        // Mainly for display, as these should be automatically invoked on equip
        AttributeModifierSet::const_iterator GetAttributeModifiersBegin() const;
        AttributeModifierSet::const_iterator GetAttributeModifiersEnd() const;

        StatusEffectModifierSet::const_iterator GetStatusEffectModifiersBegin() const { return m_statuseffect_modifiers.begin(); }
        StatusEffectModifierSet::const_iterator GetStatusEffectModifiersEnd() const { return m_statuseffect_modifiers.end(); }

        double GetAttributeMultiplier(uint attr) const;
        double GetAttributeAdd(uint attr)const;
        
        double GetStatusEffectModifier(const std::string &statuseffect)const;
    protected:
        virtual void OnEquipScript(const ParameterList& params)=0;
        virtual void OnUnequipScript(const ParameterList& params)=0;

        void Clear_Attribute_Modifiers();
        void Add_Attribute_Modifier( AttributeModifier * pAttr );
        void Clear_StatusEffect_Modifiers();
        void Add_StatusEffect_Modifier( StatusEffectModifier * pModifier );
        void Set_Rune_Type ( RuneType * pType );

    private:
        AttributeModifierSet m_attribute_modifiers;
        StatusEffectModifierSet m_statuseffect_modifiers;
        SpellOrRuneRef  m_SpellOrRuneRef;
        enum eMagic { NONE, SPELL, RUNE };
        eMagic m_eMagic;

    };
}

#endif



