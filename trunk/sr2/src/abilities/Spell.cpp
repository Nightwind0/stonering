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
:mpScript(NULL),
mpMagicResistance(NULL)
{
}

void Spell::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = get_required_string("name", pAttributes);
    meType = Magic::TypeOf(get_required_string("type", pAttributes));

    if(meType == Magic::UNKNOWN)
    {
        throw CL_Error("Bad magic type in spell.");
    }

    meUse = getUseFromString(get_required_string("use", pAttributes));
    meTargetable = getTargetableFromString(get_required_string("targetable", pAttributes));

    mbAppliesToWeapons = get_implied_bool ("appliesToWeapons",pAttributes, false);
    mbAppliesToArmor = get_implied_bool ("appliesToArmor",pAttributes, false);

    mnValue = get_required_uint("value",pAttributes);

    m_damageCategory = DamageCategoryFromString(get_required_string("damageCategory",pAttributes));

    mnMP = get_required_uint("mp", pAttributes);
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

    ref->SetType( meType );
    ref->SetName( mName );

    return ref;
}





MagicResistance::MagicResistance():meType(Magic::UNKNOWN)
{
}

void MagicResistance::load_attributes(CL_DomNamedNodeMap *pAttributes)
{
    meType = Magic::TypeOf(get_required_string("type",pAttributes));

    if(meType == Magic::UNKNOWN)
        throw CL_Error("Bad magic type on magicResistance");

    m_fResistance = get_required_float("resist", pAttributes);
}


MagicResistance::~MagicResistance()
{
}



float MagicResistance::GetResistance() const
{
    return m_fResistance;
}



Magic::eMagicType MagicResistance::GetType() const
{
    return meType;
}


