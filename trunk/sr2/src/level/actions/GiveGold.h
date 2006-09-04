#ifndef SR_GIVE_GOLD_H
#define SR_GIVE_GOLD_H

#include "Action.h"
#include "Element.h"

namespace StoneRing{
	class GiveGold : public Action, public Element
	{
	public:
		GiveGold();
		virtual ~GiveGold();
		virtual eElement whichElement() const{ return EGIVEGOLD; }	
		virtual void invoke();

		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		uint mCount;
	};
};

#endif