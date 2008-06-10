#ifndef SR_WEAPON_TYPE_EXCLUSION_LIST_H
#define SR_WEAPON_TYPE_EXCLUSION_LIST_H

#include "Element.h"
#include "WeaponTypeRef.h"
#include <list>

namespace StoneRing{
    class WeaponTypeExclusionList: public Element
    {
    public:
        WeaponTypeExclusionList();
        virtual ~WeaponTypeExclusionList();

        virtual eElement WhichElement() const{ return EWEAPONTYPEEXCLUSIONLIST; }   
        std::list<WeaponTypeRef*>::const_iterator GetWeaponTypeRefsBegin();
        std::list<WeaponTypeRef*>::const_iterator GetWeaponTypeRefsEnd();
    private:
        virtual bool handle_element(eElement element, Element * pElement);
        std::list<WeaponTypeRef*> m_WeaponTypes;

    };
};

#endif



