#ifndef SR_SAY_H
#define SR_SAY_H

#include "Action.h"
#include "Element.h"

namespace StoneRing{     
    class Say : public Action, public Element
    {
    public:
        Say();
        virtual ~Say();
        virtual eElement whichElement() const{ return ESAY; }   
        virtual void invoke();
        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

    protected:
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        virtual void handleText(const std::string &);
        std::string mSpeaker;
        std::string mText;
    };
};

#endif

