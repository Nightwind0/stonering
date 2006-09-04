#include "ArmorClass.h"
#include "AttributeEnhancer.h"
#include "ArmorEnhancer.h"
#include "ArmorTypeExclusionList.h"
#include "StatusEffectModifier.h"


using namespace StoneRing;

ArmorClass::ArmorClass()
{
}


bool ArmorClass::operator==( const ArmorClass &lhs )
{
	return mName == lhs.mName;
}

void ArmorClass::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
	mName = getRequiredString("name",pAttributes );
	mfValueMultiplier = getImpliedFloat("valueMultiplier",pAttributes,1);
	mnValueAdd = getImpliedInt("valueAdd",pAttributes,0);
}

bool ArmorClass::handleElement(eElement element, Element * pElement)
{
	switch(element)
	{
	case EATTRIBUTEENHANCER:
		mAttributeEnhancers.push_back( dynamic_cast<AttributeEnhancer*>(pElement) );
		break;
	case EARMORENHANCER:
		mArmorEnhancers.push_back( dynamic_cast<ArmorEnhancer*>(pElement) );
		break;
	case EARMORTYPEEXCLUSIONLIST:
		{
			ArmorTypeExclusionList * pList = dynamic_cast<ArmorTypeExclusionList*>(pElement);
			std::copy(pList->getArmorTypeRefsBegin(),pList->getArmorTypeRefsEnd(), 
				std::back_inserter(mExcludedTypes));

			delete pList;
			break;
		}
	case ESTATUSEFFECTMODIFIER:
		addStatusEffectModifier (dynamic_cast<StatusEffectModifier*>(pElement));
		break;
	default:
		return false;
	}

	return true;
}


ArmorClass::~ArmorClass()
{
	std::for_each(mAttributeEnhancers.begin(),mAttributeEnhancers.end(),del_fun<AttributeEnhancer>());
	std::for_each(mArmorEnhancers.begin(),mArmorEnhancers.end(),del_fun<ArmorEnhancer>());
	std::for_each(mExcludedTypes.begin(),mExcludedTypes.end(),del_fun<ArmorTypeRef>());
}


CL_DomElement ArmorClass::createDomElement ( CL_DomDocument &doc) const
{
	return CL_DomElement(doc, "armorClass");
}

std::string ArmorClass::getName() const
{
	return mName;
}

int ArmorClass::getValueAdd() const
{
	return mnValueAdd;
}

float ArmorClass::getValueMultiplier() const
{
	return mfValueMultiplier;
}

std::list<AttributeEnhancer*>::const_iterator 
ArmorClass::getAttributeEnhancersBegin()
{
	return mAttributeEnhancers.begin();
}

std::list<AttributeEnhancer*>::const_iterator 
ArmorClass::getAttributeEnhancersEnd()
{
	return mAttributeEnhancers.end();
}

std::list<ArmorEnhancer*>::const_iterator 
ArmorClass::getArmorEnhancersBegin()
{
	return mArmorEnhancers.begin();
}

std::list<ArmorEnhancer*>::const_iterator 
ArmorClass::getArmorEnhancersEnd()
{
	return mArmorEnhancers.end();
}

bool ArmorClass::isExcluded ( const ArmorTypeRef &armorType )
{
	for(std::list<ArmorTypeRef*>::const_iterator iter = mExcludedTypes.begin();
		iter != mExcludedTypes.end();
		iter++)
	{
		ArmorTypeRef * pRef = *iter;

		if( pRef->getName() == armorType.getName() ) 
			return true;
	}

	return false;
}
