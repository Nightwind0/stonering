#ifndef SR_ABILITY_MANAGER
#define SR_ABILITY_MANAGER

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
		
		static void initialize();

        static void LoadStatusEffectFile ( CL_DomDocument &doc );
        static void LoadSkillFile(CL_DomDocument &doc);
        static void LoadAnimationFile(CL_DomDocument &doc);

        static std::map<std::string,Skill*>::const_iterator GetSkillsBegin();
        static std::map<std::string,Skill*>::const_iterator GetSkillsEnd();

        static std::list<StatusEffect*>::const_iterator GetStatusEffectsBegin();
        static std::list<StatusEffect*>::const_iterator GetStatusEffectsEnd();

        static StatusEffect * GetStatusEffect ( const std::string &ref );
        static Skill * GetSkill ( const SkillRef &ref );

        static Skill * GetSkill ( const std::string &skill );

        static bool SkillExists ( const std::string &skill );

        static Animation* GetAnimation ( const std::string &animation );

    private:
        typedef std::map<std::string,Skill*> SkillMap;
        typedef std::map<std::string,StatusEffect*> StatusEffectMap;
        typedef std::map<std::string,Animation*> AnimationMap;

        SkillMap m_skills;
        StatusEffectMap m_status_effects;
        AnimationMap m_animations;
		static AbilityManager* m_pInstance;
    };


}


#endif




