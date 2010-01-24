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
        virtual eElement WhichElement() const{ return EWEAPONREF; } 
        WeaponType * GetWeaponType() const;
        WeaponClass  * GetWeaponClass() const;
        SpellRef * GetSpellRef() const;
        RuneType * GetRuneType() const;
        std::string GetName() const;
    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_finished();
    protected:
        WeaponType *m_pWeaponType;
        WeaponClass *m_pWeaponClass;
        WeaponTypeRef *m_pType;
        WeaponClassRef *m_pClass;
        SpellRef * m_pSpellRef;
        RuneType * m_pRuneType;
        std::string m_name;

    };
};

#endif



