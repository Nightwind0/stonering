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
		virtual std::string GetDebugId() const { return m_name; }				
		
    private:
        virtual void load_attributes(CL_DomNamedNodeMap attributes);
    protected:
        std::string m_name;
    };
    
    class WeaponImbuementRef : public WeaponClassRef
    {
    public:
        WeaponImbuementRef(){}
        virtual ~WeaponImbuementRef(){}
        virtual eElement WhichElement() const { return EWEAPONIMBUEMENTREF; }
    };
};

#endif



