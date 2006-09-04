#ifndef SR_GIVE_H
#define SR_GIVE_H

#include "Action.h"
#include "Element.h"

namespace StoneRing{

	class ItemRef;

	class Give: public Action, public Element
	{
	public:
		Give();
		virtual ~Give();
		virtual eElement whichElement() const{ return EGIVE; }	
		virtual void invoke();

		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
		virtual bool handleElement(eElement,Element*);
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		ItemRef *mpItemRef;
		uint mCount;
	};
};

#endif