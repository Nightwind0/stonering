#ifndef SR_OPERATOR_H
#define SR_OPERATOR_H


#include "Check.h"

namespace StoneRing{

class Operator : public Check
    {
    public:
        Operator();
        virtual ~Operator();
        virtual eElement whichElement() const{ return EOPERATOR; }  
        virtual bool evaluate();
        ushort order();
        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

    protected:
        virtual bool handleElement(eElement element, Element * pElement );
        ushort mOrder;
        std::list<Check*> mOperands;
    };

};

#endif

