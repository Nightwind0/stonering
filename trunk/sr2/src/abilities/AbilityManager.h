#ifndef SR_ABILITY_MANAGER
#define SR_ABILITY_MANAGER

#include "Spell.h"
#include "Skill.h"
#include "CharacterClass.h"
#include <ClanLib/core.h>
#include "Item.h"


namespace StoneRing
{

    class Animation;
    class AbilityManager
    {
    public:
        AbilityManager(){}
        ~AbilityManager();

        static void LoadSpellFile ( CL_DomDocument &doc );
        static void LoadStatusEffectFile ( CL_DomDocument &doc );
        static void LoadSkillFile(CL_DomDocument &doc);
        static void LoadAnimationFile(CL_DomDocument &doc);

        static std::list<Spell*>::const_iterator GetSpellsBegin();
        static std::list<Spell*>::const_iterator  GetSpellsEnd();

        static std::map<std::string,Skill*>::const_iterator GetSkillsBegin();
        static std::map<std::string,Skill*>::const_iterator GetSkillsEnd();

        static std::list<StatusEffect*>::const_iterator GetStatusEffectsBegin();
        static std::list<StatusEffect*>::const_iterator GetStatusEffectsEnd();

        static Spell * GetSpell( const SpellRef & ref );
        static StatusEffect * GetStatusEffect ( const std::string &ref );
        static Skill * GetSkill ( const SkillRef &ref );

        static Skill * GetSkill ( const std::string &skill );

        static bool SkillExists ( const std::string &skill );

		static Animation* GetAnimation ( const std::string &animation );

#ifndef NDEBUG
        static void DumpSpellList();
#endif


    private:
        typedef std::map<std::string,Skill*> SkillMap;
        typedef std::list<Spell*> SpellList;
        typedef std::list<StatusEffect*> StatusEffectList;
        typedef std::map<std::string,Animation*> AnimationMap;

        SkillMap m_skills;
        SpellList m_spells;
        StatusEffectList m_status_effects;
        AnimationMap m_animations;
    };


}


#endif




