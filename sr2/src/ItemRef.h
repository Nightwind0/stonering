#ifndef STONE_RING_ITEM_REF_H
#define STONE_RING_ITEM_REF_H

#include "Element.h"
#include <ClanLib/core.h>


namespace StoneRing
{

    class NamedItemRef;
    class WeaponRef;
    class ArmorRef;
    class Item;


class ItemRef : public Element
    {
    public:
	ItemRef();
	ItemRef(CL_DomElement *pElement );
	virtual ~ItemRef();

	enum eRefType { NAMED_ITEM, WEAPON_REF, ARMOR_REF };

//	std::string getItemName() const;

	eRefType getType() const;

	NamedItemRef * getNamedItemRef() const;
	WeaponRef * getWeaponRef() const;
	ArmorRef * getArmorRef() const;


	inline Item * getItem() const { return mpItem; }

	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
	
    protected:
	NamedItemRef * mpNamedItemRef;
	WeaponRef * mpWeaponRef;
	ArmorRef * mpArmorRef;
	eRefType meType;
	Item * mpItem;
    };



    class NamedItemRef : public Element
	{
	public:
	    NamedItemRef();
	    NamedItemRef(CL_DomElement *pElement );
	    virtual ~NamedItemRef();

	    std::string getItemName();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
	    std::string mName;
	};

};

#endif
