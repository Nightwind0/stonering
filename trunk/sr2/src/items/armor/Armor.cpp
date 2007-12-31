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
        os << pRune->getRuneTypeAsString() << ' ';
    }

    os << pClass->getName() << ' ' << pType->getName();

    if(pSpell)
    {
        os << " of " << pSpell->getName();
    }

    return os.str();
}


Armor::eAttribute 
Armor::attributeForString ( const std::string str )
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
    clearArmorEnhancers();
}



int Armor::modifyArmorAttribute( eAttribute attr, int current )
{
    int value = current;

    for(std::list<ArmorEnhancer*>::iterator iter = mArmorEnhancers.begin();
        iter != mArmorEnhancers.end();
        iter++)
    {
        ArmorEnhancer * pEnhancer = *iter;

        if( pEnhancer->getAttribute() == attr )
        {
            value= (int)(pEnhancer->getMultiplier() * (float)value);
            value += pEnhancer->getAdd();
        }
    }

    return value;
}

float Armor::modifyArmorAttribute ( eAttribute attr, float current )
{

    float  value = current;

    for(std::list<ArmorEnhancer*>::iterator iter = mArmorEnhancers.begin();
        iter != mArmorEnhancers.end();
        iter++)
    {
        ArmorEnhancer * pEnhancer = *iter;

        if( pEnhancer->getAttribute() == attr )
        {
            value *= pEnhancer->getMultiplier();
            value += pEnhancer->getAdd();
        }
    }

    return value;

}



void Armor::clearArmorEnhancers()
{
    for(std::list<ArmorEnhancer*>::iterator iter = mArmorEnhancers.begin();
        iter != mArmorEnhancers.end();
        iter++)
    {
        delete *iter;
    }
    mArmorEnhancers.clear();
}

void Armor::addArmorEnhancer (ArmorEnhancer * pEnhancer)
{
    mArmorEnhancers.push_back ( pEnhancer );
}



