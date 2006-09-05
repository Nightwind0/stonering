#ifndef SR_AND_H
#define SR_AND_H

#include "Check.h"

namespace StoneRing{
	class And : public Check
	{
	public:
		And();
		virtual ~And();

		virtual bool evaluate();
		virtual eElement whichElement() const{ return EAND; }	
		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

		ushort order();

	protected:
		virtual bool handleElement(eElement element, Element * pElement );
		ushort mOrder;
		std::list<Check*> mOperands;
	};
};

#endif

