#ifndef SR_BATTLE_MENU_OPTION_H
#define SR_BATTLE_MENU_OPTION_H

#include "Element.h"
#include "ScriptElement.h"

namespace StoneRing
{
    class BattleMenuOption : public Element
    {
    public:
        BattleMenuOption();
        virtual ~BattleMenuOption();
        virtual eElement whichElement() const { return EBATTLEMENUOPTION; }

        std::string getName() const;
        bool enabled() const;
        void invoke();
    private:
        virtual bool handleElement(eElement, Element *);
        virtual void loadAttributes(CL_DomNamedNodeMap *);
    protected:
        std::string mName;
        ScriptElement *mpConditionScript;
        ScriptElement *mpScript;
    };
};


#endif

