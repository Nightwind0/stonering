#ifndef SR_ARMORTYPE_REF_H
#define SR_ARMORTYPE_REF_H


#include "Element.h"

namespace StoneRing{
    class ArmorTypeRef : public Element
    {
    public:
        ArmorTypeRef();
        virtual ~ArmorTypeRef();
        virtual eElement WhichElement() const{ return EARMORTYPEREF; }  
        std::string GetName() const;
        void SetName(const std::string &name){ m_name = name; }
        bool operator==(const ArmorTypeRef &lhs );
    private:
        virtual void load_attributes(CL_DomNamedNodeMap *pAttributes);
    protected:
        std::string m_name;
    };
    
};

#endif



