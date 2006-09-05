#include "Or.h"

using namespace StoneRing;


Or::Or()
{
}

CL_DomElement  Or::createDomElement(CL_DomDocument &doc) const
{

    CL_DomElement  element(doc,"or");

    for(std::list<Check*>::const_iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        CL_DomElement e = (*i)->createDomElement(doc);
        element.append_child ( e );


    }

    return element;
}


bool Or::handleElement(eElement element, Element *pElement)
{
    switch(element)
    {
    case EOPERATOR:
    case EHASITEM:
    case EHASGOLD:
    case EDIDEVENT:
        mOperands.push_back(dynamic_cast<Check*>(pElement));
		return false;
    }
	return true;
}
 
Or::~Or()
{
    for(std::list<Check*>::iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        delete *i;
    }
}

bool Or::evaluate()
{
    for(std::list<Check*>::const_iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        Check * check = *i;
        if( check->evaluate() )
            return true;
    }

    return false;
}

ushort Or::order()
{
    return 0;
}

