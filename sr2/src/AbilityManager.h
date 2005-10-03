#ifndef SR_ABILITY_MANAGER
#define SR_ABILITY_MANAGER

#include "Spell.h"
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
    
    std::list<Spell*>::const_iterator getSpellsBegin() const;
    std::list<Spell*>::const_iterator  getSpellsEnd() const;

	std::list<StatusEffect*>::const_iterator getStatusEffectsBegin() const;
	std::list<StatusEffect*>::const_iterator getStatusEffectsEnd() const;

    virtual Spell * getSpell( const SpellRef & ref ) const;
	virtual StatusEffect * getStatusEffect ( const std::string &ref ) const;

#ifndef NDEBUG
    void dumpSpellList();
#endif
    

private:
    std::list<Spell*> mSpells;
	std::list<StatusEffect*> mStatusEffects;

};


};
#endif
