#include "StatusEffect.h"
#include "AbilityFactory.h"
#include "IApplication.h"
#include "Animation.h"


StoneRing::AttributeEffect::AttributeEffect()
{
}


void StoneRing::AttributeEffect::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
	mnAdd = getImpliedInt("add",pAttributes,0);
	mfMultiplier = getImpliedFloat("multiplier",pAttributes,1);

	std::string changeTo = getRequiredString("changeTo",pAttributes);

	if(changeTo == "min")
		meChangeTo = MIN;
	else if(changeTo == "max")
		meChangeTo = MAX;
	else if(changeTo == "add")
		meChangeTo = ADD;
	else if(changeTo == "multiplier")
		meChangeTo = MULTIPLIER;
	else if(changeTo == "multiplier_add")
		meChangeTo = MULTIPLIER_ADD;
	else throw CL_Error("Bad changeto on attribute effect: " + changeTo);
}

StoneRing::AttributeEffect::~AttributeEffect()
{
}

CL_DomElement StoneRing::AttributeEffect::createDomElement(CL_DomDocument &doc) const 
{
	return CL_DomElement(doc,"attributeEffect");
}


std::string StoneRing::AttributeEffect::getAttribute() const
{
	return mAttribute;
}

int StoneRing::AttributeEffect::getAdd() const
{
	return mnAdd;
}

float StoneRing::AttributeEffect::getMultiplier() const
{
	return mfMultiplier;
}


StoneRing::AttributeEffect::eChangeTo 
StoneRing::AttributeEffect::getChangeTo() const
{
	return meChangeTo;
}

StoneRing::StatusEffectActions::StatusEffectActions()
{
}


bool StoneRing::StatusEffectActions::handleElement(eElement element,Element * pElement)
{
	switch(element)
	{
	case EDOWEAPONDAMAGE:
	case EDOMAGICDAMAGE:
	case EDOSTATUSEFFECT:
	case EANIMATION:
	case EATTRIBUTEEFFECT:
		mEffects.push_back( dynamic_cast<Effect*>(pElement) );
		break;
	default:
		return false;
	}
	return true;
}
	


StoneRing::StatusEffectActions::~StatusEffectActions()
{
	for(std::list<Effect*>::iterator i = mEffects.begin();
		i != mEffects.end();
		i++)
		{
			delete *i;
		}
}

CL_DomElement StoneRing::StatusEffectActions::createDomElement(CL_DomDocument &doc) const
{
	return CL_DomElement(doc,"statusEffectActions");
}

std::list<StoneRing::Effect*>::const_iterator StoneRing::StatusEffectActions::getEffectsBegin() const
{
	return mEffects.begin();
}

std::list<StoneRing::Effect*>::const_iterator StoneRing::StatusEffectActions::getEffectsEnd() const
{
	return mEffects.end();
}


void StoneRing::StatusEffect::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
	mName = getRequiredString("name", pAttributes );
	std::string last = getRequiredString("last",pAttributes );

	if(last == "battle") meLast = BATTLE;
	else if (last == "round_count") meLast = ROUND_COUNT;
	else if (last == "permanent") meLast = PERMANENT;

	if( meLast == BATTLE )
	{
		mfLengthMultiplier = getImpliedFloat("lengthMultiplier",pAttributes,1);
	}
	else if (meLast == ROUND_COUNT )
	{
		mnRoundCount = getRequiredInt("roundCount",pAttributes);
	}

}

bool StoneRing::StatusEffect::handleElement(eElement element, Element * pElement)
{
		switch(element)
		{
		case EONINVOKE:
			mpOnInvoke = dynamic_cast<StatusEffectActions*>(pElement);
			break;
		case EONROUND:
			mpOnRound = dynamic_cast<StatusEffectActions*>(pElement);
			break;
		case EONCOUNTDOWN:
			mpOnCountdown = dynamic_cast<StatusEffectActions*>(pElement);
			break;
		case EONREMOVE:
			mpOnRemove = dynamic_cast<StatusEffectActions*>(pElement);
			break;
		default:
			return false;
		}
		return true;
}

StoneRing::StatusEffect::StatusEffect():mpOnInvoke(NULL),mpOnRound(NULL),
mpOnCountdown(NULL), mpOnRemove(NULL)
{
}

StoneRing::StatusEffect::~StatusEffect()
{
	delete mpOnInvoke;
	delete mpOnRound;
	delete mpOnCountdown;
	delete mpOnRemove;
}


CL_DomElement StoneRing::StatusEffect::createDomElement(CL_DomDocument &doc) const
{
	return CL_DomElement(doc,"statusEffect");
}

StoneRing::StatusEffectActions * StoneRing::StatusEffect::getOnInvoke() const
{
	return mpOnInvoke;
}

StoneRing::StatusEffectActions * StoneRing::StatusEffect::getOnRound() const
{
	return mpOnRound;
}

StoneRing::StatusEffectActions * StoneRing::StatusEffect::getOnCountdown() const
{
	return mpOnCountdown;
}

StoneRing::StatusEffectActions * StoneRing::StatusEffect::getOnRemove() const
{
	return mpOnRemove;
}

std::string StoneRing::StatusEffect::getName() const
{
	return mName;
}

StoneRing::StatusEffect::eLast 
StoneRing::StatusEffect::getLast() const
{
	return meLast;
}

uint StoneRing::StatusEffect::getRoundCount() const
{
	return mnRoundCount;
}


// Multiply the magic power of the user by this using an algorithm to get length..
float StoneRing::StatusEffect::getLengthMultiplier() const
{
	return mfLengthMultiplier;
}
				

