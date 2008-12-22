
#include "IApplication.h"
#include "GraphicsManager.h"
#include "CharacterManager.h"
#include "Level.h"
#include "Animation.h"
#include "Character.h"
#include "SpriteDefinition.h"
#include "StatusEffect.h"
#include <functional>
#include <ClanLib/core.h>


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

bool StoneRing::ICharacter::IsInteger(eCharacterAttribute attr)
{
    return (attr > ICharacter::_START_OF_INTS && attr < ICharacter::_START_OF_REALS) ||
        (attr > ICharacter::_MAXIMA_BASE && attr < ICharacter::_LAST_CHARACTER_ATTR_);
}

bool StoneRing::ICharacter::IsReal(eCharacterAttribute attr)
{
    return (attr > ICharacter::_START_OF_REALS && attr < ICharacter::_START_OF_TOGGLES);
}

bool StoneRing::ICharacter::IsToggle(eCharacterAttribute attr)
{
    return (attr > ICharacter::_START_OF_TOGGLES && attr < ICharacter::_LAST_CHARACTER_ATTR_);
}





StoneRing::Character::Character():m_pClass(NULL),m_pMapSprite(NULL)
{
}

uint StoneRing::Character::GetLevel(void)const
{
    return m_nLevel;
}

void StoneRing::Character::SetLevel(uint level)
{
    m_nLevel = level;
}

ICharacter::eGender StoneRing::Character::GetGender() const
{
    return NEUTER;
}

void StoneRing::Character::Kill()
{
    SetToggle(CA_ALIVE,false);
}

void StoneRing::Character::PermanentAugment(eCharacterAttribute attr, int augment)
{
    std::map<eCharacterAttribute,int>::iterator aug = m_augments.find(attr);
    if(aug == m_augments.end())
        m_augments[attr] = augment;
    else aug->second += augment;
}

void StoneRing::Character::PermanentAugment(eCharacterAttribute attr, double augment)
{
    std::map<eCharacterAttribute,double>::iterator aug = m_real_augments.find(attr);
    if(aug == m_real_augments.end())
        m_real_augments[attr] = augment;
    else  aug->second += augment;
}


bool StoneRing::Character::handle_element(eElement element, StoneRing::Element * pElement)
{
    switch(element)
    {
    case ESPRITEDEFINITION:
        {
            SpriteDefinition * pSpriteDef = dynamic_cast<SpriteDefinition*>(pElement);
            m_sprite_definition_map[pSpriteDef->GetName()] = pSpriteDef;
            break;
        }
    default:
        return false;
    }

    return true;
}

void StoneRing::Character::load_attributes(CL_DomNamedNodeMap *pAttributes)
{

    const CharacterManager * pCharacterManager = IApplication::GetInstance()->GetCharacterManager();
    m_name = get_required_string("name",pAttributes);
    std::string spriteRef = get_required_string("spriteResource",pAttributes);
    std::string className = get_required_string("class",pAttributes);
    std::string typeName = get_implied_string("type",pAttributes,"living");

    if(typeName == "living")
    {
        m_eType = LIVING;
    }
    else if(typeName == "nonliving")
    {
        m_eType = NONLIVING;
    }
    else if(typeName == "magical")
    {
        m_eType = MAGICAL;
    }
    else
    {
        throw CL_Error("Character type is invalid.");
    }

    // Get the class pointer
    m_pClass = pCharacterManager->GetClass(className);

    CL_ResourceManager * pResources = IApplication::GetInstance()->GetResources();
    m_pMapSprite = new CL_Sprite(spriteRef, pResources);

}

void StoneRing::Character::load_finished()
{
    // TODO: Make sure the battle sprites exist in the resources
}


double StoneRing::Character::GetAttributeReal(eCharacterAttribute attr) const
{
    double base = m_pClass->GetStatReal(attr,m_nLevel);
    // Go through all equipment, multiplying by the AMs that are mults
    // Same with the status effects
    // Then do it with the adders
    double augment  = 0.0;
    std::map<eCharacterAttribute,double>::const_iterator aug = m_real_augments.find(attr);
    if(aug != m_real_augments.end())
        augment = aug->second;
    return base + augment;
}

int StoneRing::Character::GetAttribute(eCharacterAttribute attr) const
{
    int base = m_pClass->GetStat(attr,m_nLevel);
    // Go through all equipment, multiplying by the AMs that are mults
    // Same with the status effects
    // Then do it with the adders
    int augment  = 0;
    std::map<eCharacterAttribute,int>::const_iterator aug = m_augments.find(attr);
    if(aug != m_augments.end())
        augment = aug->second;
    return base + augment;
}


bool StoneRing::Character::GetToggle(eCharacterAttribute attr) const
{
    return false;
}


void StoneRing::Character::SetToggle(eCharacterAttribute attr, bool value)
{
   
}


void StoneRing::Character::AddStatusEffect(StoneRing::StatusEffect *pEffect)
{
    m_status_effects.insert(StatusEffectMap::value_type(pEffect->GetName(),pEffect));
}

void StoneRing::Character::RemoveEffects(const std::string &name)
{
    StatusEffectMap::iterator start = m_status_effects.lower_bound(name);
    StatusEffectMap::iterator end   = m_status_effects.upper_bound(name);

    m_status_effects.erase(start,end);
}

void StoneRing::Character::StatusEffectRound()
{
}

double StoneRing::Character::GetSpellResistance(Magic::eMagicType /*type*/) const
{
    return 0;
}
