
#include "IApplication.h"
#include "GraphicsManager.h"
#include "CharacterManager.h"
#include "Level.h"
#include "Animation.h"
#include "Character.h"
#include "SpriteDefinition.h"
#include "StatusEffect.h"
#include <functional>


using StoneRing::ICharacter;

struct stat_entry
{
    const char * string;
    uint attr;
};

const stat_entry statXMLLookup[] = 
{
    {"hp",ICharacter::CA_HP},
    {"hp_max",ICharacter::CA_MAXHP},
    {"mp",ICharacter::CA_MP},
    {"mp_max",ICharacter::CA_MAXMP},
    {"str",ICharacter::CA_STR},
    {"def",ICharacter::CA_DEF},
    {"dex",ICharacter::CA_DEX},
    {"evd",ICharacter::CA_EVD},
    {"mag",ICharacter::CA_MAG},
    {"rst",ICharacter::CA_RST},
    {"lck",ICharacter::CA_LCK},
    {"joy",ICharacter::CA_JOY},
    {"draw_ill",ICharacter::CA_DRAW_ILL},   //    CA_DRAW_ILL,
    {"draw_stone",ICharacter::CA_DRAW_STONE}, //    CA_DRAW_STONE,
    {"draw_berserk",ICharacter::CA_DRAW_BERSERK}, //  CA_DRAW_BERSERK,
    {"draw_weak",ICharacter::CA_DRAW_WEAK}, // CA_DRAW_WEAK,
    {"draw_paralyzed",ICharacter::CA_DRAW_PARALYZED}, //    CA_DRAW_PARALYZED,
    {"draw_translucent",ICharacter::CA_DRAW_TRANSLUCENT}, 
    {"can_act", ICharacter::CA_CAN_ACT}, //   CA_CAN_ACT,
    {"can_fight",ICharacter::CA_CAN_FIGHT}, // CA_CAN_FIGHT,
    {"can_cast", ICharacter::CA_CAN_CAST}, //  CA_CAN_CAST,
    {"can_skill",ICharacter::CA_CAN_SKILL}, // CA_CAN_SKILL,
    {"can_item", ICharacter::CA_CAN_ITEM},
    {"can_run", ICharacter::CA_CAN_RUN}, //   CA_CAN_RUN,
    {"alive", ICharacter::CA_ALIVE},
    {"encounterRate", ICharacter::CA_ENCOUNTER_RATE},
    {"goldDropRate", ICharacter::CA_GOLD_DROP_RATE},
    {"itemDropRate", ICharacter::CA_ITEM_DROP_RATE},
    {"priceMultiplier", ICharacter::CA_PRICE_MULTIPLIER},
    {"expMultiplier", ICharacter::CA_EXP_MULTIPLIER},
    {"level",ICharacter::CA_LEVEL},
    {"idol_slots",ICharacter::CA_IDOL_SLOTS}
};



StoneRing::ICharacter::eCharacterAttribute StoneRing::ICharacter::CharAttributeFromString(const std::string &str)
{
    uint numberStats = _LAST_CHARACTER_ATTR_;
    
    for(uint i =0; i < numberStats; i++)
    {
        if( str == statXMLLookup[i].string )
        {
            return static_cast<eCharacterAttribute>(statXMLLookup[i].attr);
        }
    }
    return static_cast<StoneRing::ICharacter::eCharacterAttribute>(CA_INVALID);
}

StoneRing::ICharacter::eCommonAttribute StoneRing::ICharacter::CommonAttributeFromString(const std::string &str)
{
    for(int i = _LAST_CHARACTER_ATTR_; i < _LAST_COMMON_ATTR_; i++)
    {
        if(str == statXMLLookup[i].string)
        {
            return static_cast<eCommonAttribute>(statXMLLookup[i].attr);
        }
    }

    return static_cast<eCommonAttribute>(CA_INVALID);
}


uint StoneRing::ICharacter::CAFromString(const std::string &str)
{

    for(int i =0; i < _LAST_COMMON_ATTR_; i++)
    {
        if(str == statXMLLookup[i].string)
        {
            return statXMLLookup[i].attr;
        }
    }
    return CA_INVALID;
}

std::string StoneRing::ICharacter::CAToString(uint v)
{
    for(int i =0; i < _LAST_COMMON_ATTR_; i++)
    {
        if(v == statXMLLookup[i].attr)
        {
            return statXMLLookup[i].string;
        }
    }
    assert(0 && "Invalid attribute");
    return "INVALID";
}




void StoneRing::AttributeFile::assert_real(ICharacter::eCharacterAttribute attr)
{
    if (attr != ICharacter::CA_INVALID && attr < ICharacter::_START_OF_TOGGLES)
        return;
    throw CL_Error("Attribute was not a real value");
}

void StoneRing::AttributeFile::assert_bool(ICharacter::eCharacterAttribute attr)
{
    if(attr > ICharacter::_START_OF_TOGGLES && attr < ICharacter::_END_OF_TOGGLES)
        return;
    throw CL_Error("Attribute was not a real value");
}

StoneRing::AttributeFile::attr_doubles::const_iterator 
StoneRing::AttributeFile::find_attr(ICharacter::eCharacterAttribute attr) const
{
    assert_real(attr);
    return mRealAttributes.find(attr);
}

double 
StoneRing::AttributeFile::find_multiplier(ICharacter::eCharacterAttribute attr)const
{
    assert_real(attr);
    attr_doubles::const_iterator iter = mAttrMultipliers.find(attr);
    if(iter != mAttrMultipliers.end())
    {
        return iter->second; 
    }
    else return 1.0;
}

double
StoneRing::AttributeFile::find_addition(ICharacter::eCharacterAttribute attr)const
{
    assert_real(attr);
    attr_doubles::const_iterator iter = mAttrAdditions.find(attr);
    if(iter != mAttrAdditions.end())
    {
        return iter->second; 
    }
    else return 0;
}

StoneRing::AttributeFile::attr_bools::const_iterator 
StoneRing::AttributeFile::find_toggle(ICharacter::eCharacterAttribute attr)const
{
    assert_bool(attr);
    return mToggles.find(attr);
}

double StoneRing::AttributeFile::getAttribute(ICharacter::eCharacterAttribute attr)const
{
    assert_real(attr);
    attr_doubles::const_iterator 
        iter = mRealAttributes.find(attr);
    if(iter != mRealAttributes.end())
    {
        double base = iter->second; 
        return base * find_multiplier(attr) + find_addition(attr);
    }
    throw CL_Error("Attribute not set");
    return 0.0;
}

bool StoneRing::AttributeFile::getToggle(ICharacter::eCharacterAttribute attr)const
{
    assert_bool(attr);
    attr_bools::const_iterator 
        iter = mToggles.find(attr);
    if(iter != mToggles.end())
    {
        return iter->second; 
    }
    throw CL_Error("Attribute not set");
    return false;
}
void StoneRing::AttributeFile::attachMultiplication(ICharacter::eCharacterAttribute attr,double value)
{
    assert_real(attr);
    if(mAttrMultipliers.find(attr) == mAttrMultipliers.end())
    {
        mAttrMultipliers[attr] = value;
    }
    else
    {
        mAttrMultipliers[attr] *= value;
    }
}

void StoneRing::AttributeFile::attachAddition(ICharacter::eCharacterAttribute attr,double value)
{
    assert_real(attr);
    if(mAttrMultipliers.find(attr) == mAttrMultipliers.end())
    {
        mAttrMultipliers[attr] = value;
    }
    else
    {
        mAttrMultipliers[attr] += value;
    }
}

void StoneRing::AttributeFile::detachMultiplication(ICharacter::eCharacterAttribute attr,double value)
{
    assert_real(attr);
    if(value != 0.0)
    {
        if(mAttrMultipliers.find(attr) == mAttrMultipliers.end())
        {
            throw CL_Error("Attempt to detach when there was no attach!");
        }
        else
        {
            mAttrMultipliers[attr] /= value;
        }
    }
}

void StoneRing::AttributeFile::detachAddition(ICharacter::eCharacterAttribute attr,double value)
{
    assert_real(attr);
    if(mAttrMultipliers.find(attr) == mAttrMultipliers.end())
    {
        mAttrMultipliers[attr] = value;
    }
    else
    {
        mAttrMultipliers[attr] -= value;
    }
}


void StoneRing::AttributeFile::fixAttribute(ICharacter::eCharacterAttribute attr, double value)
{
    assert_real(attr);
    mRealAttributes[attr] = value;
}

void StoneRing::AttributeFile::fixAttribute(ICharacter::eCharacterAttribute attr, bool toggle)
{
    assert_bool(attr);
    mToggles[attr] = toggle;
}

StoneRing::Character::Character()
{
}

ICharacter::eGender StoneRing::Character::getGender() const
{
    return NEUTER;
}

bool StoneRing::Character::handleElement(eElement element, StoneRing::Element * pElement)
{
    switch(element)
    {
    case ESPRITEDEFINITION:
        {
            SpriteDefinition * pSpriteDef = dynamic_cast<SpriteDefinition*>(pElement);
            mSpriteDefinitionMap[pSpriteDef->getName()] = pSpriteDef;
            break;
        }
    default:
        return false;
    }

    return true;
}

void StoneRing::Character::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{

    const CharacterManager * pCharacterManager = IApplication::getInstance()->getCharacterManager();
    mName = getRequiredString("name",pAttributes);
    mSpriteRef = getRequiredString("spriteResource",pAttributes);
    std::string className = getRequiredString("class",pAttributes);
    std::string typeName = getImpliedString("type",pAttributes,"living");

    if(typeName == "living")
    {
        meType = LIVING;
    }
    else if(typeName == "nonliving")
    {
        meType = NONLIVING;
    }
    else if(typeName == "magical")
    {
        meType = MAGICAL;
    }
    else
    {
        throw CL_Error("Character type is invalid.");
    }

    // Get the class pointer
    mpClass = pCharacterManager->getClass(className);

}

void StoneRing::Character::loadFinished()
{
    // TODO: Make sure the battle sprites exist in the resources
}


double StoneRing::Character::getAttribute(eCharacterAttribute attr) const
{
    return mAttributes.getAttribute(attr);
}

bool StoneRing::Character::getToggle(eCharacterAttribute attr) const
{
    return mAttributes.getToggle(attr);
}


void StoneRing::Character::fixAttribute(eCharacterAttribute attr, bool value)
{
    mAttributes.fixAttribute(attr,value);
}

void StoneRing::Character::fixAttribute(eCharacterAttribute attr, double value)
{
    mAttributes.fixAttribute(attr,value);
}

void StoneRing::Character::attachMultiplication(eCharacterAttribute attr, double factor)
{
    mAttributes.attachMultiplication(attr,factor);
}

void StoneRing::Character::attachAddition(eCharacterAttribute attr, double value)
{
    mAttributes.attachAddition(attr,value);
}

void StoneRing::Character::detachMultiplication(eCharacterAttribute attr, double factor)
{
    mAttributes.detachMultiplication(attr,factor);
}

void StoneRing::Character::detachAddition(eCharacterAttribute attr, double value)
{
    mAttributes.detachAddition(attr,value);
}

void StoneRing::Character::addStatusEffect(StatusEffect *pEffect)
{
    mStatusEffects.insert(StatusEffectMap::value_type(pEffect->getName(),pEffect));
}

void StoneRing::Character::removeEffects(const std::string &name)
{
    StatusEffectMap::iterator start = mStatusEffects.lower_bound(name);
    StatusEffectMap::iterator end   = mStatusEffects.upper_bound(name);

    mStatusEffects.erase(start,end);
}

void StoneRing::Character::statusEffectRound()
{
}

double StoneRing::Character::getSpellResistance(Magic::eMagicType /*type*/) const
{
    return 0;
}
