
#include "Operator.h"

using namespace StoneRing;

Operator::Operator()
{
}

CL_DomElement  Operator::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"operator");

    for(std::list<Check*>::const_iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        element.append_child (  (*i)->createDomElement(doc) );
    }

    return element;    

}

bool Operator::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EOR:
    case EAND:
        mOperands.push_back(dynamic_cast<Check*>(pElement));
        break;
    default:
        return false;
    }   
    return true;
}

Operator::~Operator()
{
    for(std::list<Check*>::iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        delete *i;
    }
    
}

bool Operator::evaluate()
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

ushort Operator::order()
{
    return 0;
}
 

