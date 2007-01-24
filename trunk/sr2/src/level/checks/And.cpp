#include "And.h"

using namespace StoneRing;

And::And()
{
}

CL_DomElement  And::createDomElement(CL_DomDocument &doc) const
{

    CL_DomElement  element(doc,"and");

    for(std::list<Check*>::const_iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        CL_DomElement e = (*i)->createDomElement(doc);
        element.append_child ( e );

    }

    return element;
}

bool And::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EOPERATOR:
    case EHASITEM:
    case EHASGOLD:
    case EDIDEVENT:
    
        mOperands.push_back(dynamic_cast<Check*>(pElement));
        break;
    default:
        return false;
    }
    return true;
}

And::~And()
{
    for(std::list<Check*>::iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        delete *i;
    }
}

bool And::evaluate()
{
    for(std::list<Check*>::const_iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        Check * check = *i;
        if(! check->evaluate() )
            return false;
    }

    return true;
}

ushort And::order()
{
    return 0;
}

