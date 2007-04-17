#ifndef SR_ARMORTYPE_REF_H
#define SR_ARMORTYPE_REF_H


#include "Element.h"

namespace StoneRing{
    class ArmorTypeRef : public Element
    {
    public:
        ArmorTypeRef();
        virtual ~ArmorTypeRef();
        virtual eElement whichElement() const{ return EARMORTYPEREF; }  
        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

        std::string getName() const;

        void setName(const std::string &name){ mName = name; }
        bool operator==(const ArmorTypeRef &lhs );
    private:
        virtual void handleText(const std::string &text);
        std::string mName;
    };
    
};

#endif



