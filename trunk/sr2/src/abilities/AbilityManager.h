#ifndef SR_ABILITY_MANAGER
#define SR_ABILITY_MANAGER

#include "Spell.h"
#include "Skill.h"
#include "CharacterClass.h"
#include <ClanLib/core.h>
#include "Item.h"


namespace StoneRing
{

    class AbilityManager
    {
    public:
        AbilityManager(){}
        ~AbilityManager();

        void LoadSpellFile ( CL_DomDocument &doc );
        void LoadStatusEffectFile ( CL_DomDocument &doc );
        void LoadSkillFile(CL_DomDocument &doc );
    
        std::list<Spell*>::const_iterator GetSpellsBegin() const;
        std::list<Spell*>::const_iterator  GetSpellsEnd() const;

        std::map<std::string,Skill*>::const_iterator GetSkillsBegin() const;
        std::map<std::string,Skill*>::const_iterator GetSkillsEnd() const;

        std::list<StatusEffect*>::const_iterator GetStatusEffectsBegin() const;
        std::list<StatusEffect*>::const_iterator GetStatusEffectsEnd() const;

        virtual Spell * GetSpell( const SpellRef & ref ) const;
        virtual StatusEffect * GetStatusEffect ( const std::string &ref ) const;
        virtual Skill * GetSkill ( const SkillRef &ref ) const;

#ifndef NDEBUG
        void DumpSpellList();
#endif
    

    private:
        typedef std::map<std::string,Skill*> SkillMap;
        typedef std::list<Spell*> SpellList;
        typedef std::list<StatusEffect*> StatusEffectList;

        SkillMap m_skills;
        SpellList m_spells;
        StatusEffectList m_status_effects;
    };


};


#endif




