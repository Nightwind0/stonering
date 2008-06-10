#ifndef SR_EQUIPMENT_H
#define SR_EQUIPMENT_H

#include "Item.h"

namespace StoneRing{

    class ICharacter;
  
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

        void Equip(ICharacter *);
        void Unequip(ICharacter *);
        virtual void ExecuteScript()=0;
        virtual bool EquipCondition()=0;
        // Mainly for display, as these should be automatically invoked on equip
        std::list<AttributeEnhancer*>::const_iterator GetAttributeEnhancersBegin() const;
        std::list<AttributeEnhancer*>::const_iterator GetAttributeEnhancersEnd() const;

        std::list<StatusEffectModifier*>::const_iterator GetStatusEffectModifiersBegin() const { return m_status_effect_modifiers.begin(); }
        std::list<StatusEffectModifier*>::const_iterator GetStatusEffectModifiersEnd() const { return m_status_effect_modifiers.end(); }
    protected:
        virtual void OnEquipScript()=0;
        virtual void OnUnequipScript()=0;

        void Clear_Attribute_Enhancers();
        void Add_Attribute_Enhancer( AttributeEnhancer * pAttr );
        void Set_Spell_Ref ( SpellRef * pRef );
        void Set_Rune_Type ( RuneType * pType );
        void Add_Status_Effect_Modifier(StatusEffectModifier *pModifier) { m_status_effect_modifiers.push_back ( pModifier ) ; }
    
    private:
        std::list<AttributeEnhancer*> m_attribute_enhancers;
        SpellOrRuneRef  m_SpellOrRuneRef;
        enum eMagic { NONE, SPELL, RUNE };
        eMagic m_eMagic;
        std::list<StatusEffectModifier*> m_status_effect_modifiers;
        
    };
};

#endif



