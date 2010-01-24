#ifndef SR_SPECIAL_ITEM_H
#define SR_SPECIAL_ITEM_H

#include "NamedItem.h"

namespace StoneRing{
    class SpecialItem : public NamedItem
    {
    public:
        SpecialItem();
        virtual ~SpecialItem();
        virtual eElement WhichElement() const{ return ESPECIALITEM; }        
        virtual uint GetValue() const { return 0;} // No value to special items. cant sell 'em.
        virtual uint GetSellValue() const { return 0; }

        
        // We're overriding whatever was specified in the XML. Never drop a special item, ever.
        virtual eDropRarity GetDropRarity() const { return NEVER; } 

        virtual eItemType GetItemType() const { return SPECIAL ; }
    private:
    };
};

#endif



