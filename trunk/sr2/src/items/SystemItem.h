#ifndef SR_SYSTEM_ITEM_H
#define SR_SYSTEM_ITEM_H

#include "NamedItem.h"

namespace StoneRing{  
    class SystemItem : public NamedItem
    {
    public:
        SystemItem();
        virtual ~SystemItem();
        virtual eElement WhichElement() const{ return ESYSTEMITEM; }            
        virtual uint GetValue() const { return 0;} // No value to system items. cant sell 'em.
        virtual uint GetSellValue() const { return 0; }

        
        // We're overriding whatever was specified in the XML. Never drop a system item, ever.
        virtual eDropRarity GetDropRarity() const { return NEVER; } 

        virtual eItemType GetItemType() const { return SYSTEM ; }
    private:

    };
};

#endif



