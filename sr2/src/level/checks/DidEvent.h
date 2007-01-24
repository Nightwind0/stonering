#ifndef SR_DID_EVENT_H
#define SR_DID_EVENT_H

#include "Check.h"

namespace StoneRing{
    class DidEvent : public Check
    {
    public:
        DidEvent();
        virtual ~DidEvent();
        virtual eElement whichElement() const{ return EDIDEVENT; }  
        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
        virtual bool evaluate();
    protected:
        virtual bool handleElement(eElement element, Element * pElement );      
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        virtual void handleText(const std::string &);
        bool mbNot;
        std::string mEvent;

    };
};
#endif

