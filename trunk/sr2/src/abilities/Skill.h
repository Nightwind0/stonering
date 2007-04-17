#ifndef SR_SKILL_H
#define SR_SKILL_H
 
#include "Element.h"
#include <ClanLib/core.h>
#include "ScriptElement.h"
#include "Character.h"



namespace StoneRing
{
    class AttributeModifier;

    class StatIncrease;
    class StartingStat;
    class ArmorTypeRef;
    class WeaponTypeRef;
    class SkillRef;
    class SpellRef;

    class Skill : public Element
    {
    public:
        Skill();
        ~Skill();

        enum eType { BATTLE, SWITCH };

        virtual eElement whichElement() const{ return ESKILL; }

        std::string getName() const;
        uint getSPCost() const;
        uint getBPCost() const;
        uint getMinLevel() const;
        
        std::list<SkillRef*>::const_iterator getPreReqsBegin() const;
        std::list<SkillRef*>::const_iterator getPreReqsEnd() const;

        SpellRef * getSpellRef() const;

        eType getType() const { return meType; }
    private:
        virtual bool handleElement(eElement element, Element * pElement );
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        std::string mName;
        std::list<SkillRef*> mPreReqs;
        ScriptElement *mpOnInvoke;
        ScriptElement *mpOnRemove;
        ScriptElement *mpCondition;
        SpellRef * mpSpellRef;
        uint mnSp;
        uint mnBp;
        uint mnMinLevel;
        eType meType;
    };

    class SkillRef: public Element
    {
    public:
        SkillRef();
        ~SkillRef();
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




