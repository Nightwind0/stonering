#ifndef SR_EQUIPMENT_H
#define SR_EQUIPMENT_H

#include "Item.h"
#if defined(WIN32) || defined(__APPLE__)
#include "SteelInterpreter.h"
#else
#include "steel/SteelInterpreter.h"
#endif
#include <list>
#include <map>

namespace StoneRing{

    class ICharacter;
    class AttributeModifier;
    class StatusEffectInfliction;
    

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
        
        static std::string GetSlotName(eSlot slot);

        RuneType * GetRuneType() const;
        bool HasRuneType() const;

        // True for armor, false for weapon
        virtual bool IsArmor() const=0;
        bool IsWeapon() const { return !IsArmor(); }

        virtual eSlot GetSlot() const=0;
        virtual void Equip(ICharacter *);
        virtual void Unequip(ICharacter *);
        virtual bool EquipCondition(const Steel::ParameterList& params)=0;
     
        // TODO: Get these from some base that Omegas can share with Equipment
        typedef std::multimap<uint,AttributeModifier*> AttributeModifierSet;
        typedef std::multimap<std::string,StatusEffectModifier*> StatusEffectModifierSet;
        typedef std::multimap<std::string,StatusEffectInfliction*> StatusEffectInflictionSet;
        // Mainly for display, as these should be automatically invoked on equip
        AttributeModifierSet::const_iterator GetAttributeModifiersBegin() const;
        AttributeModifierSet::const_iterator GetAttributeModifiersEnd() const;

        StatusEffectModifierSet::const_iterator GetStatusEffectModifiersBegin() const { return m_statuseffect_modifiers.begin(); }
        StatusEffectModifierSet::const_iterator GetStatusEffectModifiersEnd() const { return m_statuseffect_modifiers.end(); }
        StatusEffectInflictionSet::const_iterator GetStatusEffectInflictionsBegin() const { return m_statuseffect_inflictions.begin(); }
        StatusEffectInflictionSet::const_iterator GetStatusEffectInflictionsEnd() const { return m_statuseffect_inflictions.end(); }        

        double GetAttributeMultiplier(uint attr) const;
        double GetAttributeAdd(uint attr)const;
        bool   GetAttributeToggle(uint attr, bool current)const;
        
        double GetStatusEffectModifier(const std::string &statuseffect)const;   
        
    protected:
        virtual void OnEquipScript(const Steel::ParameterList& params)=0;
        virtual void OnUnequipScript(const Steel::ParameterList& params)=0;

        void Clear_Attribute_Modifiers();
        void Add_Attribute_Modifier( AttributeModifier * pAttr );
        void Clear_StatusEffect_Modifiers();
        void Add_StatusEffect_Modifier( StatusEffectModifier * pModifier );
        void Clear_StatusEffect_Inflictions();
        void Add_StatusEffect_Infliction( StatusEffectInfliction * pInfliction );
        void Set_Rune_Type ( RuneType * pType );

    private:
        AttributeModifierSet m_attribute_modifiers;
        StatusEffectModifierSet m_statuseffect_modifiers;
        StatusEffectInflictionSet m_statuseffect_inflictions;
        SpellOrRuneRef  m_SpellOrRuneRef;
        enum eMagic { NONE, SPELL, RUNE };
        eMagic m_eMagic;

    };
}

#endif



