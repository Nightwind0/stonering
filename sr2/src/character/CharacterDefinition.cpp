#include <cassert>
#include "CharacterDefinition.h"
#include "AbilityManager.h"
#include "IApplication.h"
#include "Animation.h"
#include "WeaponTypeRef.h"

const char *statXMLLookup[] = 
{
    "hp",
    "hp_max",
    "mp",
    "mp_max",
    "str",
    "dex",
    "evd",
    "mag",
    "rst",
    "lck",
    "joy",
    "draw_ill",   //    CA_DRAW_ILL,
    "draw_stone", //    CA_DRAW_STONE,
    "draw_berserk", //  CA_DRAW_BERSERK,
    "draw_weak", // CA_DRAW_WEAK,
    "draw_paralyzed", //    CA_DRAW_PARALYZED,
    "draw_translucent", 
    "can_act", //   CA_CAN_ACT,
    "can_fight", // CA_CAN_FIGHT,
    "can_cast", //  CA_CAN_CAST,
    "can_skill", // CA_CAN_SKILL,
    "can_item", 
    "can_run", //   CA_CAN_RUN,
    "alive", 
    "encounterRate",
    "goldDropRate",
    "itemDropRate",
    "priceMultiplier",
    "expMultiplier"

};



StoneRing::eCharacterAttribute StoneRing::CharAttributeFromString(const std::string &str)
{
    // Lower
    uint numberStats = _LAST_CHARACTER_ATTR_;
    
    for(uint i =0; i < numberStats; i++)
    {
        if( str == statXMLLookup[i] )
        {
            return (eCharacterAttribute)i;
        }
    }
    assert ( 0 );
    return static_cast<StoneRing::eCharacterAttribute>(0);
}

StoneRing::eCommonAttribute StoneRing::CommonAttributeFromString(const std::string &str)
{
    for(int i = _LAST_CHARACTER_ATTR_; i < _LAST_COMMON_ATTR_; i++)
    {
        if(str == statXMLLookup[i])
        {
            return (eCommonAttribute)i;
        }
    }

    assert ( 0 );
    return static_cast<eCommonAttribute>(0);
}


uint StoneRing::CAFromString(const std::string &str)
{

    for(int i =0; i < _LAST_COMMON_ATTR_; i++)
    {
        if(str == statXMLLookup[i])
        {
            return i;
        }
    }

   // assert ( 0 );
    return 0;
}

std::string StoneRing::CAToString(uint i)
{

    return statXMLLookup[i];
}



StoneRing::CharacterDefinition::CharacterDefinition(void)
{
}

StoneRing::CharacterDefinition::~CharacterDefinition(void)
{
}


CL_DomElement  StoneRing::CharacterDefinition::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"character");
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

CL_DomElement  StoneRing::AnimationDefinition::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"animationDefinition");
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


CL_DomElement StoneRing::WeaponTypeSprite::createDomElement(CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"weaponTypeSprite");
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



