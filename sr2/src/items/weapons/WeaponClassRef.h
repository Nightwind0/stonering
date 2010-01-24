#ifndef SR_WEAPONCLASS_REF
#define SR_WEAPONCLASS_REF

#include "Element.h"

namespace StoneRing{

    class WeaponClassRef : public Element
    {
    public:
        WeaponClassRef();
        virtual ~WeaponClassRef();
        virtual eElement WhichElement() const{ return EWEAPONCLASSREF; }
        std::string GetName() const;

        void SetName(const std::string &name){ m_name = name; }
        bool operator== (const WeaponClassRef &lhs );
    private:
        virtual void load_attributes(CL_DomNamedNodeMap attributes);
    protected:
        std::string m_name;
    };
};

#endif



