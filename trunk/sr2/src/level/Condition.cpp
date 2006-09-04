#include "Condition.h"

using namespace StoneRing;

Condition::Condition()
{
}

CL_DomElement  Condition::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"condition");

    for(std::list<Check*>::const_iterator i = mChecks.begin();
        i != mChecks.end();
        i++)
    {
        CL_DomElement e= (*i)->createDomElement(doc);
        element.append_child ( e );

    }

    return element;
}


bool Condition::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EOPERATOR:
    case EHASITEM:
    case EHASGOLD:
    case EDIDEVENT:
        mChecks.push_back(dynamic_cast<Check*>(pElement));
        break;
	default:
		return false;
    }
	return true;
}
 

Condition::~Condition()
{
    for(std::list<Check*>::iterator i = mChecks.begin();
        i != mChecks.end(); i++)
    {
        delete *i;
    }
}

bool Condition::evaluate() const
{
    for(std::list<Check*>::const_iterator i = mChecks.begin();
        i != mChecks.end(); i++)
    {
        Check * check = *i;
        // If anybody returns false, the whole condition is false
        if(! check->evaluate() ) return false;
    }

    return true;
}
