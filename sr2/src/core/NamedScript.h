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

        SteelType executeScript();
        SteelType executeScript(const ParameterList &params);
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

    class OnSelect : public NamedScript
    {
    public:
        virtual ~OnSelect(){}
        virtual eElement whichElement() const { return EONSELECT; }
    };

    class OnDeselect : public NamedScript
    {
    public:
        virtual ~OnDeselect(){}
        virtual eElement whichElement() const { return EONDESELECT; }
    };
};
#endif
