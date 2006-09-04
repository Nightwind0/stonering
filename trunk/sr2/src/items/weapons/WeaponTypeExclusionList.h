#ifndef SR_WEAPON_TYPE_EXCLUSION_LIST_H
#define SR_WEAPON_TYPE_EXCLUSION_LIST_H

#include "Element.h"
#include "WeaponTypeRef.h"
#include <list>

namespace StoneRing{
    class WeaponTypeExclusionList: public Element
	{
	public:
	    WeaponTypeExclusionList();
	    virtual ~WeaponTypeExclusionList();
	    virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

		virtual eElement whichElement() const{ return EWEAPONTYPEEXCLUSIONLIST; }	
	    std::list<WeaponTypeRef*>::const_iterator getWeaponTypeRefsBegin();
	    std::list<WeaponTypeRef*>::const_iterator getWeaponTypeRefsEnd();

	    virtual bool handleElement(eElement element, Element * pElement);
	private:
	    std::list<WeaponTypeRef*> mWeaponTypes;

	};
};

#endif