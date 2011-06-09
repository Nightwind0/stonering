#include "Spell.h"
#include "IApplication.h"
#include "StatusEffect.h"
#include "sr_defines.h"
#include "Animation.h"
#include "DamageCategory.h"
#include "SpellRef.h"
#include "Magic.h"

using namespace StoneRing;


Spell::Spell()
:mpMagicResistance(NULL)
{
}

void Spell::load_attributes(CL_DomNamedNodeMap attributes)
{
    mName = get_required_string("name", attributes);
    meType = Magic::TypeOf(get_required_string("type", attributes));

    if(meType == Magic::UNKNOWN)
    {
        throw CL_Exception("Bad magic type in spell.");
    }

#if 0
    meUse = getUseFromString(get_required_string("use", attributes));
    meTargetable = getTargetableFromString(get_required_string("targetable", attributes));
#endif

    mbAppliesToWeapons = get_implied_bool ("appliesToWeapons",attributes, false);
    mbAppliesToArmor = get_implied_bool ("appliesToArmor",attributes, false);

    mnValue = get_required_uint("value",attributes);

    m_damageCategory = DamageCategory::DamageCategoryFromString(get_required_string("damageCategory",attributes));
}

bool Spell::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {

    case EMAGICRESISTANCE:
        mpMagicResistance = dynamic_cast<MagicResistance*>(pElement);
        break;
    default:
        return false;
    }

    return true;
}

#if 0
Spell::eUse Spell::getUseFromString ( const std::string &str)
{
    if(str == "battle") return BATTLE;
    else if (str == "world") return WORLD;
    else if (str == "both") return BOTH;
    else throw CL_Exception("Bad spell use");
}

Spell::eTargetable Spell::getTargetableFromString ( const std::string &str)
{
    if(str == "all") return ALL;
    else if (str == "single") return SINGLE;
    else if (str == "either") return EITHER;
    else if (str == "self-only") return SELF_ONLY;
    else throw CL_Exception("Bad spell targetable.");
}
#endif

Spell::~Spell()
{
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



bool Spell::appliesToWeapons() const
{
    return mbAppliesToWeapons;
}

bool Spell::appliesToArmor() const
{
    return mbAppliesToArmor;
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

MagicResistance * Spell::getMagicResistance() const
{
    return mpMagicResistance;
}




MagicResistance::MagicResistance():meType(Magic::UNKNOWN)
{
}

void MagicResistance::load_attributes(CL_DomNamedNodeMap attributes)
{
    meType = Magic::TypeOf(get_required_string("type",attributes));

    if(meType == Magic::UNKNOWN)
        throw CL_Exception("Bad magic type on magicResistance");

    m_fResistance = get_required_float("resist", attributes);
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


