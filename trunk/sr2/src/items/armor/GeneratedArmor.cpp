#include "GeneratedArmor.h"
#include "ArmorType.h"
#include "ArmorRef.h"
#include "RuneType.h"
#include "ArmorClass.h"
#include "SpellRef.h"
#include "AbilityManager.h"
#include "IApplication.h"

using namespace StoneRing;

GeneratedArmor::GeneratedArmor():mpType(NULL),mpClass(NULL)
{
}

GeneratedArmor::~GeneratedArmor()
{
}

std::string GeneratedArmor::getIconRef() const
{
	return mpType->getIconRef();
}

std::string GeneratedArmor::getName() const
{
	return mName;
}

uint GeneratedArmor::getMaxInventory() const 
{
	// todo: lookup in settings
	return 99;
}


bool GeneratedArmor::operator== ( const ItemRef &ref )
{
	if( ref.getType() == ItemRef::ARMOR_REF
		&& *ref.getArmorRef()->getArmorClass() == *mpClass &&
		*ref.getArmorRef()->getArmorType() == *mpType)
	{
		if(hasSpell() && ref.getArmorRef()->getSpellRef())
		{
			if(*getSpellRef() == *ref.getArmorRef()->getSpellRef())
			{
				return true;
			} 
			else return false;
		}
		else if ( hasSpell() || ref.getArmorRef()->getSpellRef())
		{
			// One had a spell ref and one didn't.
			return false;
		}

		if(hasRuneType() && ref.getArmorRef()->getRuneType())
		{
			if(*getRuneType() == *ref.getArmorRef()->getRuneType())
			{
				return true;
			}
			else return false;
		}
		else if ( hasRuneType() || ref.getArmorRef()->getRuneType())
		{
			return false;
		}

		return true;
	}

	return false;
}


Item::eDropRarity GeneratedArmor::getDropRarity() const
{
	if( hasSpell() || hasRuneType() )
	{
		return RARE; 
	}
	else return UNCOMMON;
}


uint GeneratedArmor::getValue() const 
{
	// @todo: add rune value
	const AbilityManager * pManager = IApplication::getInstance()->getAbilityManager();

	uint value =  (int)((float)mpType->getBasePrice() * mpClass->getValueMultiplier()) 
		+ mpClass->getValueAdd();

	if(hasSpell())
	{
		SpellRef * pSpellRef = getSpellRef();

		Spell * pSpell = pManager->getSpell ( *pSpellRef );

		value += pSpell->getValue();
	}

	if(hasRuneType())
	{
		switch( getRuneType()->getRuneType() )
		{
		case RuneType::RUNE:
			value *= 2; //@todo : get value from game settings
			break;
		case RuneType::ULTRA_RUNE:
			{
				double dValue = value;
				dValue *= 2.75; //@todo: get value from game settings
				value = (int)dValue;
				break;
			}
		}
	}

	return value;
}

uint GeneratedArmor::getSellValue() const 
{
	return getValue() / 2;
}



ArmorType * GeneratedArmor::getArmorType() const 
{
	return mpType;
}

ArmorRef GeneratedArmor::generateArmorRef() const
{

	return ArmorRef ( getArmorType(), getArmorClass(), getSpellRef(), getRuneType() );
}

void GeneratedArmor::generate( ArmorType * pType, ArmorClass * pClass, 
							  SpellRef *pSpell , RuneType *pRune)
{

	for(std::list<AttributeEnhancer*>::const_iterator iter = pClass->getAttributeEnhancersBegin();
		iter != pClass->getAttributeEnhancersEnd();
		iter++)
	{
		addAttributeEnhancer  ( *iter );
	}
	for(std::list<ArmorEnhancer*>::const_iterator iter2 = pClass->getArmorEnhancersBegin();
		iter2 != pClass->getArmorEnhancersEnd();
		iter2++)
	{
		addArmorEnhancer ( *iter2 );
	}

	std::ostringstream os;

	mpType = pType;
	mpClass = pClass;

	if(pSpell)
	{
		setSpellRef ( pSpell );
	}
	else if ( pRune )
	{
		setRuneType ( pRune );

		os << pRune->getRuneTypeAsString() << ' ';
	}

	os << pClass->getName() << ' ';

	os << pType->getName();

	if(pSpell)
	{
		os << " of " <<  pSpell->getName();
	}




	mName = os.str();
}


