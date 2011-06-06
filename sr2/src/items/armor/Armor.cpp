#include "Armor.h"
#include "ArmorEnhancer.h"
#include "RuneType.h"
#include "ArmorClass.h"
#include "ArmorType.h"
#include "SpellRef.h"

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



