#ifndef SR_HAS_ITEM_H
#define SR_HAS_ITEM_H   

#include "Element.h"
#include "Check.h"

namespace StoneRing{

	class HasItem : public Check
	{
	public:
		HasItem();
		virtual ~HasItem();
		virtual eElement whichElement() const{ return EHASITEM; }	
		virtual bool evaluate();
		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
		virtual bool handleElement(eElement element, Element * pElement );
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		ItemRef * mpItemRef;
		bool mbNot;
		std::string mItem;
		Item::eItemType mItemType;
		uint mCount;
	};
};

#endif