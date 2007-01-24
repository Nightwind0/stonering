#ifndef SR_TAKE_H
#define SR_TAKE_H

#include "Action.h"
#include "Element.h"

namespace StoneRing{     
    class ItemRef;

    class Take : public Action, public Element
    {
    public:
        Take();
        virtual ~Take();
        virtual eElement whichElement() const{ return ETAKE; }  
        virtual void invoke();

        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

    protected:
        virtual bool handleElement(eElement element, Element * pElement );
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        ItemRef *mpItemRef;
        uint mCount;
    };
};

#endif

