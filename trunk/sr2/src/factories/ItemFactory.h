#ifndef SR_ITEM_FACTORY_H
#define SR_ITEM_FACTORY_H


#include <ClanLib/core.h>
#include "IFactory.h"

namespace StoneRing
{


    class RegularItem;
    class SpecialItem;
    class SystemItem;
    class Rune;
    class UniqueWeapon;
    class UniqueArmor;
    class WeaponTypeRef;
    class WeaponClassRef;
    class ArmorTypeRef;
    class ArmorClassRef;
    class WeaponRef;
    class ArmorRef;
    class RuneType;
    class SpellRef;
    class WeaponEnhancer;
    class ArmorEnhancer;
    class AttributeEnhancer;
    class WeaponClass;
    class WeaponType;
    class ArmorClass;
    class ArmorType;
    class NamedItemElement;
    class WeaponDamageCategory;
    class MagicDamageCategory;
    class StatusEffectModifier;

    class ItemFactory : public IFactory
	{
	public:
	    ItemFactory(){}
	    ~ItemFactory();

	    virtual bool canCreate( Element::eElement element );
	    virtual Element * createElement( Element::eElement element );

	protected:

	    virtual Element * createNamedItemElement() const;
	    virtual Element * createRegularItem()const;  
	    virtual Element * createSpecialItem()const; 
	    virtual Element * createSystemItem()const; 
	    virtual Element * createRune()const; 
	    virtual Element * createUniqueWeapon()const; 
	    virtual Element * createUniqueArmor()const; 
	    virtual Element * createWeaponTypeRef()const; 
	    virtual Element * createWeaponClassRef()const; 
	    virtual Element * createArmorTypeRef()const; 
	    virtual Element * createArmorClassRef()const; 
	    virtual Element * createWeaponRef()const; 
	    virtual Element * createArmorRef()const; 
	    virtual Element * createRuneType()const; 
	    virtual Element * createSpellRef()const; 
	    virtual Element * createWeaponEnhancer()const; 
	    virtual Element * createArmorEnhancer()const; 
	    virtual Element * createAttributeEnhancer()const; 
	    virtual Element * createWeaponClass()const; 
	    virtual Element * createWeaponType()const; 
	    virtual Element * createArmorClass()const; 
	    virtual Element * createArmorType()const; 
	    virtual Element * createWeaponDamageCategory() const;
	    virtual Element * createMagicDamageCategory() const;
	    virtual Element * createStatusEffectModifier() const;
		virtual Element * createWeaponTypeExclusionList() const;
		virtual Element * createArmorTypeExclusionList() const;
		virtual Element * createIconRef() const;

	private:
	    typedef Element * (ItemFactory::*factoryMethod)() const;

	    factoryMethod getMethod(Element::eElement element) const;
	};


};

#endif
