
#include "IApplication.h"
#include "GraphicsManager.h"
#include "CharacterManager.h"
#include "Level.h"
#include "Animation.h"
#include "Character.h"
#include "SpriteDefinition.h"
#include "StatusEffect.h"
#include "Equipment.h"
#include "StatusEffectModifier.h"
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
    {"bp",ICharacter::CA_BP},
    {"bp_max",ICharacter::CA_MAXBP},
    {"str",ICharacter::CA_STR},
    {"def",ICharacter::CA_DEF},
    {"dex",ICharacter::CA_DEX},
    {"evd",ICharacter::CA_EVD},
    {"mag",ICharacter::CA_MAG},
    {"rst",ICharacter::CA_RST},
    {"lck",ICharacter::CA_LCK},
    {"joy",ICharacter::CA_JOY},
    {"bash_def",ICharacter::CA_BASH_DEF},
    {"pierce_def",ICharacter::CA_PIERCE_DEF},
    {"slash_def",ICharacter::CA_SLASH_DEF},
    {"wind_rst",ICharacter::CA_WIND_RST},
    {"fire_rst",ICharacter::CA_FIRE_RST},
    {"water_rst",ICharacter::CA_WATER_RST},
    {"earth_rst",ICharacter::CA_EARTH_RST},
    {"dark_rst",ICharacter::CA_DARK_RST},
    {"holy_rst",ICharacter::CA_HOLY_RST},
    {"gravity_rst",ICharacter::CA_GRAVITY_RST},
    {"electric_rst",ICharacter::CA_ELECTRIC_RST},
    {"draw_ill",ICharacter::CA_DRAW_ILL},   //    CA_DRAW_ILL,
    {"draw_stone",ICharacter::CA_DRAW_STONE}, //    CA_DRAW_STONE,
    {"draw_berserk",ICharacter::CA_DRAW_BERSERK}, //  CA_DRAW_BERSERK,
    {"draw_weak",ICharacter::CA_DRAW_WEAK}, // CA_DRAW_WEAK,
    {"draw_paralyzed",ICharacter::CA_DRAW_PARALYZED}, //    CA_DRAW_PARALYZED,
    {"draw_translucent",ICharacter::CA_DRAW_TRANSLUCENT},
    {"draw_mini",ICharacter::CA_DRAW_MINI},
    {"draw_flipped",ICharacter::CA_DRAW_FLIPPED},
    {"draw_still",ICharacter::CA_DRAW_STILL},
    {"visible", ICharacter::CA_VISIBLE},
    {"can_act", ICharacter::CA_CAN_ACT}, //   CA_CAN_ACT,
    {"can_fight",ICharacter::CA_CAN_FIGHT}, // CA_CAN_FIGHT,
    {"can_cast", ICharacter::CA_CAN_CAST}, //  CA_CAN_CAST,
    {"can_skill",ICharacter::CA_CAN_SKILL}, // CA_CAN_SKILL,
    {"can_item", ICharacter::CA_CAN_ITEM},
    {"can_run", ICharacter::CA_CAN_RUN}, //   CA_CAN_RUN,
    {"alive", ICharacter::CA_ALIVE},
    {"still",ICharacter::CA_DRAW_STILL},
    {"encounterRate", ICharacter::CA_ENCOUNTER_RATE},
    {"goldDropRate", ICharacter::CA_GOLD_DROP_RATE},
    {"itemDropRate", ICharacter::CA_ITEM_DROP_RATE},
    {"priceMultiplier", ICharacter::CA_PRICE_MULTIPLIER},
    {"expMultiplier", ICharacter::CA_EXP_MULTIPLIER},
    {"idol_slots",ICharacter::CA_IDOL_SLOTS}
};


bool StoneRing::ICharacter::IsDamageCategoryAttribute(StoneRing::ICharacter::eCharacterAttribute attr)
{
    if(attr > StoneRing::ICharacter::_START_OF_DAMAGE_CATEGORIES && attr < StoneRing::ICharacter::_END_OF_DAMAGE_CATEGORIES)
        return true;
    else return false;
}


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

    for(int i =0; i < sizeof(statXMLLookup) / sizeof(stat_entry); i++)
    {
        if(str == statXMLLookup[i].string)
        {
            return statXMLLookup[i].attr;
        }
    }
    return CA_INVALID;
}

std::string StoneRing::ICharacter::CAToLabel(uint a)
{
    switch(a)
    {
        case CA_MAXHP:
            return "MaxHP";
        case CA_MAXMP:
            return "MaxMP";
        default:
            std::string str = CAToString(a);
            str[0] = toupper(str[0]);
            if(IsReal(static_cast<eCharacterAttribute>(a)))
                str += "%";
            return str;
    }
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

bool StoneRing::ICharacter::ToggleDefaultTrue(eCharacterAttribute attr)
{
    assert ( IsToggle ( attr ) );
    
    switch(attr)
    {
        case CA_ALIVE:
        case CA_VISIBLE:
        case CA_CAN_ACT:
        case CA_CAN_RUN:
        case CA_CAN_CAST:
        case CA_CAN_FIGHT:
        case CA_CAN_ITEM:
        case CA_CAN_SKILL:
            return true;
        default:
            return false;
    }
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
    m_nSP = 0;
    set_toggle_defaults();
}


void StoneRing::Character::LearnSkill(const std::string& skill)
{
    m_skillset.insert ( skill );
}

bool StoneRing::Character::HasSkill(const std::string& skill)
{
    for(std::list<SkillTreeNode*>::const_iterator iter = m_pClass->GetSkillTreeNodesBegin();
        iter != m_pClass->GetSkillTreeNodesEnd(); iter++)
        {
            SkillTreeNode* pNode = *iter;
            // If this skill is a top-level skill (no pre-reqs),
            // and has no special restrictions (by, level, etc),
            // then we are considered to have it, regardless of it being in our explicit list
            if(pNode->GetRef()->GetRef() == skill && pNode->GetParent() == NULL 
                && pNode->CanLearn(this) && pNode->GetSPCost() == 0)
                return true;
        }
    return m_skillset.count(skill);
}

void StoneRing::Character::set_toggle_defaults()
{
    for(uint toggle=_START_OF_TOGGLES+1;toggle<_END_OF_TOGGLES;toggle++)
    {
        m_toggles[static_cast<eCharacterAttribute>(toggle)] = ToggleDefaultTrue(static_cast<eCharacterAttribute>(toggle));
    }
}

void StoneRing::Character::RemoveBattleStatusEffects()
{
    for(StatusEffectMap::iterator iter = m_status_effects.begin();
        iter != m_status_effects.end(); iter++)
        {
            if(iter->second->GetLast() == StatusEffect::PERMANENT)
                RemoveEffect(iter->second);
        }
}

uint StoneRing::Character::GetLevel(void)const
{
    return m_nLevel;
}

void StoneRing::Character::SetLevel(uint level)
{
    
}

uint   StoneRing::Character::GetXP()const
{
    return m_nXP;
}

void   StoneRing::Character::SetXP(uint amount)
{
    m_nXP = amount;
    static AstScript * LNT = IApplication::GetInstance()->GetUtility(IApplication::LEVEL_FOR_XP);
    ParameterList params;
    params.push_back(ParameterListItem("$_XP",(int)m_nXP));
    m_nLevel = IApplication::GetInstance()->RunScript(LNT,params);
}

uint StoneRing::Character::GetSP() const
{
    return m_nSP;
}

void StoneRing::Character::SetSP(uint amount)
{
    m_nSP = amount;
}

StoneRing::ICharacter::eGender StoneRing::Character::GetGender() const
{
    return NEUTER;
}

void StoneRing::Character::Kill()
{
    SetToggle(CA_ALIVE,false);
    SetCurrentSprite(GraphicsManager::CreateCharacterSprite(m_name,"dead"));
}

void StoneRing::Character::Raise()
{
    SetToggle(CA_ALIVE,true);
    // TODO: Maybe draw weak
    SetCurrentSprite(GraphicsManager::CreateCharacterSprite(m_name,"idle"));
}

void StoneRing::Character::Attacked(ICharacter* pAttacker, DamageCategory::eDamageCategory category, bool melee, int amount)
{
    ParameterList params;
    params.push_back(ParameterListItem("$_Character",this));
    params.push_back(ParameterListItem("$_Attacker",pAttacker));
    params.push_back(ParameterListItem("$_Category",static_cast<int>(category)));
    params.push_back(ParameterListItem("$_Amount",amount));
    params.push_back(ParameterListItem("$_Melee",melee));
    
    for(std::map<Equipment::eSlot,Equipment*>::const_iterator iter = m_equipment.begin();
        iter != m_equipment.end(); iter++)
    {
        Armor * pArmor = dynamic_cast<Armor*>(iter->second);
        if(pArmor){
            pArmor->Invoke(params);
        }
    }
    
    AstScript * pScript = IApplication::GetInstance()->GetUtility(IApplication::ON_ATTACK);
    if(pScript)
    {
        IApplication::GetInstance()->RunScript(pScript,params);
    }
}


double StoneRing::Character::GetDamageCategoryResistance(DamageCategory::eDamageCategory type) const
{
    switch(type)
    {
        case DamageCategory::WIND:
            return GetAttribute(CA_WIND_RST);
        case DamageCategory::WATER:
            return GetAttribute(CA_WATER_RST);
        case DamageCategory::EARTH:
            return GetAttribute(CA_EARTH_RST);
        case DamageCategory::FIRE:
            return GetAttribute(CA_FIRE_RST);
        case DamageCategory::BASH:
            return GetAttribute(CA_BASH_DEF);
        case DamageCategory::PIERCE:
            return GetAttribute(CA_PIERCE_DEF);
        case DamageCategory::SLASH:
            return GetAttribute(CA_SLASH_DEF);
        case DamageCategory::DARK:
            return GetAttribute(CA_DARK_RST);
        case DamageCategory::HOLY:
            return GetAttribute(CA_HOLY_RST);
    }


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

double StoneRing::Character::GetAttributeWithoutEquipment ( ICharacter::eCharacterAttribute attr, StoneRing::Equipment* pExclude ) const
{
    double base = 0.0;
     
    if(!IsTransient(attr))
    {
        if(ICharacter::IsDamageCategoryAttribute(attr))
        {
            base = 1.0;
            if(attr == ICharacter::CA_HOLY_RST)
                base = -1.0;
        }
        else 
        {
            base = m_pClass->GetStat(attr,GetLevel());
        }
        // Go through all equipment, multiplying by the AMs that are mults
        for(std::map<Equipment::eSlot,Equipment*>::const_iterator iter= m_equipment.begin();
            iter != m_equipment.end();iter++)
        {
            assert(iter->second);
            if(pExclude != iter->second)
                base *=  iter->second->GetAttributeMultiplier(attr);
        }
        for(StatusEffectMap::const_iterator iter= m_status_effects.begin();
            iter != m_status_effects.end();iter++)
        {
            base *=  iter->second->GetAttributeMultiplier(attr);
        }
        // Same with the status effects
        // Then do it with the adders
        for(std::map<Equipment::eSlot,Equipment*>::const_iterator iter= m_equipment.begin();
            iter != m_equipment.end();iter++)
        {
            assert(iter->second);
            if(pExclude != iter->second)
                base += iter->second->GetAttributeAdd(attr);
        }
        for(StatusEffectMap::const_iterator iter= m_status_effects.begin();
            iter != m_status_effects.end();iter++)
        {
            base += iter->second->GetAttributeAdd(attr);
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


double StoneRing::Character::GetBaseAttribute(eCharacterAttribute attr)const
{
    double augment  = 0.0;
    double base = m_pClass->GetStat(attr,GetLevel());
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
    return GetAttributeWithoutEquipment(attr,NULL);
}



bool StoneRing::Character::GetToggle(eCharacterAttribute attr) const
{
    bool base = ToggleDefaultTrue(attr);
    std::map<eCharacterAttribute,bool>::const_iterator iter = m_toggles.find(attr);
    if(iter != m_toggles.end())
        base =  iter->second;
    
    for(StatusEffectMap::const_iterator iter= m_status_effects.begin();
            iter != m_status_effects.end();iter++)
        {
            if(ToggleDefaultTrue(attr))
                 base = base && iter->second->GetAttributeToggle(attr, base);
            else
                base = base || iter->second->GetAttributeToggle(attr, base);
        }   
        
    for(std::map<Equipment::eSlot,Equipment*>::const_iterator iter = m_equipment.begin();
        iter != m_equipment.end(); iter++)
        {
            if(ToggleDefaultTrue(attr))
                 base = base && iter->second->GetAttributeToggle(attr, base);
            else
                base = base || iter->second->GetAttributeToggle(attr, base);              
        }
    return base;
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
    assert(pEquip);
    m_equipment[slot] = pEquip;
}

StoneRing::Equipment* StoneRing::Character::GetEquipment(Equipment::eSlot slot)
{
    return m_equipment[slot];
}

CL_Sprite StoneRing::Character::GetPortrait(ePortrait portrait)
{
    if(m_portrait.is_null())
    {
	m_portrait = GraphicsManager::GetPortraits(GetName());
    }
    m_portrait.set_frame(static_cast<int>(portrait));
    return m_portrait;
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
        assert(it->second);
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
        assert(it->second);
        if(it->second->IsArmor()){
            Armor* armor = dynamic_cast<Armor*>(it->second);
            v += armor->GetArmorAttribute(attr);
        }
    }

    return v;
}


void StoneRing::Character::AddStatusEffect(StoneRing::StatusEffect *pEffect)
{
    //RemoveEffect(pEffect);
    m_status_effect_rounds[pEffect->GetName()] = 0;
    m_status_effects[pEffect->GetName()] = pEffect;
    ParameterList params;
    params.push_back(ParameterListItem("$_Character",this));
    pEffect->Invoke(params);
}

void StoneRing::Character::RemoveEffect(StoneRing::StatusEffect *pEffect)
{
    m_status_effects.erase(pEffect->GetName());
    m_status_effect_rounds.erase(pEffect->GetName());
    ParameterList params;
    params.push_back(ParameterListItem("$_Character",this));
    pEffect->Remove(params);
}

double StoneRing::Character::StatusEffectChance(StoneRing::StatusEffect *pEffect) const
{
        double chance = 1.0;
        for(std::map<Equipment::eSlot,Equipment*>::const_iterator iter= m_equipment.begin();
            iter != m_equipment.end();iter++)
        {
            chance += iter->second->GetStatusEffectModifier(pEffect->GetName());
        }
        for(StatusEffectMap::const_iterator iter = m_status_effects.begin();
            iter != m_status_effects.end(); iter++)
        {
            chance += iter->second->GetStatusEffectModifier(pEffect->GetName());
        }
        
        return chance;
}

void StoneRing::Character::StatusEffectRound()
{   
    for(StatusEffectMap::iterator iter = m_status_effects.begin();
        iter != m_status_effects.end(); iter++)
        {
            StatusEffect * pEffect = iter->second;
            uint round = ++m_status_effect_rounds[pEffect->GetName()];
            ParameterList params;
            params.push_back(ParameterListItem("$_Character",this));
            params.push_back(ParameterListItem("$_EffectRound",static_cast<int>(round)));
            if(pEffect->GetLast() == StatusEffect::ROUND_COUNT)
            {
                if(round <= pEffect->GetRoundCount())
                    iter->second->Round(params);
                else
                    RemoveEffect(pEffect); // Times up!
            }
            else
            {
                iter->second->Round(params);
            }
        }
}

void StoneRing::Character::IterateStatusEffects ( Visitor< StoneRing::StatusEffect* >& visitor)
{
    for(StatusEffectMap::iterator iter = m_status_effects.begin();
        iter != m_status_effects.end();
        iter++)
        {
            visitor.Visit(iter->second);
        }
}



StoneRing::BattleMenu * StoneRing::Character::GetBattleMenu() const
{
    return m_pClass->GetBattleMenu();
}

CL_Sprite StoneRing::Character::GetCurrentSprite(bool pure)
{
    if(!pure){
       // m_currentSprite.set_color(CL_Colorf::white);
        m_currentSprite.set_scale(1.0,1.0);
        m_currentSprite.set_alpha(1.0);
    }
    // preserve alpha, in case we set the color
    float alpha = m_currentSprite.get_alpha();
    if(GetToggle(ICharacter::CA_DRAW_ILL))
        m_currentSprite.set_color(CL_Colorf::palegreen);
    if(GetToggle(ICharacter::CA_DRAW_BERSERK))
        m_currentSprite.set_color(CL_Colorf::red);
    if(GetToggle(ICharacter::CA_DRAW_MINI)){
        m_currentSprite.set_scale(0.5,0.5);
    }
    if(GetToggle(ICharacter::CA_DRAW_FLIPPED)){
        m_currentSprite.set_scale(-1.0,0.5);
    }
    if(GetToggle(ICharacter::CA_DRAW_MINI) &&
        GetToggle(ICharacter::CA_DRAW_FLIPPED)){
        m_currentSprite.set_scale(-0.5,0.5);
    }
    if(GetToggle(ICharacter::CA_DRAW_TRANSLUCENT)){
        m_currentSprite.set_alpha(0.25);
    }
    if(GetToggle(ICharacter::CA_DRAW_STONE))
        m_currentSprite.set_color(CL_Colorf::gray40);
    if(GetToggle(ICharacter::CA_DRAW_PARALYZED))
        m_currentSprite.set_color(CL_Colorf::purple);

    m_currentSprite.set_alpha(alpha);
    return m_currentSprite;
}

void StoneRing::Character::Serialize ( std::ostream& out )
{
    WriteString(out,m_name);
    out.write((char*)&m_nLevel,sizeof(uint));
    out.write((char*)&m_nXP,sizeof(uint));
    out.write((char*)&m_nSP,sizeof(uint));
    out.write((char*)&m_nBP,sizeof(uint));
    
    uint augment_count = m_augments.size();
    out.write((char*)&augment_count,sizeof(uint));
    for(std::map<eCharacterAttribute,double>::const_iterator iter = m_augments.begin();
        iter != m_augments.end(); iter++){
            out.write((char*)&iter->first,sizeof(eCharacterAttribute));
            out.write((char*)&iter->second,sizeof(double));
    }
    
    uint toggle_count = m_toggles.size();
    out.write((char*)&toggle_count,sizeof(uint));
    for(std::map<eCharacterAttribute,bool>::const_iterator iter = m_toggles.begin();
        iter != m_toggles.end(); iter++){
        out.write((char*)&iter->first,sizeof(eCharacterAttribute));
        out.write((char*)&iter->second,sizeof(bool));
    }
    
    uint skill_size = m_skillset.size();
    out.write((char*)&skill_size,sizeof(uint));
    for(std::set<std::string>::const_iterator iter = m_skillset.begin();
        iter != m_skillset.end(); iter++){
        WriteString(out,*iter);
    }
    uint equipment_count =  m_equipment.size();
    for(std::map<Equipment::eSlot,Equipment*>::const_iterator iter = m_equipment.begin();
        iter != m_equipment.end(); iter++){
        out.write((char*)&iter->first,sizeof(uint));
        Equipment* pEquip = iter->second;
        ItemManager::SerializeItem(out,pEquip);        
    }
}

void StoneRing::Character::Deserialize ( std::istream& in )
{
    m_name = ReadString(in);
}







