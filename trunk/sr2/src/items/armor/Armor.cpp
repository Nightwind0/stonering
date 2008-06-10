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

    else if (str == "Steal_MP%") return STEAL_MP;
    else if (str == "Steal_HP%") return STEAL_HP;

    else if (str == "ElementalRST") return ELEMENTAL_RESIST;
    else if (str == "RST") return RESIST; // Resist is basically Magic AC
    else if (str == "Status%") return STATUS;
    else if (str == "SlashAC") return SLASH_AC; // Extra AC against slash attacks
    else if (str == "JabAC") return JAB_AC;
    else if (str == "BashAC") return BASH_AC;
    else if (str == "WhiteRST") return WHITE_RESIST;
    else throw CL_Error("Bad Armor enhancer attribute.");
}


Armor::Armor()
{
}


Armor::~Armor()
{
    Clear_Armor_Enhancers();
}



int Armor::ModifyArmorAttribute( eAttribute attr, int current )
{
    int value = current;

    for(std::list<ArmorEnhancer*>::iterator iter = m_armor_enhancers.begin();
        iter != m_armor_enhancers.end();
        iter++)
    {
        ArmorEnhancer * pEnhancer = *iter;

        if( pEnhancer->GetAttribute() == attr )
        {
            value= (int)(pEnhancer->GetMultiplier() * (float)value);
            value += pEnhancer->GetAdd();
        }
    }

    return value;
}

float Armor::ModifyArmorAttribute ( eAttribute attr, float current )
{

    float  value = current;

    for(std::list<ArmorEnhancer*>::iterator iter = m_armor_enhancers.begin();
        iter != m_armor_enhancers.end();
        iter++)
    {
        ArmorEnhancer * pEnhancer = *iter;

        if( pEnhancer->GetAttribute() == attr )
        {
            value *= pEnhancer->GetMultiplier();
            value += pEnhancer->GetAdd();
        }
    }

    return value;

}



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



