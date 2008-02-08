#ifndef SR_SKILL_H
#define SR_SKILL_H
 
#include "Element.h"
#include <ClanLib/core.h>
#include "ScriptElement.h"
#include "Character.h"
#ifdef _WINDOWS_
#include "SteelType.h"
#include "SteelInterpreter.h"
#else
#include <steel/SteelType.h>
#include <steel/SteelInterpreter.h>
#endif

namespace StoneRing
{
    class AttributeModifier;

    class StatIncrease;
    class StartingStat;
    class ArmorTypeRef;
    class WeaponTypeRef;
    class SkillRef;
    class SpellRef;
    class ActionQueue;
    class NamedScript;



    class Skill : public Element
    {
    public:
        Skill();
        virtual ~Skill();

        enum eType { BATTLE, SWITCH };

        virtual eElement whichElement() const{ return ESKILL; }

        std::string getName() const;
        uint getSPCost() const;
        uint getBPCost() const;
        uint getMinLevel() const;
        
        std::list<SkillRef*>::const_iterator getPreReqsBegin() const;
        std::list<SkillRef*>::const_iterator getPreReqsEnd() const;

        // This is called when you actually select the option.
        // Most options will then let you select a character/party as a target
        // and then queue that selection up. 
        void select(ActionQueue *pQueue);
        // If you cancel an option, it should be able to clean itself up
        // (especially removing entries from the queue)
        void deselect(ActionQueue *pQueue);

        // For invoking from the  menu
        void invoke();

        eType getType() const { return meType; }
    private:
        virtual bool handleElement(eElement element, Element * pElement );
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        static eType typeFromString(const std::string type);
        std::string mName;
        std::list<SkillRef*> mPreReqs;
        NamedScript *mpOnInvoke;
        NamedScript *mpOnRemove;
        NamedScript *mpCondition;
        NamedScript *mpOnSelect;
        NamedScript *mpOnDeselect;

        uint mnSp;
        uint mnBp;
        uint mnMinLevel;
        eType meType;
        bool mbAllowsGroupTarget;
        bool mbDefaultToEnemyGroup;
    };

    class SkillRef: public Element
    {
    public:
        SkillRef();
        virtual ~SkillRef();
        virtual eElement whichElement() const{ return ESKILLREF; }
        std::string getRef() const;
        uint getSPCost() const;
        uint getBPCost() const;
        uint getMinLevel() const;

    private:
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        uint mnSp;
        uint mnBp;
        uint mnMinLevel;
        std::string mRef;
    };

    
}

#endif




