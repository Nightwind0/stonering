#include "UniqueArmor.h"
#include "ArmorType.h"
#include "ItemManager.h"
#include "ArmorTypeRef.h"
#include "ArmorEnhancer.h"
#include "AttributeEnhancer.h"
#include "SpellRef.h"
#include "RuneType.h"
#include "StatusEffectModifier.h"

using namespace StoneRing;

UniqueArmor::UniqueArmor():mpArmorType(NULL)
{

}
UniqueArmor::~UniqueArmor()
{

}


uint UniqueArmor::getValue() const
{
	return mnValue;
}

uint UniqueArmor::getSellValue() const 
{
	return mnValue / 2;
}

ArmorType *UniqueArmor::getArmorType() const
{
	return mpArmorType;
}

CL_DomElement  
UniqueArmor::createDomElement(CL_DomDocument &doc) const
{
	return CL_DomElement(doc,"uniqueArmor");
}

void UniqueArmor::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
	mValueMultiplier = getImpliedFloat("valueMultiplier",pAttributes,1);

}

void UniqueArmor::loadFinished()
{
	cl_assert ( mpArmorType );
	mnValue = (int)(mpArmorType->getBasePrice() * mValueMultiplier);
}

bool UniqueArmor::handleElement(eElement element, Element * pElement)
{
	switch(element)
	{
	case EARMORTYPEREF:
    {
        const ItemManager * pItemManager = IApplication::getInstance()->getItemManager();
        ArmorTypeRef * pType = dynamic_cast<ArmorTypeRef*>(pElement);
        mpArmorType = pItemManager->getArmorType( *pType );
        break;
    }
	case EARMORENHANCER:
		addArmorEnhancer( dynamic_cast<ArmorEnhancer*>(pElement) );
		break;
	case EATTRIBUTEENHANCER:
		addAttributeEnhancer( dynamic_cast<AttributeEnhancer*>(pElement) );
		break;
	case ESPELLREF:
		setSpellRef ( dynamic_cast<SpellRef*>(pElement) );
		break;
	case ERUNETYPE:
		setRuneType( dynamic_cast<RuneType*>(pElement) );
		break;
	case ESTATUSEFFECTMODIFIER:
		addStatusEffectModifier( dynamic_cast<StatusEffectModifier*>(pElement) );
		break;
	default:
		return false;

	}

	return true;
}


