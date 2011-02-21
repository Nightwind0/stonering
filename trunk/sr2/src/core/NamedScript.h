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

        SteelType ExecuteScript();
        SteelType ExecuteScript(const ParameterList &params);
    private:
        virtual bool handle_element(eElement element, Element * pElement );
        ScriptElement *m_pScript;
    };

    class OnEquip : public NamedScript
    {
    public:
        virtual ~OnEquip(){}
        virtual eElement WhichElement() const { return EONEQUIP; }
    };

    class OnUnequip : public NamedScript
    {
    public:
        virtual ~OnUnequip() {}
        virtual eElement WhichElement() const { return EONUNEQUIP; }
    };

    class OnRound : public NamedScript
    {
    public:
        virtual ~OnRound() {}
        virtual eElement WhichElement() const { return EONROUND; }
    };

    class OnStep : public NamedScript
    {
    public:
        virtual ~OnStep() {}
        virtual eElement WhichElement() const { return EONSTEP; }
    };

    class OnCountdown : public NamedScript
    {
    public:
        virtual ~OnCountdown(){}
        virtual eElement WhichElement() const { return EONCOUNTDOWN; }
    };

    class OnInvoke : public NamedScript
    {
    public:
        virtual ~OnInvoke(){}
        virtual eElement WhichElement() const { return EONINVOKE; }
    };

    class OnRemove : public NamedScript
    {
    public:
        virtual ~OnRemove(){}
        virtual eElement WhichElement() const { return EONREMOVE; }
    };

    class OnSelect : public NamedScript
    {
    public:
        virtual ~OnSelect(){}
        virtual eElement WhichElement() const { return EONSELECT; }
    };

    class OnDeselect : public NamedScript
    {
    public:
        virtual ~OnDeselect(){}
        virtual eElement WhichElement() const { return EONDESELECT; }
    };

}
#endif

