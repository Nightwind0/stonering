#ifndef SR_POP_H
#define SR_POP_H

#include "Action.h"
#include "Element.h"

namespace StoneRing{
	
	class Pop: public Action, public Element
	{
	public:
		Pop();
		virtual ~Pop();
		virtual eElement whichElement() const{ return EPOP; }	
		virtual void invoke();

		virtual CL_DomElement createDomElement(CL_DomDocument&) const;
	protected:
		virtual void loadAttributes(CL_DomNamedNodeMap*);
		bool mbAll;

	};
};

#endif

