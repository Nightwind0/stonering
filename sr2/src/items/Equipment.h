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
                
        SpellRef * getSpellRef() const;
        RuneType * getRuneType() const;
        bool hasSpell() const ;
        bool hasRuneType() const;

        void equip(ICharacter *);
        void unequip(ICharacter *);
        virtual void executeScript()=0;
        virtual bool equipCondition()=0;
        // Mainly for display, as these should be automatically invoked on equip
        std::list<AttributeEnhancer*>::const_iterator getAttributeEnhancersBegin() const;
        std::list<AttributeEnhancer*>::const_iterator getAttributeEnhancersEnd() const;

        std::list<StatusEffectModifier*>::const_iterator getStatusEffectModifiersBegin() const { return mStatusEffectModifiers.begin(); }
        std::list<StatusEffectModifier*>::const_iterator getStatusEffectModifiersEnd() const { return mStatusEffectModifiers.end(); }
    protected:
        virtual void onEquipScript()=0;
        virtual void onUnequipScript()=0;

        void clearAttributeEnhancers();
        void addAttributeEnhancer( AttributeEnhancer * pAttr );
        void setSpellRef ( SpellRef * pRef );
        void setRuneType ( RuneType * pType );
        void addStatusEffectModifier(StatusEffectModifier *pModifier) { mStatusEffectModifiers.push_back ( pModifier ) ; }
    
    private:
        std::list<AttributeEnhancer*> mAttributeEnhancers;
        SpellOrRuneRef  mSpellOrRuneRef;
        enum eMagic { NONE, SPELL, RUNE };
        eMagic meMagic;
        std::list<StatusEffectModifier*> mStatusEffectModifiers;
        
    };
};

#endif



