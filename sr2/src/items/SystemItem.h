#ifndef SR_SYSTEM_ITEM_H
#define SR_SYSTEM_ITEM_H

#include "NamedItem.h"

namespace StoneRing{  
    class SystemItem : public NamedItemElement, public Item
    {
    public:
        SystemItem();
        virtual ~SystemItem();
        virtual eElement WhichElement() const{ return ESYSTEMITEM; }            
        virtual uint GetValue() const { return 0;} // No value to system items. cant sell 'em.
        virtual uint GetSellValue() const { return 0; }
        virtual std::string GetName() const { return NamedItemElement::GetName(); }
        virtual CL_Image GetIcon() const { return NamedItemElement::GetIcon(); }
        virtual uint GetMaxInventory() const { return 999; }
        virtual std::string GetDescription() const { return "System use"; }
        
        // We're overriding whatever was specified in the XML. Never drop a system item, ever.
        virtual eDropRarity GetDropRarity() const { return NEVER; } 
        virtual eItemType GetItemType() const { return SYSTEM ; }
        
        virtual bool operator == ( const ItemRef &ref );
    private:

    };
};

#endif



