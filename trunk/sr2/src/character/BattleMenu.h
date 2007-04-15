#ifndef SR_BATTLE_MENU_H
#define SR_BATTLE_MENU_H

#include <list>
#include "Element.h"

namespace StoneRing
{
    class BattleMenuOption;

    class BattleMenu : public Element
    {
    public:
        BattleMenu();
        virtual ~BattleMenu();
        virtual eElement whichElement() const { return EBATTLEMENU; }

        std::list<BattleMenuOption*>::const_iterator getOptionsBegin() const;
        std::list<BattleMenuOption*>::const_iterator getOptionsEnd() const;
    private:
        virtual bool handleElement(eElement, Element *);
        virtual void loadAttributes(CL_DomNamedNodeMap *);

        std::list<BattleMenuOption*> mOptions;
    };
};

#endif