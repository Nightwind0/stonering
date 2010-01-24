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

        enum eType{
            POPUP,
            SKILLS,
            SPELLS,
            ITEMS,
            CUSTOM // TODO: Way to populate this
        };

        std::list<BattleMenuOption*>::iterator GetOptionsBegin();
        std::list<BattleMenuOption*>::iterator GetOptionsEnd();

        std::list<BattleMenuOption*>::iterator GetSelectedOption();

        void SelectNext();
        void SelectPrevious();

        eType GetType ( void ) const;
    private:
        virtual bool handle_element(eElement, Element *);
        virtual void load_attributes(CL_DomNamedNodeMap );
        virtual void load_finished();

        std::list<BattleMenuOption*> m_options;
        std::list<BattleMenuOption*>::iterator m_current;
        eType m_eType;
    };
};

#endif

