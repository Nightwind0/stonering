#ifndef SR_SPELL_H
#define SR_SPELL_H

#include "Element.h"
#include "Effect.h"
#include <ClanLib/core.h>

namespace StoneRing
{

    class WeaponDamageCategory;
    class MagicDamageCategory;
    class SpellRef;


    class DoWeaponDamage : public Element, public Effect
    {
    public:
	DoWeaponDamage();
	DoWeaponDamage(CL_DomElement *pElement);
	virtual ~DoWeaponDamage();

	WeaponDamageCategory * getDamageCategory();

	uint getBaseAttack() const;
	float getBaseCritical() const;
	float getBaseHit() const;
	bool isRanged() const;
     
	virtual eType getEffectType() const { return WEAPON_DAMAGE; }

	CL_DomElement createDomElement( CL_DomDocument &doc ) const;

    private:
	WeaponDamageCategory * mpDamageCategory;
	uint mnBaseAttack;
	float mfBaseCritical;
	float mfBaseHit;
	bool mbRanged;
    };

    class DoMagicDamage : public Element, public Effect
    {
    public:
	DoMagicDamage();
	DoMagicDamage(CL_DomElement *pElement);
	virtual ~DoMagicDamage();


	uint getBaseDamage() const;
	float getBaseHit() const;
	bool drain() const;
	bool isPiercing() const;
	
	MagicDamageCategory * getMagicCategory();

	virtual eType getEffectType() const { return MAGIC_DAMAGE; }

	enum eDamageAttr { HP, MP };

	eDamageAttr getDamageAttr() const;

	CL_DomElement createDomElement( CL_DomDocument &doc ) const;
    private:
	MagicDamageCategory * mpDamageCategory;
	uint mnBaseDamage;
	float mfBaseHit;
	bool mbDrain;
	bool mbPiercing;
	eDamageAttr meDamageAttr;
    };


    class DoStatusEffect : public Element, public Effect
    {
    public:
	DoStatusEffect();
	DoStatusEffect(CL_DomElement *pElement);
	virtual ~DoStatusEffect();

	std::string getStatusRef() const;
	float getChance() const;

	bool removeStatus() const;

	CL_DomElement createDomElement( CL_DomDocument &doc ) const;

	virtual eType getEffectType() const { return STATUS_EFFECT; }
    private:
	std::string mStatusRef;
	float mfChance;
	bool mbRemove;
    };

    class Spell : public Element
    {
    public:
	Spell();
	Spell(CL_DomElement *pElement);
	virtual ~Spell();


	std::string getName() const;

	enum eType { ELEMENTAL, WHITE, STATUS, OTHER };
	enum eUse { BATTLE, WORLD, BOTH };
	enum eTargetable { ALL, SINGLE, EITHER, SELF_ONLY };
	eType getType() const;
	eUse getUse() const;
	eTargetable getTargetable() const;


	bool appliesToWeapons() const;
	bool appliesToArmor() const;
	
	uint getMP() const;

	SpellRef * createSpellRef() const;

	std::list<Effect*>::const_iterator getEffectsBegin() const;
	std::list<Effect*>::const_iterator getEffectsEnd() const;

	CL_DomElement createDomElement( CL_DomDocument &doc ) const;

	
    private:

	eType getTypeFromString(const std::string &str);
	eUse getUseFromString ( const std::string &str);
	eTargetable getTargetableFromString ( const std::string &str);

	eType meType;
	eUse meUse;
	eTargetable meTargetable;
	std::string mName;
	bool mbAppliesToWeapons;
	bool mbAppliesToArmor;
	uint mnMP;

	std::list<Effect*> mEffects;
    };

/*
    class WeaponDamageCategory : public Element
    {
    public:
	WeaponDamageCategory();
	WeaponDamageCategory(CL_DomElement * pElement);
	virtual ~WeaponDamageCategory();
	
	enum eType { SLASH, BASH, JAB };
	eType getType() const;

	CL_DomElement createDomElement( CL_DomDocument &doc ) const;
	
    private:
	eType meType;
    };

    class MagicDamageCategory : public Element
    {
    public:
	MagicDamageCategory();
	MagicDamageCategory(CL_DomElement * pElement);
	virtual ~MagicDamageCategory();
	
	enum eType { FIRE, WATER, EARTH, WIND, HOLY, OTHER };
	eType getType() const;

	CL_DomElement createDomElement( CL_DomDocument &doc ) const;
    private:
	eType meType;
    };


*/

};


#endif
