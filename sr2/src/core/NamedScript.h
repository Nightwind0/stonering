#ifndef SR_NAMED_SCRIPT_H
#define SR_NAMED_SCRIPT_H

#include "Element.h"
#include "ScriptElement.h"

namespace StoneRing{
    class NamedScript: public Element
    {
    public:
        NamedScript();
        virtual ~NamedScript();
        CL_DomElement createDomElement(CL_DomDocument &doc) const;

        void executeScript();

    private:
        virtual bool handleElement(eElement element, Element * pElement );
        ScriptElement *mpScript;
    };

    class OnEquip : public NamedScript
    {
    public:
        virtual ~OnEquip(){}
        virtual eElement whichElement() const { return EONEQUIP; }
    };

    class OnUnequip : public NamedScript
    {
    public:
        virtual ~OnUnequip() {}
        virtual eElement whichElement() const { return EONUNEQUIP; }
    };

    class OnRound : public NamedScript
    {
    public:
        virtual ~OnRound() {}
        virtual eElement whichElement() const { return EONROUND; }
    };

    class OnStep : public NamedScript
    {
    public:
        virtual ~OnStep() {}
        virtual eElement whichElement() const { return EONSTEP; }
    };

    class OnCountdown : public NamedScript
    {
    public:
        virtual ~OnCountdown(){}
        virtual eElement whichElement() const { return EONCOUNTDOWN; }
    };

    class OnInvoke : public NamedScript
    {
    public:
        virtual ~OnInvoke(){}
        virtual eElement whichElement() const { return EONINVOKE; }
    };

    class OnRemove : public NamedScript
    {
    public:
        virtual ~OnRemove(){}
        virtual eElement whichElement() const { return EONREMOVE; }
    };
};
#endif

