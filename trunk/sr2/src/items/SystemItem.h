#ifndef SR_SYSTEM_ITEM_H
#define SR_SYSTEM_ITEM_H

#include "NamedItem.h"

namespace StoneRing{  
    class SystemItem : public NamedItem
    {
    public:
        SystemItem();
        virtual ~SystemItem();
        virtual eElement whichElement() const{ return ESYSTEMITEM; }            
        virtual uint getValue() const { return 0;} // No value to system items. cant sell 'em.
        virtual uint getSellValue() const { return 0; }

        
        // We're overriding whatever was specified in the XML. Never drop a system item, ever.
        virtual eDropRarity getDropRarity() const { return NEVER; } 

        virtual eItemType getItemType() const { return SYSTEM ; }

        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

    private:

    };
};

#endif



