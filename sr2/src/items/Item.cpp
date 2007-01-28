#include <string>
#include <sstream>
#include "Item.h"
#include "ItemManager.h"
#include "ItemFactory.h"
#include "CharacterDefinition.h"


using namespace StoneRing;

// Defined in Level.cpp. No need to include the entire header.

bool   StoneRing::operator < ( const StoneRing::Item &lhs, const StoneRing::Item &rhs )
{
    return std::string(Item::ItemTypeAsString(lhs.getItemType()) + lhs.getName())
        <
        std::string(Item::ItemTypeAsString(rhs.getItemType()) + rhs.getName());
}


std::string StoneRing::Item::ItemTypeAsString ( StoneRing::Item::eItemType type )
{

    switch(type )
    {
        //enum eItemType { ITEM, WEAPON, ARMOR, RUNE, SPECIAL, SYSTEM };
    case Item::REGULAR_ITEM:
        return "item";
    case Item::WEAPON:
        return "weapon";
    case Item::ARMOR:
        return "armor";
    case Item::RUNE:
        return "rune";
    case Item::SPECIAL:
        return "special";
    case Item::SYSTEM:
        return "system";

    }

    return "";
}


Item::eDropRarity 
Item::DropRarityFromString(const std::string &str)
{
    eDropRarity eRarity = NEVER;

    if(str == "never") eRarity = NEVER;
    else if (str == "common") eRarity = COMMON;
    else if (str == "uncommon") eRarity = UNCOMMON;
    else if (str == "rare") eRarity = RARE;

    return eRarity;
}



Item::Item()
{
}

Item::~Item()
{
}







/*
  AC,
  POISON,
  STONE,
  DEATH,
  CONFUSE,
  BERSERK,
  SLOW,
  WEAK,
  BREAK, 
  SILENCE,
  SLEEP,
  BLIND,
  STEAL_MP,
  STEAL_HP,
  DROPSTR,
  DROPDEX,
  DROPMAG,
  ELEMENTAL_RESIST,
  RESIST, // All magic
  STATUS // Resistance against ANY status affect
*/


