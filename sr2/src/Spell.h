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
    class StatusEffect;




    class DoWeaponDamage : public Element, public Effect
	{
	public:
	    DoWeaponDamage();
	    virtual ~DoWeaponDamage();
		virtual eElement whichElement() const{ return EDOWEAPONDAMAGE; }
	    WeaponDamageCategory * getDamageCategory();

	    uint getBaseAttack() const;
	    float getBaseCritical() const;
	    float getBaseHit() const;
	    bool isRanged() const;
     
	    virtual eType getEffectType() const { return WEAPON_DAMAGE; }

	    CL_DomElement createDomElement( CL_DomDocument &doc ) const;

	private:
	    virtual void handleElement(eElement, Element * );
	    virtual void loadAttributes(CL_DomNamedNodeMap *);
		virtual void loadFinished();
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
	    virtual ~DoMagicDamage();
		virtual eElement whichElement() const{ return EDOMAGICDAMAGE; }

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
	    virtual void handleElement(eElement, Element * );
	    virtual void loadAttributes(CL_DomNamedNodeMap *);
		virtual void loadFinished();
	    MagicDamageCategory * mpDamageCategory;
	    uint mnBaseDamage;
	    float mfBaseHit;
	    bool mbDrain;
	    bool mbPiercing;
	    eDamageAttr meDamageAttr;
	};

	class DoAttack : public Element, public Effect
	{
	public:
		DoAttack();
		~DoAttack();
		virtual eElement whichElement() const{ return EDOATTACK; }
		uint getNumberOfHits() const;
		
		float getCritMultiplier() const { return mfMultiplyCritical; }
		float getCritAdd() const{ return mfAddCritical; }
		float getAttackMultiplier() const { return mfMultiplyAttack; }
		int getAttackAdd() const { return mnAddAttack; }
		float getHitsMultiplier() const { return mfHitsMultiplier; }
		int getHitsAdd() const { return mnHitsAdd; }
		bool hitAllenemies() const;
	    virtual eType getEffectType() const { return ATTACK; }
	private:
	    virtual void loadAttributes(CL_DomNamedNodeMap *);
		bool mbHitAllEnemies;
		float mfMultiplyAttack;
		int mnAddAttack;
		float mfMultiplyCritical;
		float mfAddCritical;
		float mfHitsMultiplier;
		int mnHitsAdd;
		uint mnHits;
	};


    class DoStatusEffect : public Element, public Effect
	{
	public:
	    DoStatusEffect();
	    virtual ~DoStatusEffect();
		virtual eElement whichElement() const{ return EDOSTATUSEFFECT; }
	    std::string getStatusRef() const;
	    float getChance() const;

	    bool removeStatus() const;

	    CL_DomElement createDomElement( CL_DomDocument &doc ) const;

	    virtual eType getEffectType() const { return STATUS_EFFECT; }
	private:
	    virtual void loadAttributes(CL_DomNamedNodeMap *);
	    std::string mStatusRef;
	    float mfChance;
	    bool mbRemove;
	};
    

    class MagicResistance : public Element
	{
	public:
	    MagicResistance();
	    virtual ~MagicResistance();
		virtual eElement whichElement() const{ return EMAGICRESISTANCE; }
	    float getResistance() const;
	    eMagicType getType() const;
		bool resistAll() const;
		bool resistElemental() const;
	    CL_DomElement  createDomElement(CL_DomDocument &doc) const { return CL_DomElement(doc,"magicResistance"); }

	private:
	    virtual void loadAttributes(CL_DomNamedNodeMap *);
	    eMagicType meType;
		bool mbResistAll;
		bool mbResistElemental;
	    float mfResistance;
	};


    class Spell : public Element
	{
	public:
	    Spell();
	    virtual ~Spell();

		virtual eElement whichElement() const{ return ESPELL; }
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

	    uint getValue() const;

	    std::list<Effect*>::const_iterator getEffectsBegin() const;
	    std::list<Effect*>::const_iterator getEffectsEnd() const;

	    CL_DomElement createDomElement( CL_DomDocument &doc ) const;

	    MagicResistance * getMagicResistance() const;

	
	private:
		virtual void handleElement(eElement, Element * );
	    virtual void loadAttributes(CL_DomNamedNodeMap *);

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
	    uint mnValue;
	    MagicResistance * mpMagicResistance;
	    std::list<Effect*> mEffects;
	};




/*
  class WeaponDamageCategory : public Element
  {
  public:
  WeaponDamageCategory();
  WeaponDamageCategory();
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
