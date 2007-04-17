#ifndef SR_ARMOR_TYPE_EXCLUSION_LIST_H
#define SR_ARMOR_TYPE_EXCLUSION_LIST_H

#include "Element.h"
#include "ArmorTypeRef.h"

namespace StoneRing{
    class ArmorTypeExclusionList: public Element
    {
    public:
        ArmorTypeExclusionList();
        virtual ~ArmorTypeExclusionList();
        virtual CL_DomElement createDomElement ( CL_DomDocument &) const;
        virtual eElement whichElement() const{ return EARMORTYPEEXCLUSIONLIST; }    

        std::list<ArmorTypeRef*>::const_iterator getArmorTypeRefsBegin();
        std::list<ArmorTypeRef*>::const_iterator getArmorTypeRefsEnd();

        virtual bool handleElement(eElement element, Element * pElement);
    private:
        std::list<ArmorTypeRef*> mArmorTypes;

    };
};

#endif



