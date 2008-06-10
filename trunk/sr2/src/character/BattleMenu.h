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
        virtual eElement WhichElement() const { return EBATTLEMENU; }

        std::list<BattleMenuOption*>::const_iterator GetOptionsBegin() const;
        std::list<BattleMenuOption*>::const_iterator GetOptionsEnd() const;
    private:
        virtual bool handle_element(eElement, Element *);
        virtual void load_attributes(CL_DomNamedNodeMap *);

        std::list<BattleMenuOption*> m_options;
    };
};

#endif

