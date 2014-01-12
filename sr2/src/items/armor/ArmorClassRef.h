#ifndef SR_ARMORCLASS_REF
#define SR_ARMORCLASS_REF

#include "Element.h"
#include "Armor.h"

namespace StoneRing{


    class ArmorClassRef : public Element
    {
    public:
        ArmorClassRef();
        virtual ~ArmorClassRef();
        virtual eElement WhichElement() const{ return EARMORCLASSREF; }
        std::string GetName() const;
        void SetName(const std::string &name){ m_name = name; }
        bool operator==(const ArmorClassRef &lhs );
		virtual std::string GetDebugId() const { return m_name; }				
    private:
        virtual void load_attributes(clan::DomNamedNodeMap);
    protected:
        std::string m_name;
    };
    
    class ArmorImbuementRef: public ArmorClassRef
    {
    public:
        ArmorImbuementRef(){}
        virtual ~ArmorImbuementRef(){}
        virtual eElement WhichElement() const { return EARMORIMBUEMENTREF; }
    };

};


#endif




