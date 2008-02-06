
#ifndef SR_WEAPONTYPE_REF
#define SR_WEAPONTYPE_REF

#include "Element.h"

namespace StoneRing{
    class WeaponTypeRef : public Element
    {
    public:
        WeaponTypeRef();
        virtual ~WeaponTypeRef();
        virtual eElement whichElement() const{ return EWEAPONTYPEREF; } 
        std::string getName() const;

        void setName(const std::string &name) { mName = name; }

        bool operator== ( const WeaponTypeRef &lhs );
    private:
        virtual void loadAttributes(CL_DomNamedNodeMap *pAttributes);
    protected:
        std::string mName;
    };
};


#endif



