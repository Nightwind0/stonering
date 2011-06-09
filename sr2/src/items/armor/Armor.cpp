#include "Armor.h"
#include "ArmorEnhancer.h"
#include "RuneType.h"
#include "ArmorClass.h"
#include "ArmorType.h"
#include "SpellRef.h"
#include "AbilityManager.h"
#include "AttributeModifier.h"

using namespace StoneRing;

std::string StoneRing::Armor::CreateArmorName(ArmorType *pType, ArmorClass *pClass, SpellRef *pSpell, RuneType *pRune)
{
    std::ostringstream os;

    if ( pRune )
    {
        os << pRune->GetRuneTypeAsString() << ' ';
    }

    os << pClass->GetName() << ' ' << pType->GetName();

    if(pSpell)
    {
        os << " of " << pSpell->GetName();
    }

    return os.str();
}


Armor::eAttribute
Armor::AttributeForString ( const std::string str )
{
    if(str == "AC") return AC;
    else if(str == "RST") return RST;
    else if(str == "Change_BP") return CHANGE_BP;
    else if (str == "Steal_MP%") return STEAL_MP;
    else if (str == "Steal_HP%") return STEAL_HP;
    else if (str == "Status%") return STATUS;
    else throw CL_Exception("Bad Armor enhancer attribute.");
}


Armor::Armor()
{
}


Armor::~Armor()
{
    Clear_Armor_Enhancers();
}


void Armor::Set_Rune_Type ( RuneType* pType )
{
    Equipment::Set_Rune_Type(pType);
    
    // TODO: Create and apply attribute modifiers
}

void Armor::Set_Spell_Ref ( SpellRef* pRef )
{
    assert(pRef);
    Equipment::Set_Spell_Ref(pRef);
    Spell * pSpell = AbilityManager::GetSpell(*pRef);
    MagicResistance * pMagicResistance = pSpell->getMagicResistance();
    
    if(pMagicResistance)
    {
        AttributeModifier *pAM = new AttributeModifier();
        uint attribute = 0;
        // TODO: Handle when it applies to more than one magic type,
        // such as Magic::ALL, Magic::ELEMENTAL, Magic::DIVINE
        switch(pMagicResistance->GetType())
        {
            case Magic::DARK:
                attribute = ICharacter::CA_DARK_RST;
                break;
            case Magic::HOLY:
                attribute = ICharacter::CA_HOLY_RST;
                break;
            case Magic::FIRE:
                attribute = ICharacter::CA_FIRE_RST;
                break;
            case Magic::WATER:
                attribute = ICharacter::CA_WATER_RST;
                break;
            case Magic::WIND:
                attribute = ICharacter::CA_WIND_RST;
                break;
            case Magic::EARTH:
                attribute = ICharacter::CA_EARTH_RST;
                break;
        }
        pAM->SetAttribute(attribute);
        pAM->SetAdd(pMagicResistance->GetResistance());
        Add_Attribute_Modifier(pAM);
    }
}


double Armor::GetArmorAttribute ( eAttribute attr )
{

    double current = 0.0;
    switch(attr){
        case AC:
            current = GetArmorType()->GetBaseAC();
            break;
         case RST:
            current = GetArmorType()->GetBaseRST();
            break;
        default:
            break;
    }
    double  value = current;

    for(std::list<ArmorEnhancer*>::iterator iter = m_armor_enhancers.begin();
        iter != m_armor_enhancers.end();
        iter++)
    {
        ArmorEnhancer * pEnhancer = *iter;

        if( pEnhancer->GetAttribute() == attr )
        {
            value *= pEnhancer->GetMultiplier();
        }
    }
    for(std::list<ArmorEnhancer*>::iterator iter = m_armor_enhancers.begin();
        iter != m_armor_enhancers.end();
        iter++)
    {
        ArmorEnhancer * pEnhancer = *iter;

        if( pEnhancer->GetAttribute() == attr )
        {
            value += pEnhancer->GetAdd();
        }
    }


    return value;

}


/*
int Armor::GetResistanceAdd( DamageCategory::eDamageCategory category )
{
    int value = 0;
    
    for(std::list<ArmorEnhancer*>::iterator iter = m_armor_enhancers.begin();
        iter != m_armor_enhancers.end();
        iter++)
    {
        ArmorEnhancer * pEnhancer = *iter;

        if( pEnhancer->GetType() == ArmorEnhancer::DAMAGE_CATEGORY &&  pEnhancer->GetDamageCategory() == category )
        {
            value += pEnhancer->GetAdd();
        }
    }
    
    return value;
}
*/

void Armor::Clear_Armor_Enhancers()
{
    for(std::list<ArmorEnhancer*>::iterator iter = m_armor_enhancers.begin();
        iter != m_armor_enhancers.end();
        iter++)
    {
        delete *iter;
    }
    m_armor_enhancers.clear();
}

void Armor::Add_Armor_Enhancer (ArmorEnhancer * pEnhancer)
{
    m_armor_enhancers.push_back ( pEnhancer );
}



