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
    case EANIMATIONDEFINITION:
        mAnimationDefinitions.push_back(dynamic_cast<AnimationDefinition*>(pElement));
        break;
    case EWEAPONTYPESPRITE:
        mWeaponTypeSprites.push_back(dynamic_cast<WeaponTypeSprite*>(pElement));
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



StoneRing::AnimationDefinition::AnimationDefinition():mpSkillRef(NULL),mpAnimation(NULL)
{
}

StoneRing::AnimationDefinition::~AnimationDefinition()
{
}


StoneRing::SkillRef * 
StoneRing::AnimationDefinition::getSkillRef() const
{
    return mpSkillRef;
}

StoneRing::Animation * 
StoneRing::AnimationDefinition::getAnimation() const
{
    return mpAnimation;
}

bool StoneRing::AnimationDefinition::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESKILLREF:
        if(mpSkillRef) throw CL_Error("More than one skill ref specified for animation definition");
        mpSkillRef = dynamic_cast<SkillRef*>(pElement);
        break;
    case EANIMATION:
        if(mpSkillRef) throw CL_Error("More than one animation specified for animation definition");
        mpAnimation = dynamic_cast<Animation*>(pElement);
        break;
    default:
        return false;
    }

    return true;
}

void StoneRing::AnimationDefinition::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mAnimationRef = getImpliedString("animationRef",pAttributes,"");
}


StoneRing::WeaponTypeSprite::WeaponTypeSprite():mpWeaponTypeRef(NULL)
{
}

StoneRing::WeaponTypeSprite::~WeaponTypeSprite()
{
}


StoneRing::WeaponTypeRef * StoneRing::WeaponTypeSprite::getWeaponTypeRef() const
{
    return mpWeaponTypeRef;
}


std::string StoneRing::WeaponTypeSprite::getSpriteRef() const
{
    return mSpriteRef;
}


bool StoneRing::WeaponTypeSprite::handleElement(eElement element, Element *pElement )
{
    if(element == EWEAPONTYPEREF)
    {
        mpWeaponTypeRef = dynamic_cast<WeaponTypeRef*>(pElement);
        return true;
    }
    else
    {
        return false;
    }
}

void StoneRing::WeaponTypeSprite::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mSpriteRef = getRequiredString("spriteRef",pAttributes);
}





