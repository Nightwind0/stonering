#include "CharacterClass.h"
#include "Skill.h"
#include "Item.h"
#include "WeaponTypeRef.h"
#include "ArmorTypeRef.h"
#include "BattleMenu.h"
#include <algorithm>
#include "WeaponType.h"
#include "ArmorType.h"

using namespace StoneRing;

void CharacterClass::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);

#ifndef NDEBUG
    std::cout << "Loading class: " << m_name << std::endl;
#endif


    std::string gender = get_implied_string("gender",attributes, "either");

    if(gender == "male") m_eGender = ICharacter::MALE;
    else if (gender == "female") m_eGender = ICharacter::FEMALE;
    else if (gender == "either") m_eGender = ICharacter::NEUTER;
}

bool CharacterClass::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case EWEAPONTYPEREF:
        m_weapon_types.push_back ( dynamic_cast<WeaponTypeRef*>(pElement) );
        break;
    case EARMORTYPEREF:
        m_armor_types.push_back ( dynamic_cast<ArmorTypeRef*>(pElement) );
        break;
    case ESTATSCRIPT:
        {
            StatScript *pScript = dynamic_cast<StatScript*>(pElement);
            m_stat_scripts[pScript->GetCharacterStat()] = pScript;
            break;
        }
    case ESKILLTREENODE:
        m_skill_tree.push_back( dynamic_cast<SkillTreeNode*>(pElement));
        break;
    case EBATTLEMENU:
        m_pMenu = dynamic_cast<BattleMenu*>(pElement);
        break;
    default:
        return false;
    }
    return true;
}

/*
 * 
 * For now it's okay if they have a battle menu option that refers to a skill that the class doesn't even have..
 * but ideally it should be a warning */
/*
void CharacterClass::verify_menu_options ( BattleMenu* pMenu )
{
    for(std::vector<BattleMenuOption*>::iterator iter = pMenu->GetOptionsBegin();
        iter != pMenu->GetOptionsEnd(); iter++)
        {
            
        }
}
*/

void CharacterClass::load_finished()
{
    if(m_pMenu == NULL)
        throw CL_Exception("Missing battle menu on class " + m_name);
    if(m_skill_tree.empty())
        throw CL_Exception("No skill tree on class " + m_name);
}

double CharacterClass::GetStat(ICharacter::eCharacterAttribute attr, int level)
{
    StatMap::iterator it = m_stat_scripts.find(attr);
    if(it == m_stat_scripts.end())
        throw CL_Exception("Missing stat: " + ICharacter::CAToString(attr) +  " on character class: " + m_name );

    StatScript *pScript = it->second;
    return pScript->GetStat(level);
}

CharacterClass::CharacterClass()
:m_pMenu(NULL)
{

}

CharacterClass::~CharacterClass()
{
    delete m_pMenu;
    std::for_each(m_weapon_types.begin(),m_weapon_types.end(),del_fun<WeaponTypeRef>());
    std::for_each(m_armor_types.begin(),m_armor_types.end(),del_fun<ArmorTypeRef>());
    std::for_each(m_skill_tree.begin(),m_skill_tree.end(),del_fun<SkillTreeNode>());

    for(std::map<ICharacter::eCharacterAttribute,StatScript*>::iterator it = m_stat_scripts.begin();
        it != m_stat_scripts.end();
        it++)
    {
        delete it->second;
    }
}

std::list<WeaponTypeRef*>::const_iterator CharacterClass::GetWeaponTypeRefsBegin() const
{
    return m_weapon_types.begin();
}

std::list<WeaponTypeRef*>::const_iterator CharacterClass::GetWeaponTypeRefsEnd() const
{
    return m_weapon_types.end();
}

std::list<ArmorTypeRef*>::const_iterator CharacterClass::GetArmorTypeRefsBegin() const
{
    return m_armor_types.begin();
}

std::list<ArmorTypeRef*>::const_iterator CharacterClass::GetArmorTypeRefsEnd() const
{
    return m_armor_types.end();
}


std::list<SkillTreeNode*>::const_iterator CharacterClass::GetSkillTreeNodesBegin() const
{
    return m_skill_tree.begin();
}

std::list<SkillTreeNode*>::const_iterator CharacterClass::GetSkillTreeNodesEnd() const
{
    return m_skill_tree.end();
}

std::string CharacterClass::GetName() const
{
    return m_name;
}

bool CharacterClass::CanEquip ( WeaponType* pWeaponType ) const
{
    for(std::list<WeaponTypeRef*>::const_iterator iter = m_weapon_types.begin();
        iter != m_weapon_types.end(); iter++)
        {
            if(pWeaponType->GetName() == (*iter)->GetName())
                return true;
        }
        
        return false;
}

bool CharacterClass::CanEquip ( ArmorType* pArmorType ) const
{
    for(std::list<ArmorTypeRef*>::const_iterator iter = m_armor_types.begin();
        iter != m_armor_types.end(); iter++)
    {
        if(pArmorType->GetName() == (*iter)->GetName())
            return true;
    }
        
    return false;
}

bool CharacterClass::CanEquip ( Equipment* pEquipment ) const
{
    if(pEquipment->GetItemType() == Item::WEAPON) {
        Weapon * pWeapon = dynamic_cast<Weapon*>(pEquipment);
        return CanEquip(pWeapon->GetWeaponType());    
    }else {
        Armor * pArmor = dynamic_cast<Armor*>(pEquipment);
        return CanEquip(pArmor->GetArmorType());
    }
}


StoneRing::BattleMenu * StoneRing::CharacterClass::GetBattleMenu() const{
    return m_pMenu;
}

ICharacter::eGender
CharacterClass::GetGender() const
{
    return m_eGender;
}



void StatScript::load_attributes(CL_DomNamedNodeMap attributes)
{
    std::string stat = get_required_string("stat",attributes);
    m_eStat = ICharacter::CharAttributeFromString ( stat );
}

bool StatScript::handle_element(Element::eElement element, StoneRing::Element *pElement)
{
    if(element == ESCRIPT)
    {
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        return true;
    }

    return false;
}

void StatScript::load_finished()
{
    if(m_pScript == NULL) throw CL_Exception("No script defined for scriptElement");
}

StatScript::StatScript( )
{

}

StatScript::~StatScript()
{
}

ICharacter::eCharacterAttribute
StatScript::GetCharacterStat() const
{
    return m_eStat;
}

double StatScript::GetStat(int level)
{
    // Magic conversion to double
    ParameterList params;
    params.push_back ( ParameterListItem("$_CL",level) );

    return m_pScript->ExecuteScript(params);
}







