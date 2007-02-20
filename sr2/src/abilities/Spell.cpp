#include "Spell.h"
#include "AbilityFactory.h"
#include "IApplication.h"
#include "StatusEffect.h"
#include "sr_defines.h"
#include "Animation.h"
#include "DamageCategory.h"
#include "SpellRef.h"

using namespace StoneRing;


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

bool Spell::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESCRIPT:
        mpScript = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EMAGICRESISTANCE:
        mpMagicResistance = dynamic_cast<MagicResistance*>(pElement);
        break;
    default:
        return false;
    }

    return true;
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
    delete mpScript;
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


