#include "Spell.h"
#include "AbilityFactory.h"
#include "IApplication.h"
#include "StatusEffect.h"
#include "sr_defines.h"
#include "Animation.h"

using namespace StoneRing;

Effect::Effect()
{
}

Effect::~Effect()
{
}

void DoWeaponDamage::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
	
    mnBaseAttack = getRequiredUint("baseAttack",pAttributes);
    mfBaseCritical = getRequiredFloat("baseCritical",pAttributes);
    mfBaseHit = getRequiredFloat("baseHit",pAttributes);
    mbRanged = getImpliedBool("ranged",pAttributes,false);

}

void DoWeaponDamage::handleElement(eElement element,Element *pElement)
{
		if(element == EWEAPONDAMAGECATEGORY)
		{
			mpDamageCategory = dynamic_cast<WeaponDamageCategory*>(pElement);
		}
}

void DoWeaponDamage::loadFinished()
{
	if(mpDamageCategory == NULL)
		throw CL_Error("No weapon damage category was defined for this doWeaponDamage");
}

DoWeaponDamage::DoWeaponDamage():mpDamageCategory(NULL)
{

    
    
}

DoWeaponDamage::~DoWeaponDamage()
{
}


WeaponDamageCategory * DoWeaponDamage::getDamageCategory()
{
    return mpDamageCategory;
}


uint DoWeaponDamage::getBaseAttack() const
{
    return mnBaseAttack;
}

float DoWeaponDamage::getBaseCritical() const
{
    return mfBaseCritical;
}

float DoWeaponDamage::getBaseHit() const
{
    return mfBaseHit;
}

bool DoWeaponDamage::isRanged() const
{
    return mbRanged;
}



CL_DomElement DoWeaponDamage::createDomElement( CL_DomDocument &doc ) const
{
    return CL_DomElement(doc,"doWeaponDamage");
}




DoMagicDamage::DoMagicDamage()
{
}


void DoMagicDamage::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
	
    mnBaseDamage = getRequiredUint("baseDamage",pAttributes);
    mfBaseHit = getRequiredFloat("baseHit",pAttributes);
    mbDrain = getImpliedBool("drain",pAttributes,false);
	mbPiercing = getImpliedBool("piercing",pAttributes,false);	

}

void DoMagicDamage::handleElement(eElement element,Element *pElement)
{
		if(element == EMAGICDAMAGECATEGORY)
		{
			mpDamageCategory = dynamic_cast<MagicDamageCategory*>(pElement);
		}
}

void DoMagicDamage::loadFinished()
{
	if(mpDamageCategory == NULL)
		throw CL_Error("No magic damage category was defined for this doMagicDamage");
}

DoMagicDamage::~DoMagicDamage()
{
}


uint DoMagicDamage::getBaseDamage() const
{
    return mnBaseDamage;
}

float DoMagicDamage::getBaseHit() const
{
    return mfBaseHit;
}

bool DoMagicDamage::drain() const
{
    return mbDrain;
}

bool DoMagicDamage::isPiercing() const
{
    return mbPiercing;
}

MagicDamageCategory * DoMagicDamage::getMagicCategory()
{
    return mpDamageCategory;
}



DoMagicDamage::eDamageAttr DoMagicDamage::getDamageAttr() const
{
    return meDamageAttr;
}

CL_DomElement DoMagicDamage::createDomElement( CL_DomDocument &doc ) const
{
    return CL_DomElement(doc,"doMagicDamage" );
}




DoAttack::DoAttack()
{
}

DoAttack::~DoAttack()
{
}


uint DoAttack::getNumberOfHits() const
{
	return mnHits;
}


bool DoAttack::hitAllenemies() const
{
	return mbHitAllEnemies;
}

void DoAttack::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
	mbHitAllEnemies = getImpliedBool("allEnemies",pAttributes,false);
	mfMultiplyCritical = getImpliedFloat("multiplyCritical",pAttributes,1.0);
	mnHits = getImpliedInt("hits",pAttributes,1);
	mfAddCritical = getImpliedFloat("addCritical",pAttributes,0);
	mnAddAttack = getImpliedInt("addAttack",pAttributes,0);
	mfMultiplyAttack = getImpliedFloat("multiplyAttack",pAttributes,1);
	mfHitsMultiplier = getImpliedFloat("multiplyHits",pAttributes,1);
	mnHitsAdd = getImpliedInt("addHits",pAttributes,0);
}


DoStatusEffect::DoStatusEffect()
{
}

void DoStatusEffect::handleElement(eElement element, Element * pElement)
{
	if(element == ESTATUSEFFECT)
	{
		mpStatusEffect = dynamic_cast<StatusEffect*>(pElement);
	}
}

void DoStatusEffect::loadFinished()
{
	if(mpStatusEffect == NULL)
		throw CL_Error("Error: DoStatusEffect must either define a status effect or include a statusRef");
}

void DoStatusEffect::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
	const AbilityManager * pManager = IApplication::getInstance()->getAbilityManager();

	if(hasAttr("statusRef",pAttributes))
	{
		std::string statusRef = getString("statusRef",pAttributes);
		mpStatusEffect = pManager->getStatusEffect(statusRef);
	}

    mfChance = getRequiredFloat("chance",pAttributes);
    mbRemove = getImpliedBool("removeStatus",pAttributes, false);
}

DoStatusEffect::~DoStatusEffect()
{
}


StatusEffect* DoStatusEffect::getStatusEffect() const
{
    return mpStatusEffect;
}

float DoStatusEffect::getChance() const
{
    return mfChance;
}

bool DoStatusEffect::removeStatus() const
{
    return mbRemove;
}

CL_DomElement DoStatusEffect::createDomElement( CL_DomDocument &doc ) const
{
    return CL_DomElement(doc,"doStatusEffect");
}


Spell::Spell()
{
}

void Spell::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
	mName = getRequiredString("name", pAttributes);
    meType = getTypeFromString(getRequiredString("type", pAttributes));
    meUse = getUseFromString(getRequiredString("use", pAttributes));
    meTargetable = getTargetableFromString(getRequiredString("targetable", pAttributes));

    mbAppliesToWeapons = getImpliedBool ("appliesToWeapons",pAttributes, false);
    mbAppliesToArmor = getImpliedBool ("appliesToArmor",pAttributes, false);
    
    mnValue = getRequiredUint("value",pAttributes);

    mnMP = getRequiredUint("mp", pAttributes);
}

void Spell::handleElement(eElement element, Element * pElement)
{
	switch(element)
	{
	case EDOWEAPONDAMAGE:
	case EDOMAGICDAMAGE:
	case EDOSTATUSEFFECT:
	case EANIMATION:
		mEffects.push_back ( dynamic_cast<Effect*>(pElement) );
		break;
	case EMAGICRESISTANCE:
		mpMagicResistance = dynamic_cast<MagicResistance*>(pElement);
		break;
	default:
		throw CL_Error("Bad element in spell.");
	}
}



Spell::eType Spell::getTypeFromString(const std::string &str)
{
    if(str == "elemental") return ELEMENTAL;
    else if (str == "white") return WHITE;
    else if (str == "status") return STATUS;
    else if (str == "other") return OTHER;
    else throw CL_Error("Bad spell type.");
}

Spell::eUse Spell::getUseFromString ( const std::string &str)
{
    if(str == "battle") return BATTLE;
    else if (str == "world") return WORLD;
    else if (str == "both") return BOTH;
    else throw CL_Error("Bad spell use");
}

Spell::eTargetable Spell::getTargetableFromString ( const std::string &str)
{
    if(str == "all") return ALL;
    else if (str == "single") return SINGLE;
    else if (str == "either") return EITHER;
    else if (str == "self-only") return SELF_ONLY;
    else throw CL_Error("Bad spell targetable.");
}


Spell::~Spell()
{
	std::for_each(mEffects.begin(),mEffects.end(),del_fun<Effect>());
	delete mpMagicResistance;
}


MagicResistance * Spell::getMagicResistance() const
{
    return mpMagicResistance;
}

std::string Spell::getName() const
{
    return mName;
}

uint Spell::getValue() const
{
    return mnValue;
}


Spell::eType Spell::getType() const
{
    return meType;
}

Spell::eUse Spell::getUse() const
{
    return meUse;
}

Spell::eTargetable Spell::getTargetable() const
{
    return meTargetable;
}



bool Spell::appliesToWeapons() const
{
    return mbAppliesToWeapons;
}

bool Spell::appliesToArmor() const
{
    return mbAppliesToArmor;
}

	
uint Spell::getMP() const
{
    return mnMP;
}


std::list<Effect*>::const_iterator Spell::getEffectsBegin() const
{
    return mEffects.begin();
}

std::list<Effect*>::const_iterator Spell::getEffectsEnd() const
{
    return mEffects.end();
}

CL_DomElement Spell::createDomElement( CL_DomDocument &doc ) const
{
    return CL_DomElement(doc,"spell");
}

SpellRef * Spell::createSpellRef() const
{
    SpellRef * ref = new SpellRef;
    SpellRef::eSpellType type;
    switch(meType)
    {
    case ELEMENTAL:
	type = SpellRef::ELEMENTAL;
	break;
    case WHITE:
	type = SpellRef::WHITE;
	break;
    case STATUS:
	type = SpellRef::STATUS;
	break;
    case OTHER:
	type = SpellRef::OTHER;
	break;
    }
    ref->setType( type );
    ref->setName ( mName );

    return ref;
}





MagicResistance::MagicResistance():mbResistAll(false),mbResistElemental(false)
{
}

void MagicResistance::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
	std::string type = getRequiredString("type",pAttributes);
    
    if(type == "fire") meType = FIRE;
    else if (type == "water") meType = WATER;
    else if (type == "earth") meType = EARTH;
    else if (type == "wind") meType = WIND;
    else if (type == "holy") meType = HOLY;
	else if (type == "dark") meType = DARK;
    else if (type == "elemental") mbResistElemental = true;
    else if (type == "all") mbResistAll = true;
    else throw CL_Error("Bad magic resistance type of " + type);

    mfResistance = getRequiredFloat("resist", pAttributes);

}


MagicResistance::~MagicResistance()
{
}



float MagicResistance::getResistance() const
{
    return mfResistance;
}



eMagicType MagicResistance::getType() const
{
    return meType;
}



/*

WeaponDamageCategory::WeaponDamageCategory()
{
}

WeaponDamageCategory::WeaponDamageCategory(CL_DomElement * pElement)
{
    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    std::string type = getRequiredString("type", pAttributes);

    if(type == "slash") meType = SLASH;
    else if (type == "bash") meType = BASH;
    else if (type == "jab") meType = JAB;
    else throw CL_Error("Bad type: " + type + " on weapon damage category.");

}

WeaponDamageCategory::~WeaponDamageCategory()
{
}

	

WeaponDamageCategory::eType WeaponDamageCategory::getType() const
{
    return meType;
}


CL_DomElement WeaponDamageCategory::createDomElement( CL_DomDocument &doc ) const
{
    return CL_DomElement(doc,"weaponDamageCategory" );
}



MagicDamageCategory::MagicDamageCategory()
{
}

MagicDamageCategory::MagicDamageCategory(CL_DomElement * pElement)
{

    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    std::string type = getRequiredString("type", pAttributes);

    if(type == "eart") meType = EARTH;
    else if (type == "wind") meType = WIND;
    else if (type == "fire") meType = FIRE;
    else if (type == "water") meType = WATER;
    else if (type == "holy") meType = HOLY;
    else if (type == "other") meType = OTHER;

    else throw CL_Error("Bad type: " + type + " on weapon damage category.");

}

MagicDamageCategory::~MagicDamageCategory()
{
}

	

MagicDamageCategory::eType MagicDamageCategory::getType() const
{
    return meType;
}


CL_DomElement MagicDamageCategory::createDomElement( CL_DomDocument &doc ) const
{
    return CL_DomElement(doc,"magicDamageCategory" );
}
	


*/
