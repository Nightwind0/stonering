#include "StatusEffect.h"
#include "AbilityFactory.h"
#include "IApplication.h"
#include "Animation.h"

StoneRing::AttributeEffect::AttributeEffect(CL_DomElement * pElement)
{
	AbilityFactory * pFactory = IApplication::getInstance()->getAbilityFactory();
    
    CL_DomNamedNodeMap attributes = pElement->get_attributes();


	mAttribute = getRequiredString("attribute", &attributes);

	if( hasAttr("add", &attributes))
	{
		mnAdd = getInt("add",&attributes);
	}
	else mnAdd = 0;

	if (hasAttr("multiplier", &attributes))
	{
		mfMultiplier = getFloat("multiplier",&attributes);
	}
	else mfMultiplier = 1;

	std::string changeTo = getRequiredString("changeTo",&attributes);

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
	

StoneRing::StatusEffectActions::StatusEffectActions(CL_DomElement * pElement)
{
	AbilityFactory * pFactory = IApplication::getInstance()->getAbilityFactory();
    
    CL_DomNamedNodeMap attributes = pElement->get_attributes();

    CL_DomElement child = pElement->get_first_child().to_element();

	while(!child.is_null())
    {
	
		std::string name = child.get_node_name();
	
		if(name == "doWeaponDamage")
		{
			mEffects.push_back ( pFactory->createDoWeaponDamage ( &child ) );
		}
		else if ( name == "doMagicDamage")
		{
			mEffects.push_back ( pFactory->createDoMagicDamage ( &child ) );
		}
		else if ( name == "doStatusEffect")
		{
			mEffects.push_back ( pFactory->createDoStatusEffect ( &child ) );
		}
		else if	( name == "animation")
		{
			mEffects.push_back ( pFactory->createAnimation( &child ) );
		}
		else if ( name == "attributeEffect") 
		{
			mEffects.push_back ( pFactory->createAttributeEffect ( &child ) );
		}
		else throw CL_Error("Bad effect in StatusEffectAction: " + name);
		

		child = child.get_next_sibling().to_element();
    }

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


StoneRing::StatusEffect::StatusEffect(CL_DomElement * pElement):mpOnInvoke(NULL),mpOnRound(NULL),
mpOnCountdown(NULL), mpOnRemove(NULL)
{
	AbilityFactory * pFactory = IApplication::getInstance()->getAbilityFactory();
    
    CL_DomNamedNodeMap attributes = pElement->get_attributes();

	mName = getRequiredString("name", &attributes );
	std::string last = getRequiredString("last",&attributes );

	if(last == "battle") meLast = BATTLE;
	else if (last == "round_count") meLast = ROUND_COUNT;
	else if (last == "permanent") meLast = PERMANENT;

	if( meLast == BATTLE )
	{
		if(hasAttr("lengthMultiplier", &attributes ))
		{
			mfLengthMultiplier = getRequiredFloat("lengthMultiplier", &attributes);
		}
		else mfLengthMultiplier = 1;
	}
	else if (meLast == ROUND_COUNT )
	{
		mnRoundCount = getRequiredInt("roundCount",&attributes);
	}


    CL_DomElement child = pElement->get_first_child().to_element();

	while(!child.is_null())
	{
		std::string name = child.get_node_name();

		if(name == "onInvoke")
		{
			mpOnInvoke = pFactory->createStatusEffectActions(&child);
		}
		else if (name == "onRound")
		{
			mpOnRound = pFactory->createStatusEffectActions(&child);
		}
		else if (name == "onCountdown")
		{
			mpOnCountdown = pFactory->createStatusEffectActions(&child);
		}
		else if (name == "onRemove")
		{
			mpOnRemove = pFactory->createStatusEffectActions(&child);
		}


		child = child.get_next_sibling().to_element();
	}


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
				