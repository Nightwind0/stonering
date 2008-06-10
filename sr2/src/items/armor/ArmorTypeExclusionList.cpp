#include "ArmorTypeExclusionList.h"
#include "ArmorTypeRef.h"

using namespace StoneRing;

ArmorTypeExclusionList::ArmorTypeExclusionList()
{
}

ArmorTypeExclusionList::~ArmorTypeExclusionList()
{
    // Dont delete. The armor class does that.
    //  std::for_each(mArmorTypes.begin(),mArmorTypes.end(),del_fun<ArmorTypeRef>());
}

std::list<ArmorTypeRef*>::const_iterator ArmorTypeExclusionList::GetArmorTypeRefsBegin()
{
    return m_ArmorTypes.begin();
}
std::list<ArmorTypeRef*>::const_iterator ArmorTypeExclusionList::GetArmorTypeRefsEnd()
{
    return m_ArmorTypes.end();
}
bool ArmorTypeExclusionList::handle_element(eElement element, Element * pElement)
{
    if(element == EARMORTYPEREF)
    {
        m_ArmorTypes.push_back ( dynamic_cast<ArmorTypeRef*>(pElement) );
        return true;
    }
    else return false;
}




