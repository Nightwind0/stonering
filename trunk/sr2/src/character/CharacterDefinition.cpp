#include <cassert>
#include "CharacterDefinition.h"
#include "AbilityManager.h"
#include "IApplication.h"
#include "Animation.h"
#include "WeaponTypeRef.h"

namespace StoneRing{
struct stat_entry
{
    const char * string;
    uint attr;
};

const stat_entry statXMLLookup[] = 
{
    {"hp",CA_HP},
    {"hp_max",CA_MAXHP},
    {"mp",CA_MP},
    {"mp_max",CA_MAXMP},
    {"str",CA_STR},
    {"def",CA_DEF},
    {"dex",CA_DEX},
    {"evd",CA_EVD},
    {"mag",CA_MAG},
    {"rst",CA_RST},
    {"lck",CA_LCK},
    {"joy",CA_JOY},
    {"draw_ill",CA_DRAW_ILL},   //    CA_DRAW_ILL,
    {"draw_stone",CA_DRAW_STONE}, //    CA_DRAW_STONE,
    {"draw_berserk",CA_DRAW_BERSERK}, //  CA_DRAW_BERSERK,
    {"draw_weak",CA_DRAW_WEAK}, // CA_DRAW_WEAK,
    {"draw_paralyzed",CA_DRAW_PARALYZED}, //    CA_DRAW_PARALYZED,
    {"draw_translucent",CA_DRAW_TRANSLUCENT}, 
    {"can_act", CA_CAN_ACT}, //   CA_CAN_ACT,
    {"can_fight",CA_CAN_FIGHT}, // CA_CAN_FIGHT,
    {"can_cast", CA_CAN_CAST}, //  CA_CAN_CAST,
    {"can_skill",CA_CAN_SKILL}, // CA_CAN_SKILL,
    {"can_item", CA_CAN_ITEM},
    {"can_run", CA_CAN_RUN}, //   CA_CAN_RUN,
    {"alive", CA_ALIVE},
    {"encounterRate", CA_ENCOUNTER_RATE},
    {"goldDropRate", CA_GOLD_DROP_RATE},
    {"itemDropRate", CA_ITEM_DROP_RATE},
    {"priceMultiplier", CA_PRICE_MULTIPLIER},
    {"expMultiplier", CA_EXP_MULTIPLIER},
    {"level",CA_LEVEL},
    {"idol_slots",CA_IDOL_SLOTS}
};

}

StoneRing::eCharacterAttribute StoneRing::CharAttributeFromString(const std::string &str)
{
    uint numberStats = _LAST_CHARACTER_ATTR_;
    
    for(uint i =0; i < numberStats; i++)
    {
        if( str == statXMLLookup[i].string )
        {
            return static_cast<eCharacterAttribute>(statXMLLookup[i].attr);
        }
    }
    return static_cast<StoneRing::eCharacterAttribute>(CA_INVALID);
}

StoneRing::eCommonAttribute StoneRing::CommonAttributeFromString(const std::string &str)
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


uint StoneRing::CAFromString(const std::string &str)
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

std::string StoneRing::CAToString(uint v)
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



StoneRing::CharacterDefinition::CharacterDefinition(void)
{
}

StoneRing::CharacterDefinition::~CharacterDefinition(void)
{
}


bool StoneRing::CharacterDefinition::handleElement(eElement element, StoneRing::Element * pElement )
{
    switch(element)
    {
    case ESPRITEDEFINITION:
        mSpriteDefinitions.push_back(dynamic_cast<SpriteDefinition*>(pElement));
        break;
    default:
        return false;
    }

    return true;
}

void StoneRing::CharacterDefinition::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{

    const AbilityManager * pAbilityManager = IApplication::getInstance()->getAbilityManager();
    mName = getRequiredString("name",pAttributes);
    mSpriteRef = getRequiredString("spriteResource",pAttributes);
    std::string className = getRequiredString("class",pAttributes);

    // Get the class pointer
    mpClass = pAbilityManager->getClass(className);

}



StoneRing::SpriteDefinition::SpriteDefinition():mpSpriteRef(NULL),mbHasBindPoints(false)
{
}

StoneRing::SpriteDefinition::~SpriteDefinition()
{
}



bool StoneRing::SpriteDefinition::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESPRITEREF:
        if(mpSpriteRef) throw CL_Error("Sprite Ref already defined for Sprite Definition");
        mpSpriteRef = dynamic_cast<SpriteRef*>(pElement);
    default:
        return false;
    }

    return true;
}

void StoneRing::SpriteDefinition::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mName = getRequiredString("name",pAttributes);

    if(hasAttr("bindPoint1",pAttributes))
    {
        mbHasBindPoints = true;
        mnBindPoint1 = getRequiredInt("bindPoint1",pAttributes);
        mnBindPoint2 = getRequiredInt("bindPoint2",pAttributes);
    }

}

