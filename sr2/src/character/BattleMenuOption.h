#ifndef SR_BATTLE_MENU_OPTION_H
#define SR_BATTLE_MENU_OPTION_H

#include "Element.h"
#include "ScriptElement.h"
#include "Skill.h"


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
        virtual eElement whichElement() const { return EBATTLEMENUOPTION; }

        std::string getName() const;
        bool enabled() const;

    private:
        virtual bool handleElement(eElement, Element *);
        virtual void loadAttributes(CL_DomNamedNodeMap *);
        virtual void loadFinished();
    protected:
        enum ActionType
        {
            INVALID,
            SKILLREF,
            SCRIPT,
            SUBMENU
        };
        std::string mName;
        int mnLevel;
        ScriptElement *mpConditionScript;
        union Action{ 
            ScriptElement *mpScript;
            SkillRef *mpSkillRef;
            BattleMenu * mpSubMenu;
        };
        Action mAction;
        ActionType mActionType;
    };
};


#endif

