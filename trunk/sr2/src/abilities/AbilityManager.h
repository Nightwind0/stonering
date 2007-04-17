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
        ~AbilityManager(){}


        void loadSpellFile ( CL_DomDocument &doc );
        void loadStatusEffectFile ( CL_DomDocument &doc );
        void loadSkillFile(CL_DomDocument &doc );
        void loadCharacterClassFile (CL_DomDocument  &doc);
    
        std::list<Spell*>::const_iterator getSpellsBegin() const;
        std::list<Spell*>::const_iterator  getSpellsEnd() const;

        std::map<std::string,Skill*>::const_iterator getSkillsBegin() const;
        std::map<std::string,Skill*>::const_iterator getSkillsEnd() const;

        std::list<StatusEffect*>::const_iterator getStatusEffectsBegin() const;
        std::list<StatusEffect*>::const_iterator getStatusEffectsEnd() const;

        virtual Spell * getSpell( const SpellRef & ref ) const;
        virtual StatusEffect * getStatusEffect ( const std::string &ref ) const;
        virtual Skill * getSkill ( const SkillRef &ref ) const;
        virtual CharacterClass *getClass (const std::string &className ) const;

#ifndef NDEBUG
        void dumpSpellList();
#endif
    

    private:
        std::map<std::string,Skill*> mSkills;
        std::list<Spell*> mSpells;
        std::list<StatusEffect*> mStatusEffects;
        std::map<std::string,CharacterClass*> mCharacterClasses;

    };


};

// Blah blah what kind of endings?


#endif




