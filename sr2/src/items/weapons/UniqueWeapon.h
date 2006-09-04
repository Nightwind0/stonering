#ifndef SR_UNIQUE_WEAPON_H
#define SR_UNIQUE_WEAPON_H

#include "NamedItem.h"
#include "Weapon.h"

namespace StoneRing{
    class UniqueWeapon : public NamedItem, public Weapon
	{
	public:
	    UniqueWeapon();
	    ~UniqueWeapon();

		virtual eElement whichElement() const{ return EUNIQUEWEAPON; }	
	    virtual uint getValue() const ;
	    virtual uint getSellValue() const ;


	    WeaponType *getWeaponType() const ;
	    bool isRanged() const ;
	    bool isTwoHanded() const;
        
	    virtual eItemType getItemType() const { return WEAPON ; }
 
	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	private:
	    virtual bool handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
		virtual void loadFinished();
	    WeaponType * mpWeaponType;
		float mValueMultiplier;
	    uint mnValue;
        
        
	};
};
#endif