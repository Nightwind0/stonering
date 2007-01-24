#ifndef SR_EQUIPMENT_H
#define SR_EQUIPMENT_H

#include "Item.h"

namespace StoneRing{
  
    class Equipment : public virtual Item
    {
    public:
        Equipment();
        virtual ~Equipment();


        SpellRef * getSpellRef() const;
        RuneType * getRuneType() const;
        
        bool hasSpell() const ;
        bool hasRuneType() const;

        // Apply any attribute enhancements 
        void equip();

        // Remove any attribute enhancements
        void unequip();


        std::list<AttributeEnhancer*>::const_iterator getAttributeEnhancersBegin() const;
        std::list<AttributeEnhancer*>::const_iterator getAttributeEnhancersEnd() const;

        std::list<StatusEffectModifier*>::const_iterator getStatusEffectModifiersBegin() const { return mStatusEffectModifiers.begin(); }
        std::list<StatusEffectModifier*>::const_iterator getStatusEffectModifiersEnd() const { return mStatusEffectModifiers.end(); }
    protected:
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

