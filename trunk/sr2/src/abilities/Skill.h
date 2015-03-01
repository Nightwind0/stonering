#ifndef SR_SKILL_H
#define SR_SKILL_H

#include "sr_defines.h"
#include "Element.h"
#include <ClanLib/core.h>
#include "ScriptElement.h"
#include "Character.h"
#include "ScriptElement.h"



namespace StoneRing
{
    class AttributeModifier;

    class StatIncrease;
    class StartingStat;
    class ArmorTypeRef;
    class WeaponTypeRef;
    class SkillRef;
    class ActionQueue;
    class NamedScript;


    class Skill : public Element, public SteelType::IHandle
    {
    public:
        Skill();
        virtual ~Skill();

        enum eType { BATTLE, WORLD, BOTH };

        virtual eElement WhichElement() const{ return ESKILL; }

        std::string GetName() const;
        
        clan::Image GetIcon() const{ return m_pIcon; }
        
        uint GetBPCost() const;
        uint GetMPCost() const;

        // This is called when you actually select the option.
        // Most options will then let you select a character/party as a target
	// Returns whether the skill went through (i.e. was not cancelled)
        bool Invoke(ICharacter*pCharacter, const ParameterList& params);
        
        std::string GetDescription() const { return m_description; }

        eType GetType() const { return m_eType; }
		virtual std::string GetDebugId() const { return m_name; }		        
    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(clan::DomNamedNodeMap attributes);
        static eType TypeFromString(const std::string type);
        std::string m_name;
        std::string m_description;

        NamedScript* m_pOnInvoke;
        NamedScript* m_pOnRemove;
        NamedScript* m_pCondition;

        uint m_nBp;
        uint m_nMp;
        clan::Image m_pIcon;

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
        Skill * GetSkill() const;
		virtual std::string GetDebugId() const { return m_ref; }				
    private:
        virtual void load_attributes(clan::DomNamedNodeMap attributes);
        
        std::string m_ref;
    };


    class SkillTreeNode: public Element
    {
    public:
        SkillTreeNode();
        virtual ~SkillTreeNode();
        virtual eElement WhichElement() const{ return ESKILLTREENODE; }
        uint GetSPCost() const;
        uint GetMinLevel() const;
        bool CanLearn(Character* pCharacter);
        std::string GetRequirements() const { return m_requirements; }
        std::string GetRef() const { return m_ref; }
        SkillTreeNode* GetParent() const;
        std::list<SkillTreeNode*>::const_iterator GetSubSkillsBegin() const;
        std::list<SkillTreeNode*>::const_iterator GetSubSkillsEnd() const;
		virtual std::string GetDebugId() const { return m_ref; }				
    private:
        virtual bool handle_element(eElement element, Element * pElement);
        virtual void load_attributes(clan::DomNamedNodeMap attributes);
        ScriptElement* m_pCondition;
		ScriptElement* m_pVisibilityCondition;
        std::list<SkillTreeNode*> m_sub_skills;
        std::string m_ref;
        std::string m_requirements;
        uint m_nSp;
        uint m_nMinLevel;        
        SkillTreeNode* m_parent;
    };
    
  

    
    bool operator==(const SkillRef& lhs, const SkillRef& rhs);

}

#endif




