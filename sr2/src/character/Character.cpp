
#include "IApplication.h"
#include "GraphicsManager.h"
#include "CharacterManager.h"
#include "Level.h"
#include "Animation.h"
#include "Character.h"
#include "SpriteDefinition.h"
#include "StatusEffect.h"
#include "Equipment.h"
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
    {"draw_mini",ICharacter::CA_DRAW_MINI},
    {"visible", ICharacter::CA_VISIBLE},
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
    uint numberStats = sizeof(statXMLLookup) / sizeof(stat_entry);

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
    uint numberStats = sizeof(statXMLLookup) / sizeof(stat_entry);

    for(uint i =0; i < numberStats; i++)
    {
        if( str == statXMLLookup[i].string )
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

bool StoneRing::ICharacter::IsTransient(eCharacterAttribute attr)
{
    return (attr > ICharacter::_START_OF_TRANSIENTS && attr < ICharacter::_START_OF_INTS);
}

bool StoneRing::ICharacter::IsReal(eCharacterAttribute attr)
{
    return (attr > ICharacter::_START_OF_REALS && attr < ICharacter::_START_OF_TOGGLES);
}

bool StoneRing::ICharacter::IsToggle(eCharacterAttribute attr)
{
    return (attr > ICharacter::_START_OF_TOGGLES && attr < ICharacter::_LAST_CHARACTER_ATTR_);
}

ICharacter::eCharacterAttribute StoneRing::ICharacter::GetMaximumAttribute(eCharacterAttribute attr)
{
    if(ICharacter::_MAXIMA_BASE + attr < ICharacter::_LAST_CHARACTER_ATTR_)
    {
        // There IS a maximum for this guy
        return static_cast<eCharacterAttribute>(_MAXIMA_BASE + attr);
    }

    return ICharacter::_LAST_CHARACTER_ATTR_;
}


StoneRing::Character::Character():m_pClass(NULL)
{
    set_toggle_defaults();
}

void StoneRing::Character::set_toggle_defaults()
{
    for(uint toggle=_START_OF_TOGGLES;toggle<_END_OF_TOGGLES;toggle++)
    {
        switch(toggle)
        {
            case CA_DRAW_ILL:
            case CA_DRAW_STONE:
            case CA_DRAW_BERSERK:
            case CA_DRAW_WEAK:
            case CA_DRAW_PARALYZED:
            case CA_DRAW_TRANSLUCENT:
            case CA_DRAW_MINI:
                m_toggles[static_cast<eCharacterAttribute>(toggle)] = false;
                break;
            case CA_VISIBLE:
            case CA_CAN_ACT:
            case CA_CAN_FIGHT:
            case CA_CAN_CAST:
            case CA_CAN_SKILL:
            case CA_CAN_ITEM:
            case CA_CAN_RUN:
            case CA_ALIVE:
                m_toggles[static_cast<eCharacterAttribute>(toggle)] = true;
                break;
            default:
                break;
        }
    }
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

void StoneRing::Character::Attacked()
{
    // Go through armors calling Invoke on them
}


double StoneRing::Character::GetDamageCategoryResistance(eDamageCategory type) const
{
    if(type == HOLY)
        return -1.0;
    else
        return 1.0;
}


void StoneRing::Character::PermanentAugment(eCharacterAttribute attr, double augment)
{
    std::map<eCharacterAttribute,double>::iterator aug = m_augments.find(attr);
    if(aug == m_augments.end())
        m_augments[attr] = augment;
    else  aug->second += augment;

    if(IsTransient(attr))
    {
        if(m_augments[attr] > GetAttribute ( GetMaximumAttribute(attr) ) )
        {
            m_augments[attr] = GetAttribute ( GetMaximumAttribute(attr) );
        }
    }
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

void StoneRing::Character::load_attributes(CL_DomNamedNodeMap attributes)
{

    const CharacterManager * pCharacterManager = IApplication::GetInstance()->GetCharacterManager();
    m_name = get_required_string("name",attributes);
    std::string spriteRef = get_required_string("spriteResource",attributes);
    std::string className = get_required_string("class",attributes);
    std::string typeName = get_implied_string("type",attributes,"living");

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
        throw CL_Exception("Character type is invalid.");
    }

    // Get the class pointer
    m_pClass = pCharacterManager->GetClass(className);

    CL_ResourceManager&  resources = IApplication::GetInstance()->GetResources();
    m_mapSprite = CL_Sprite(GET_MAIN_GC(),spriteRef, &resources);

}

void StoneRing::Character::load_finished()
{
    // TODO: Make sure the battle sprites exist in the resources
}

double StoneRing::Character::GetBaseAttribute(eCharacterAttribute attr)const
{
    double augment  = 0.0;
    double base = m_pClass->GetStat(attr,m_nLevel);
    std::map<eCharacterAttribute,double>::const_iterator aug = m_augments.find(attr);
    if(aug != m_augments.end())
        augment = aug->second;

    if(IsInteger(attr))
        return static_cast<int>(base + augment);
    else return base + augment;
}

// Note:
// If this shit takes too long at any point, another way we can do it is to
// have all the shit register AttributeModifiers with us directly which
// we will stick in a multimap by attr. Alternatively, we can register/unregister
// them ourselves when we equip/unequip or are affected/unaffected. But
// this requires remembering which AM goes with which equipment/status effect
double StoneRing::Character::GetAttribute(eCharacterAttribute attr) const
{
    double base = 0.0;
    if(!IsTransient(attr))
    {
        base = m_pClass->GetStat(attr,m_nLevel);
        // Go through all equipment, multiplying by the AMs that are mults
        for(std::map<Equipment::eSlot,Equipment*>::const_iterator iter= m_equipment.begin();
            iter != m_equipment.end();iter++)
        {
            base *= iter->second->GetAttributeMultiplier(attr);
        }
        for(StatusEffectMap::const_iterator iter= m_status_effects.begin();
            iter != m_status_effects.end();iter++)
        {
            base *= iter->second->GetAttributeMultiplier(attr);
        }
        // Same with the status effects
        // Then do it with the adders
        for(std::map<Equipment::eSlot,Equipment*>::const_iterator iter= m_equipment.begin();
            iter != m_equipment.end();iter++)
        {
            base += iter->second->GetAttributeAdd(attr);
        }
        for(StatusEffectMap::const_iterator iter= m_status_effects.begin();
            iter != m_status_effects.end();iter++)
        {
            base += iter->second->GetAttributeMultiplier(attr);
        }
    }
    double augment  = 0.0;
    std::map<eCharacterAttribute,double>::const_iterator aug = m_augments.find(attr);
    if(aug != m_augments.end())
        augment = aug->second;

    if(IsInteger(attr))
        return static_cast<int>( base + augment );
    else
        return base + augment;

}



bool StoneRing::Character::GetToggle(eCharacterAttribute attr) const
{
    std::map<eCharacterAttribute,bool>::const_iterator iter = m_toggles.find(attr);
    if(iter != m_toggles.end())
        return iter->second;
    else return false;
}


void StoneRing::Character::SetToggle(eCharacterAttribute attr, bool value)
{
    m_toggles[attr] = value;
}

bool StoneRing::Character::HasEquipment(Equipment::eSlot slot)
{
    return m_equipment.find(slot) != m_equipment.end();
}

// Equipment. If theres equipment in this slot already,
// this overwrites it.
void StoneRing::Character::Equip(Equipment::eSlot slot, Equipment *pEquip)
{
    m_equipment[slot] = pEquip;
}

StoneRing::Equipment* StoneRing::Character::GetEquipment(Equipment::eSlot slot)
{
    return m_equipment[slot];
}

// Returns a pointer to the equipment that was in that slot
StoneRing::Equipment* StoneRing::Character::Unequip(Equipment::eSlot slot)
{
    Equipment * there = NULL;
    std::map<Equipment::eSlot,Equipment*>::iterator it = m_equipment.find(slot);

    if(it != m_equipment.end()){
        there = it->second;
        m_equipment.erase(it);
    }

    return there;
}

double StoneRing::Character::GetEquippedWeaponAttribute(Weapon::eAttribute attr) const
{
    double v = 0.0;
    for(std::map<Equipment::eSlot,Equipment*>::const_iterator it=m_equipment.begin();
        it!=m_equipment.end();it++)
    {
        if(it->second->IsWeapon()){
            Weapon* weapon = dynamic_cast<Weapon*>(it->second);
            v += weapon->GetWeaponAttribute(attr);
        }
    }

    return v;
}
double StoneRing::Character::GetEquippedArmorAttribute(Armor::eAttribute attr) const
{
    double v = 0.0;
    for(std::map<Equipment::eSlot,Equipment*>::const_iterator it=m_equipment.begin();
        it!=m_equipment.end();it++)
    {
        if(it->second->IsArmor()){
            Armor* armor = dynamic_cast<Armor*>(it->second);
            v += armor->GetArmorAttribute(attr);
        }
    }

    return v;
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

StoneRing::BattleMenu * StoneRing::Character::GetBattleMenu() const
{
    return m_pClass->GetBattleMenu();
}
