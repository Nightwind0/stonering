#ifndef SR_INVOKE_SHOP_H
#define SR_INVOKE_SHOP_H

#include "Action.h"
#include "Element.h"

namespace StoneRing{
	class InvokeShop : public Action, public Element
	{
	public:
		InvokeShop();
		virtual ~InvokeShop();
		virtual eElement whichElement() const{ return EINVOKESHOP; }	
		virtual void invoke();
		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		std::string mShopType;
	};
};

#endif