#ifndef SR_SPECIAL_ITEM_H
#define SR_SPECIAL_ITEM_H

#include "NamedItem.h"

namespace StoneRing{
    class SpecialItem : public NamedItem
	{
	public:
	    SpecialItem();
	    virtual ~SpecialItem();
   		virtual eElement whichElement() const{ return ESPECIALITEM; }	     
	    virtual uint getValue() const { return 0;} // No value to special items. cant sell 'em.
	    virtual uint getSellValue() const { return 0; }

        
	    // We're overriding whatever was specified in the XML. Never drop a special item, ever.
	    virtual eDropRarity getDropRarity() const { return NEVER; } 

	    virtual eItemType getItemType() const { return SPECIAL ; }

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
		  
	private:
	};
};

#endif

