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

        virtual eElement WhichElement() const{ return ESKILL; }

        std::string GetName() const;
        uint GetSPCost() const;
        uint GetBPCost() const;
        uint GetMinLevel() const;

        std::list<SkillRef*>::const_iterator GetPreReqsBegin() const;
        std::list<SkillRef*>::const_iterator GetPreReqsEnd() const;

        // This is called when you actually select the option.
        // Most options will then let you select a character/party as a target
        void Select(const ParameterList& params);
        // If you cancel an option, it should be able to clean itself up
        // (especially removing entries from the queue)
        void Deselect();

        // For invoking from the  menu
        void Invoke();

        eType GetType() const { return m_eType; }
    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes);
        static eType TypeFromString(const std::string type);
        std::string m_name;
        std::list<SkillRef*> m_pre_reqs;
        NamedScript *m_pOnInvoke;
        NamedScript *m_pOnRemove;
        NamedScript *m_pCondition;
        NamedScript *m_pOnSelect;
        NamedScript *m_pOnDeselect;
        uint m_nSp;
        uint m_nBp;
        uint m_nMinLevel;
        eType m_eType;
        bool m_bAllowsGroupTarget;
        bool m_bDefaultToEnemyGroup;
    };

    class SkillRef: public Element
    {
    public:
        SkillRef();
        virtual ~SkillRef();
        virtual eElement WhichElement() const{ return ESKILLREF; }
        std::string GetRef() const;
        uint GetSPCost() const;
        uint GetBPCost() const;
        uint GetMinLevel() const;
        Skill * GetSkill() const;

    private:
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes);
        uint m_nSp;
        uint m_nBp;
        uint m_nMinLevel;
        std::string m_ref;
    };


}

#endif




