#include "CharacterDefinition.h"
#include "AbilityManager.h"
#include "IApplication.h"
#include "Animation.h"

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
	"spr",
	"null",
	"encounterRate",
	"goldDropRate",
	"itemDropRate",
	"priceMultiplier"

};



StoneRing::eCharacterAttribute StoneRing::CharAttributeFromString(const std::string &str)
{

	uint numberStats = _LAST_CHARACTER_ATTR_;
	
	for(uint i =0; i < numberStats; i++)
	{
		if( str == statXMLLookup[i] )
		{
			return (eCharacterAttribute)i;
		}
	}

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

void StoneRing::CharacterDefinition::handleElement(eElement element, StoneRing::Element * pElement )
{
	switch(element)
	{
	case EANIMATIONDEFINITION:
		mAnimationDefinitions.push_back(dynamic_cast<AnimationDefinition*>(pElement));
		break;
	case EWEAPONTYPESPRITE:
		mWeaponTypeSprites.push_back(dynamic_cast<WeaponTypeSprite*>(pElement));
		break;
	}
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

void StoneRing::AnimationDefinition::handleElement(eElement element, Element * pElement)
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
		throw CL_Error("Unexpected element found in animation definition.");
	}
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


void StoneRing::WeaponTypeSprite::handleElement(eElement element, Element *pElement )
{
	if(element == EWEAPONTYPEREF) mpWeaponTypeRef = dynamic_cast<WeaponTypeRef*>(pElement);
}

void StoneRing::WeaponTypeSprite::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
	mSpriteRef = getRequiredString("spriteRef",pAttributes);
}

