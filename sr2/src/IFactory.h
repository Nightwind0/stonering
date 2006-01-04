#ifndef SR_FACTORY_H
#define SR_FACTORY_H

#include <string>

#include "Element.h"

namespace StoneRing
{

class IFactory
{
public:
	IFactory(){}
	virtual ~IFactory(){}

	virtual bool canCreate( Element::eElement element )=0;
	virtual Element * createElement( Element::eElement element )=0;
};
};

#endif
