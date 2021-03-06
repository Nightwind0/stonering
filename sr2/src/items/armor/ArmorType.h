#ifndef SR_ARMOR_TYPE_H
#define SR_ARMOR_TYPE_H

#include "Element.h"
#include "Equipment.h"

namespace StoneRing{
    class ArmorType: public Element, public Steel::SteelType::IHandle
    {
    public:
        ArmorType();
        ArmorType(clan::DomElement * pElement );
        ~ArmorType();
        virtual eElement WhichElement() const{ return EARMORTYPE; }

        std::string GetName() const;
        std::string GetIconRef() const;

        uint GetBasePrice() const;
        int GetBaseAC() const;
        int GetBaseRST() const;
        Equipment::eSlot GetSlot() const;

        bool operator==(const ArmorType &lhs );
		virtual std::string GetDebugId() const { return m_name; }		
    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(clan::DomNamedNodeMap attributes) ;
        std::string m_name;
        std::string m_icon_ref;
        uint m_nBasePrice;
        int m_nBaseAC;
        int m_nBaseRST;
        Equipment::eSlot m_eSlot;

    };
};
#endif



