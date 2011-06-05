#ifndef SR_CHARACTER_CLASS
#define SR_CHARACTER_CLASS

#include "Element.h"
#include <ClanLib/core.h>
#include "Character.h"

namespace StoneRing
{

    class WeaponTypeRef;
    class ArmorTypeRef;
    class StatScript;
    class SkillRef;
    class BattleMenu;
    class ScriptElement;
    class SkillTreeNode;

    class CharacterClass : public Element, public SteelType::IHandle
    {
    public:
        CharacterClass();
        virtual ~CharacterClass();
        virtual eElement WhichElement() const{ return ECHARACTERCLASS; }

        std::list<WeaponTypeRef*>::const_iterator GetWeaponTypeRefsBegin() const;
        std::list<WeaponTypeRef*>::const_iterator GetWeaponTypeRefsEnd() const;

        std::list<ArmorTypeRef*>::const_iterator GetArmorTypeRefsBegin() const;
        std::list<ArmorTypeRef*>::const_iterator GetArmorTypeRefsEnd() const;

        double    GetStat(ICharacter::eCharacterAttribute attr, int level);
	int	  GetExperienceToLevel(int level);
	int       GetLevelForExperience(int xp);

        std::list<SkillTreeNode*>::const_iterator GetSkillTreeNodesBegin() const;
        std::list<SkillTreeNode*>::const_iterator GetSkillTreeNodesEnd() const;

        BattleMenu * GetBattleMenu() const;
        std::string GetName() const;
        ICharacter::eGender GetGender() const;
    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap attributes);
        virtual void load_finished();
        //void verify_menu_options(BattleMenu* pMenu);
        std::string m_name;
        ICharacter::eGender m_eGender;
        std::list<WeaponTypeRef*> m_weapon_types;
        std::list<ArmorTypeRef*> m_armor_types;
        typedef std::map<ICharacter::eCharacterAttribute,StatScript*> StatMap;
        StatMap m_stat_scripts;
	//AstScript* m_pTNL;
        std::list<SkillTreeNode*> m_skill_tree;
        BattleMenu *m_pMenu;
    };

    class StatScript : public Element
    {
    public:
        StatScript();
        ~StatScript();
        virtual eElement WhichElement() const{ return ESTATSCRIPT; }
        ICharacter::eCharacterAttribute GetCharacterStat() const;
        double GetStat(int level);
    private:
        virtual void load_attributes(CL_DomNamedNodeMap attributes);
        virtual bool handle_element(Element::eElement, Element * pElement);
        virtual void load_finished();
        ScriptElement *m_pScript;
        ICharacter::eCharacterAttribute m_eStat;
    };
}

#endif




