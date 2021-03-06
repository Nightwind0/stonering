#include "Armor.h"
#include "ArmorEnhancer.h"
#include "RuneType.h"
#include "ArmorClass.h"
#include "ArmorType.h"
#include "AbilityManager.h"
#include "AttributeModifier.h"

using namespace StoneRing;


Equipment::eSlot Armor::GetSlot() const
{
    return GetArmorType()->GetSlot();
}

std::string Armor::StringForAttribute ( Armor::eAttribute attr )
{
    switch(attr){
        case AC:
            return "A.Ac";
        case RST:
            return "A.Rst";
        default:
            assert(0);
            return "";
    }
}


bool Armor::AttributeIsInteger( Armor::eAttribute attr ) {
	return attr == Armor::AC || attr == Armor::RST;
}


std::string StoneRing::Armor::CreateArmorName(ArmorType *pType, ArmorClass *pClass, ArmorClass* pImbuement, RuneType *pRune)
{
    std::ostringstream os;

    if ( pRune )
    {
        os << pRune->GetRuneTypeAsString() << ' ';
    }

    os << pClass->GetName() << ' ' << pType->GetName();

    if(pImbuement)
    {
        os << ' ' << pImbuement->GetName();
    }

    return os.str();
}


Armor::eAttribute
Armor::AttributeForString ( const std::string str )
{
    if(str == "AC") return AC;
    else if(str == "RST") return RST;
    else throw XMLException("Bad Armor enhancer attribute.");
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
    m_armor_enhancers.clear();
}

void Armor::Add_Armor_Enhancer (ArmorEnhancer * pEnhancer)
{
    m_armor_enhancers.push_back ( pEnhancer );
}



