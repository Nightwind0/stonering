#ifndef SR_PAUSE_H
#define SR_PAUSE_H

#include "Action.h"
#include "Element.h"

namespace StoneRing{ 
	class Pause : public Action, public Element
	{
	public:
		Pause();
		virtual ~Pause();
		virtual eElement whichElement() const{ return EPAUSE; }	
		virtual void invoke();

		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
		virtual void handleText(const std::string &);
		uint mMs;
	};
};

#endif