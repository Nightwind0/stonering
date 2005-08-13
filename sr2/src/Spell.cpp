#include "Spell.h"
#include "AbilityFactory.h"
#include "IApplication.h"
#include "sr_defines.h"
#include "Animation.h"

using namespace StoneRing;

Effect::Effect()
{
}

Effect::~Effect()
{
}




DoWeaponDamage::DoWeaponDamage()
{
}

DoWeaponDamage::DoWeaponDamage(CL_DomElement *pElement):mpDamageCategory(NULL)
{
    CL_DomNamedNodeMap attributes = pElement->get_attributes();
    AbilityFactory * pFactory = IApplication::getInstance()->getAbilityFactory();

    mnBaseAttack = getRequiredUint("baseAttack",&attributes);
    mfBaseCritical = getRequiredFloat("baseCritical",&attributes);
    mfBaseHit = getRequiredFloat("baseHit",&attributes);
    

    mbRanged = getImpliedBool("ranged",&attributes,false);
    

    CL_DomElement child = pElement->get_first_child().to_element();


    while(!child.is_null())
    {
	std::string name = child.get_node_name();

	if(name == "weaponDamageCategory")
	{
	    mpDamageCategory = pFactory->createWeaponDamageCategory ( &child );
	}

	
	child = child.get_next_sibling().to_element();
    }

    if(mpDamageCategory == NULL)
	throw CL_Error("No weapon damage category was defined for this doWeaponDamage");
    
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

DoMagicDamage::DoMagicDamage(CL_DomElement *pElement)
{
    CL_DomNamedNodeMap attributes = pElement->get_attributes();
    AbilityFactory * pFactory = IApplication::getInstance()->getAbilityFactory();

    mnBaseDamage = getRequiredUint("baseDamage",&attributes);
    mfBaseHit = getRequiredFloat("baseHit",&attributes);
       
    mbDrain = getImpliedBool("drain",&attributes, false);
    mbPiercing = getImpliedBool("piercing",&attributes,false);



    CL_DomElement child = pElement->get_first_child().to_element();


    while(!child.is_null())
    {
	std::string name = child.get_node_name();

	if(name == "magicDamageCategory")
	{
	    mpDamageCategory = pFactory->createMagicDamageCategory ( &child );
	}

	
	child = child.get_next_sibling().to_element();
    }

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



DoStatusEffect::DoStatusEffect()
{
}

DoStatusEffect::DoStatusEffect(CL_DomElement *pElement)
{
    CL_DomNamedNodeMap attributes = pElement->get_attributes();


    mStatusRef = getRequiredString("statusRef", &attributes);
    mfChance = getRequiredFloat("chance",&attributes);
   
    
    mbRemove = getImpliedBool("removeStatus",&attributes, false);




}

DoStatusEffect::~DoStatusEffect()
{
}


std::string DoStatusEffect::getStatusRef() const
{
    return mStatusRef;
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

Spell::Spell(CL_DomElement *pElement)
{
    AbilityFactory * pFactory = IApplication::getInstance()->getAbilityFactory();
    

    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    mName = getRequiredString("name", &attributes);
    meType = getTypeFromString(getRequiredString("type", &attributes));
    meUse = getUseFromString(getRequiredString("use", &attributes));
    meTargetable = getTargetableFromString(getRequiredString("targetable", &attributes));

    mbAppliesToWeapons = getImpliedBool ("appliesToWeapons",&attributes, false);
    mbAppliesToArmor = getImpliedBool ("appliesToArmor",&attributes, false);


    mnMP = getRequiredUint("mp", &attributes);



    CL_DomElement child = pElement->get_first_child().to_element();


    while(!child.is_null())
    {
	std::string name = child.get_node_name();

	if(name == "doWeaponDamage")
	{
	    mEffects.push_back ( pFactory->createDoWeaponDamage ( &child ) );
	}
	else if ( name == "doMagicDamage")
	{
	    mEffects.push_back ( pFactory->createDoMagicDamage ( &child ) );
	}
	else if ( name == "doStatusEffect")
	{
	    mEffects.push_back ( pFactory->createDoStatusEffect ( &child ) );
	}
	else if ( name == "animation")
	{
	    mEffects.push_back ( pFactory->createAnimation( &child ) );
	}
		

	
	child = child.get_next_sibling().to_element();
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
}


std::string Spell::getName() const
{
    return mName;
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


/*

WeaponDamageCategory::WeaponDamageCategory()
{
}

WeaponDamageCategory::WeaponDamageCategory(CL_DomElement * pElement)
{
    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    std::string type = getRequiredString("type", &attributes);

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

    std::string type = getRequiredString("type", &attributes);

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
