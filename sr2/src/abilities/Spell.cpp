#include "Spell.h"
#include "AbilityFactory.h"
#include "IApplication.h"
#include "StatusEffect.h"
#include "sr_defines.h"
#include "Animation.h"
#include "DamageCategory.h"
#include "SpellRef.h"
#include "Magic.h"

using namespace StoneRing;


Spell::Spell()
{
}

void Spell::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name", pAttributes);
    meType = Magic::typeOf(getRequiredString("type", pAttributes));

    if(meType == Magic::UNKNOWN)
    {
        throw CL_Error("Bad magic type in spell.");
    }

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


std::string Spell::getName() const
{
    return mName;
}

uint Spell::getValue() const
{
    return mnValue;
}


Magic::eMagicType Spell::getMagicType() const
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
   
    ref->setType( meType );
    ref->setName( mName );

    return ref;
}





MagicResistance::MagicResistance():meType(Magic::UNKNOWN)
{
}

void MagicResistance::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    meType = Magic::typeOf(getRequiredString("type",pAttributes));

    if(meType == Magic::UNKNOWN)
        throw CL_Error("Bad magic type on magicResistance");

    mfResistance = getRequiredFloat("resist", pAttributes);
}


MagicResistance::~MagicResistance()
{
}



float MagicResistance::getResistance() const
{
    return mfResistance;
}



Magic::eMagicType MagicResistance::getType() const
{
    return meType;
}


