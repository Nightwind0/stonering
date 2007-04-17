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
        std::string getName() const;
        void setName(const std::string &name){ mName = name; }
        bool operator==(const ArmorTypeRef &lhs );
    private:
        virtual void handleText(const std::string &text);
    protected:
        std::string mName;
    };
    
};

#endif



