#ifndef SR_CONDITION_H
#define SR_CONDITION_H

#include "Element.h"
#include "Check.h"

namespace StoneRing
{

	class Condition : public Element
	{
	public:
		Condition();
		virtual ~Condition();

		bool evaluate() const;
		virtual eElement whichElement() const{ return ECONDITION; }	
		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
		virtual bool handleElement(eElement element, Element * pElement );
		std::list<Check*> mChecks;
	};

};

#endif