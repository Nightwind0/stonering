
#ifndef SR_WEAPONTYPE_REF
#define SR_WEAPONTYPE_REF

#include "Element.h"

namespace StoneRing{
    class WeaponTypeRef : public Element
    {
    public:
        WeaponTypeRef();
        virtual ~WeaponTypeRef();
        virtual eElement WhichElement() const{ return EWEAPONTYPEREF; }
        std::string GetName() const;

        void SetName(const std::string &name) { m_name = name; }

        bool operator== ( const WeaponTypeRef &lhs );
		virtual std::string GetDebugId() const { return m_name; }				
		
    private:
        virtual void load_attributes(clan::DomNamedNodeMap attributes);
    protected:
        std::string m_name;
    };
};


#endif



