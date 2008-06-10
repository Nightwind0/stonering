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

std::list<WeaponTypeRef*>::const_iterator WeaponTypeExclusionList::GetWeaponTypeRefsBegin()
{
    return m_WeaponTypes.begin();
}
std::list<WeaponTypeRef*>::const_iterator WeaponTypeExclusionList::GetWeaponTypeRefsEnd()
{
    return m_WeaponTypes.end();
}
bool WeaponTypeExclusionList::handle_element(eElement element, Element * pElement)
{
    if(element == EWEAPONTYPEREF)
    {
        m_WeaponTypes.push_back ( dynamic_cast<WeaponTypeRef*>(pElement) );
        return true;
    }
    else return false;
}




