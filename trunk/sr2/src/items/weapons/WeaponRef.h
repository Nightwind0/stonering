#ifndef SR_WEAPON_REF_H
#define SR_WEAPON_REF_H

#include "Element.h"
#include "WeaponTypeRef.h"
#include "WeaponClassRef.h"
#include "WeaponClass.h"
#include "SpellRef.h"
#include "RuneType.h"
#include "WeaponType.h"

namespace StoneRing{


    class WeaponRef : public Element
    {
    public:
        WeaponRef();
        WeaponRef ( WeaponType *pType, WeaponClass *pClass, 
                    SpellRef * pSpell, RuneType *pRune );
        virtual ~WeaponRef();
        virtual eElement whichElement() const{ return EWEAPONREF; } 
        WeaponType * getWeaponType() const;
        WeaponClass  * getWeaponClass() const;
        SpellRef * getSpellRef() const;
        RuneType * getRuneType() const;
        std::string getName() const;
    private:
        virtual bool handleElement(eElement element, Element * pElement );
        virtual void loadFinished();
    protected:
        WeaponType *mpWeaponType;
        WeaponClass *mpWeaponClass;
        WeaponTypeRef *mpType;
        WeaponClassRef *mpClass;
        SpellRef * mpSpellRef;
        RuneType * mpRuneType;
        std::string mName;

    };
};

#endif



