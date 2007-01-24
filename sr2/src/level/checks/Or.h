#ifndef SR_OR_H
#define SR_OR_H

#include "Check.h"

namespace StoneRing{

    class Or : public Check
    {
    public:
        Or();
        virtual ~Or();

        virtual bool evaluate();
        virtual eElement whichElement() const{ return EOR; }    
        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

        ushort order();

    protected:
        virtual bool handleElement(eElement element, Element * pElement );
        ushort mOrder;
        std::list<Check*> mOperands;

    };

};

#endif

