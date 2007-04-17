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

CL_DomElement ArmorTypeExclusionList::createDomElement ( CL_DomDocument &doc) const
{
    return CL_DomElement(doc,"ArmorTypeExclusionList");
}


std::list<ArmorTypeRef*>::const_iterator ArmorTypeExclusionList::getArmorTypeRefsBegin()
{
    return mArmorTypes.begin();
}
std::list<ArmorTypeRef*>::const_iterator ArmorTypeExclusionList::getArmorTypeRefsEnd()
{
    return mArmorTypes.end();
}
bool ArmorTypeExclusionList::handleElement(eElement element, Element * pElement)
{
    if(element == EARMORTYPEREF)
    {
        mArmorTypes.push_back ( dynamic_cast<ArmorTypeRef*>(pElement) );
        return true;
    }
    else return false;
}




