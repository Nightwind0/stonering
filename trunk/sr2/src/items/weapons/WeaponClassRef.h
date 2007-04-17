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
        std::string getName() const;

        void setName(const std::string &name){ mName = name; }
        bool operator== (const WeaponClassRef &lhs );
    private:
        virtual void handleText(const std::string &text);
    protected:
        std::string mName;
    };
};

#endif



