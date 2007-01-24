#ifndef SR_REMOVE_CHARACTER_H
#define SR_REMOVE_CHARACTER_H

#include "Action.h"
#include "Element.h"

namespace StoneRing{
    class RemoveCharacter : public Action, public Element
    {
    public:
        RemoveCharacter();
        virtual ~RemoveCharacter();
        virtual void invoke();
        virtual eElement whichElement() const{ return EREMOVECHARACTER; }
        virtual CL_DomElement createDomElement(CL_DomDocument&) const;

        std::string getName() const;
        bool keepStuff() const;
    private:
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);

        std::string mName;
        bool mbKeepStuff;
    };
};


#endif

