#ifndef SR_SPECIAL_ITEM_H
#define SR_SPECIAL_ITEM_H

#include "NamedItem.h"

namespace StoneRing{
    class SpecialItem : public NamedItemElement, public Item
    {
    public:
        SpecialItem();
        virtual ~SpecialItem();
        virtual eElement WhichElement() const{ return ESPECIALITEM; }        
        virtual uint GetValue() const { return 0;} // No value to special items. cant sell 'em.
        virtual uint GetSellValue() const { return 0; }
        virtual std::string GetName() const { return NamedItemElement::GetName(); }
        virtual uint GetMaxInventory() const { return 999; }
        virtual std::string GetDescription() const { return m_description; }
        virtual CL_Image GetIcon() const { return NamedItemElement::GetIcon(); }
        // We're overriding whatever was specified in the XML. Never drop a special item, ever.
        virtual eDropRarity GetDropRarity() const { return NEVER; } 
        virtual eItemType GetItemType() const { return SPECIAL ; }
        
        virtual bool operator==(const ItemRef&);
    private:
        virtual bool handle_element(eElement element, Element * pElement );        
        std::string m_description;
    };
};

#endif



