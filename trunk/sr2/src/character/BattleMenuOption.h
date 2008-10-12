#ifndef SR_BATTLE_MENU_OPTION_H
#define SR_BATTLE_MENU_OPTION_H

#include "Element.h"
#include "ScriptElement.h"
#include "Skill.h"
#include "IBattleAction.h"


namespace StoneRing
{
    class SteelInterpreter;
    class ICharacterGroup;
    class ICharacter;
    // This is the object that the battle system uses to queue up actions
    class ActionQueue;
    class BattleMenuOption;
    class BattleMenu;

    class BattleMenuOption : public Element
    {
    public:
        BattleMenuOption(int level=0);
        virtual ~BattleMenuOption();
        virtual eElement WhichElement() const { return EBATTLEMENUOPTION; }

        std::string GetName() const;
        bool Enabled() const;
    private:
        virtual bool handle_element(eElement, Element *);
        virtual void load_attributes(CL_DomNamedNodeMap *);
        virtual void load_finished();
    protected:
        enum ActionType
        {
            INVALID,
            SKILLREF,
            SCRIPT,
            SUBMENU
        };
        std::string m_name;
        int m_nLevel;
        ScriptElement *m_pConditionScript;
        union Action{ 
            ScriptElement *m_pScript;
            SkillRef *m_pSkillRef;
            BattleMenu * m_pSubMenu;
        };
        Action m_action;
        ActionType m_action_type;
    };
};


#endif
