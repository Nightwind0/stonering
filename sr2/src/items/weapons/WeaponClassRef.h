#ifndef SR_WEAPONCLASS_REF
#define SR_WEAPONCLASS_REF

#include "Element.h"

namespace StoneRing{     

    class WeaponClassRef : public Element
    {
    public:
        WeaponClassRef();
        virtual ~WeaponClassRef();
        virtual eElement whichElement() const{ return EWEAPONCLASSREF; }    
        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

        std::string getName() const;

        void setName(const std::string &name){ mName = name; }
        bool operator== (const WeaponClassRef &lhs );
    private:
        virtual void handleText(const std::string &text);
        std::string mName;
    };
};

#endif

