#include "WeaponTypeExclusionList.h"
#include "WeaponTypeRef.h"

using namespace StoneRing;

WeaponTypeExclusionList::WeaponTypeExclusionList()
{
}

WeaponTypeExclusionList::~WeaponTypeExclusionList()
{
	// std::for_each(mWeaponTypes.begin(),mWeaponTypes.end(),del_fun<WeaponTypeRef>());
}

CL_DomElement WeaponTypeExclusionList::createDomElement ( CL_DomDocument &doc) const
{
	return CL_DomElement(doc,"weaponTypeExclusionList");
}


std::list<WeaponTypeRef*>::const_iterator WeaponTypeExclusionList::getWeaponTypeRefsBegin()
{
	return mWeaponTypes.begin();
}
std::list<WeaponTypeRef*>::const_iterator WeaponTypeExclusionList::getWeaponTypeRefsEnd()
{
	return mWeaponTypes.end();
}
bool WeaponTypeExclusionList::handleElement(eElement element, Element * pElement)
{
	if(element == EWEAPONTYPEREF)
	{
		mWeaponTypes.push_back ( dynamic_cast<WeaponTypeRef*>(pElement) );
		return true;
	}
	else return false;
}
