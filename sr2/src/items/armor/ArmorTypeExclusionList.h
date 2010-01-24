#ifndef SR_ARMOR_TYPE_EXCLUSION_LIST_H
#define SR_ARMOR_TYPE_EXCLUSION_LIST_H

#include "Element.h"
#include "ArmorTypeRef.h"
#include <list>

namespace StoneRing{
    class ArmorTypeExclusionList: public Element
    {
    public:
        ArmorTypeExclusionList();
        virtual ~ArmorTypeExclusionList();
        virtual eElement WhichElement() const{ return EARMORTYPEEXCLUSIONLIST; }

        std::list<ArmorTypeRef*>::const_iterator GetArmorTypeRefsBegin();
        std::list<ArmorTypeRef*>::const_iterator GetArmorTypeRefsEnd();

    private:
        virtual bool handle_element(eElement element, Element * pElement);
        std::list<ArmorTypeRef*> m_ArmorTypes;

    };
};

#endif



