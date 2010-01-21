#ifndef SR_BATTLE_MENU_OPTION_H
#define SR_BATTLE_MENU_OPTION_H

#include "Element.h"
#include "ScriptElement.h"
#include "Skill.h"

class SteelInterpreter;

namespace StoneRing
{

    class ICharacterGroup;
    class ICharacter;
    // This is the object that the battle system uses to queue up actions
    class ActionQueue;
    class BattleMenuOption;
    class BattleMenu;

    typedef std::stack<BattleMenu*> BattleMenuStack;

    class BattleMenuOption : public Element
    {
    public:
        BattleMenuOption(int level=0);
        virtual ~BattleMenuOption();
        virtual eElement WhichElement() const { return EBATTLEMENUOPTION; }

        std::string GetName() const;
        CL_Surface* GetIcon() const { return m_pIcon; }
        bool Enabled(const ParameterList &params) const;
        void Select(BattleMenuStack& stack, const ParameterList& params);
        void Deselect(BattleMenuStack& stack); // For backing out of a selection
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
        CL_Surface* m_pIcon;
        Action m_action;
        ActionType m_action_type;
    };
}


#endif
