#ifndef SR_ARMOR_REF_H
#define SR_ARMOR_REF_H

#include "Element.h"
#include "ArmorTypeRef.h"
#include "ArmorClassRef.h"
#include "ArmorType.h"
#include "ArmorClass.h"
#include "SpellRef.h"
#include "RuneType.h"

namespace StoneRing{
    class ArmorRef : public Element
    {
    public:
        ArmorRef();
        ArmorRef ( ArmorType *pType, ArmorClass *pClass, 
                   SpellRef * pSpell, RuneType *pRune );

        ~ArmorRef();
        virtual eElement whichElement() const{ return EARMORREF; }  

        ArmorType * getArmorType() const;
        ArmorClass * getArmorClass() const;
        SpellRef * getSpellRef() const;
        RuneType * getRuneType() const;

        bool operator==(const ArmorRef &lhs );

    private:
        virtual bool handleElement(eElement element, Element * pElement );

    protected:
        ArmorType * mpArmorType;
        ArmorClass * mpArmorClass;
        ArmorTypeRef *mpType;
        ArmorClassRef *mpClass;
        SpellRef * mpSpellRef;
        RuneType * mpRuneType;

    };
};

#endif



