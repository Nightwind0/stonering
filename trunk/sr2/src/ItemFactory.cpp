
#include <ClanLib/core.h>
#include "ItemFactory.h"
#include "Item.h"

using namespace StoneRing;

ItemFactory::~ItemFactory(){}



NamedItemElement * ItemFactory::createNamedItemElement() const
{
    return new NamedItemElement();
}

NamedItemElement * ItemFactory::createNamedItemElement(CL_DomElement *pElement) const
{
    return new NamedItemElement(pElement);
}


RegularItem * ItemFactory::createRegularItem()const
{

    return new RegularItem();
}

RegularItem * ItemFactory::createRegularItem(CL_DomElement * pElement) const
{
    RegularItem * pItem = ItemFactory::createRegularItem();
    pItem->loadItem ( pElement );
    return pItem;

}

SpecialItem * ItemFactory::createSpecialItem()const
{
    return new SpecialItem();
}

SpecialItem * ItemFactory::createSpecialItem(CL_DomElement * pElement) const
{
    SpecialItem * pItem = ItemFactory::createSpecialItem();
    pItem->loadItem ( pElement );
    return pItem;
}

SystemItem * ItemFactory::createSystemItem()const
{
    return new SystemItem();
}

SystemItem * ItemFactory::createSystemItem(CL_DomElement * pElement) const
{
    SystemItem * pItem = ItemFactory::createSystemItem();
    pItem->loadItem ( pElement );
    return pItem;
}

Rune * ItemFactory::createRune()const
{
    return new Rune();
}

Rune * ItemFactory::createRune(CL_DomElement * pElement) const
{

    Rune * pItem = ItemFactory::createRune();
    pItem->loadItem ( pElement );
    return pItem;
}

UniqueWeapon * ItemFactory::createUniqueWeapon()const
{
    return new UniqueWeapon();
}

UniqueWeapon * ItemFactory::createUniqueWeapon(CL_DomElement * pElement) const
{
    UniqueWeapon * pItem = ItemFactory::createUniqueWeapon();
    pItem->loadItem ( pElement );
    return pItem;

}

UniqueArmor * ItemFactory::createUniqueArmor()const
{
    return new UniqueArmor();
}

UniqueArmor * ItemFactory::createUniqueArmor(CL_DomElement * pElement) const
{
    UniqueArmor * pItem = ItemFactory::createUniqueArmor();
    pItem->loadItem ( pElement );
    return pItem;
}

WeaponTypeRef * ItemFactory::createWeaponTypeRef()const
{
    return new WeaponTypeRef();
}

WeaponTypeRef * ItemFactory::createWeaponTypeRef(CL_DomElement * pElement) const
{
    return new WeaponTypeRef(pElement);
}

WeaponClassRef * ItemFactory::createWeaponClassRef()const
{
    return new WeaponClassRef();
}

WeaponClassRef * ItemFactory::createWeaponClassRef(CL_DomElement * pElement) const
{
    return new WeaponClassRef(pElement);
}

ArmorTypeRef * ItemFactory::createArmorTypeRef()const
{
    return new ArmorTypeRef();
}

ArmorTypeRef * ItemFactory::createArmorTypeRef(CL_DomElement * pElement) const
{
    return new ArmorTypeRef(pElement);
}

ArmorClassRef * ItemFactory::createArmorClassRef()const
{
    return new ArmorClassRef();
}

ArmorClassRef * ItemFactory::createArmorClassRef(CL_DomElement * pElement) const
{
    return new ArmorClassRef(pElement);
}

WeaponRef * ItemFactory::createWeaponRef()const
{
    return new WeaponRef();
}

WeaponRef * ItemFactory::createWeaponRef(CL_DomElement * pElement) const
{
    return new WeaponRef(pElement);
}

ArmorRef * ItemFactory::createArmorRef()const
{
    return new ArmorRef();
}

ArmorRef * ItemFactory::createArmorRef(CL_DomElement * pElement) const
{
    return new ArmorRef(pElement);
}

RuneType * ItemFactory::createRuneType()const
{
    return new RuneType();
}

RuneType * ItemFactory::createRuneType(CL_DomElement * pElement) const
{
    return new RuneType(pElement);
}

SpellRef * ItemFactory::createSpellRef()const
{
    return new SpellRef();
}

SpellRef * ItemFactory::createSpellRef(CL_DomElement * pElement) const
{
    return new SpellRef(pElement);
}

WeaponEnhancer * ItemFactory::createWeaponEnhancer()const
{
    return new WeaponEnhancer();
}

WeaponEnhancer * ItemFactory::createWeaponEnhancer(CL_DomElement * pElement) const
{
    return new WeaponEnhancer(pElement);
}

ArmorEnhancer * ItemFactory::createArmorEnhancer()const
{
    return new ArmorEnhancer();
}

ArmorEnhancer * ItemFactory::createArmorEnhancer(CL_DomElement * pElement) const
{
    return new ArmorEnhancer(pElement);
}

AttributeEnhancer * ItemFactory::createAttributeEnhancer()const
{
    return new AttributeEnhancer();
}

AttributeEnhancer * ItemFactory::createAttributeEnhancer(CL_DomElement * pElement) const
{
    return new AttributeEnhancer(pElement);
}

WeaponClass * ItemFactory::createWeaponClass()const
{
    return new WeaponClass();
}

WeaponClass * ItemFactory::createWeaponClass(CL_DomElement * pElement) const
{
    return new WeaponClass(pElement);
}

WeaponType * ItemFactory::createWeaponType()const
{
    return new WeaponType();
}

WeaponType * ItemFactory::createWeaponType(CL_DomElement * pElement) const
{
    return new WeaponType(pElement);
}

ArmorClass * ItemFactory::createArmorClass()const
{
    return new ArmorClass();
}

ArmorClass * ItemFactory::createArmorClass(CL_DomElement * pElement) const
{
    return new ArmorClass(pElement);
}

ArmorType * ItemFactory::createArmorType()const
{
    return new ArmorType();
}

ArmorType * ItemFactory::createArmorType(CL_DomElement * pElement) const
{
    return new ArmorType(pElement);
}



