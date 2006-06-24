
#include <ClanLib/core.h>
#include "ItemFactory.h"
#include "Item.h"

using namespace StoneRing;

ItemFactory::~ItemFactory(){}


Element * ItemFactory::createIconRef() const
{
	return new IconRef();
}


Element * ItemFactory::createNamedItemElement() const
{
    return new NamedItemElement();
}


Element * ItemFactory::createRegularItem()const
{
    return new RegularItem();
}

Element * ItemFactory::createWeaponTypeExclusionList() const
{
	return new WeaponTypeExclusionList;
}
Element * ItemFactory::createArmorTypeExclusionList() const
{
	return new ArmorTypeExclusionList;
}

Element * ItemFactory::createSpecialItem()const
{
    return new SpecialItem();
}


Element * ItemFactory::createSystemItem()const
{
    return new SystemItem();
}


Element * ItemFactory::createRune()const
{
    return new Rune();
}


Element * ItemFactory::createUniqueWeapon()const
{
    return new UniqueWeapon();
}


Element * ItemFactory::createUniqueArmor()const
{
    return new UniqueArmor();
}


Element * ItemFactory::createWeaponTypeRef()const
{
    return new WeaponTypeRef();
}


Element * ItemFactory::createWeaponClassRef()const
{
    return new WeaponClassRef();
}


Element * ItemFactory::createArmorTypeRef()const
{
    return new ArmorTypeRef();
}


Element * ItemFactory::createArmorClassRef()const
{
    return new ArmorClassRef();
}


Element * ItemFactory::createWeaponRef()const
{
    return new WeaponRef();
}

Element * ItemFactory::createArmorRef()const
{
    return new ArmorRef();
}


Element * ItemFactory::createRuneType()const
{
    return new RuneType();
}


Element * ItemFactory::createSpellRef()const
{
    return new SpellRef();
}


Element * ItemFactory::createWeaponEnhancer()const
{
    return new WeaponEnhancer();
}


Element * ItemFactory::createArmorEnhancer()const
{
    return new ArmorEnhancer();
}


Element * ItemFactory::createAttributeEnhancer()const
{
    return new AttributeEnhancer();
}


Element * ItemFactory::createWeaponClass()const
{
    return new WeaponClass();
}

Element * ItemFactory::createWeaponType()const
{
    return new WeaponType();
}

Element * ItemFactory::createArmorClass()const
{
    return new ArmorClass();
}

Element * ItemFactory::createArmorType()const
{
    return new ArmorType();
}

Element * ItemFactory::createWeaponDamageCategory() const
{
    return new WeaponDamageCategory();
}


Element * ItemFactory::createMagicDamageCategory() const
{
    return new MagicDamageCategory();
}


Element *ItemFactory::createStatusEffectModifier() const
{
    return new StatusEffectModifier();
}


bool ItemFactory::canCreate( Element::eElement element )
{
    factoryMethod method = getMethod(element);

    if(method == NULL) return false;
    else return true;
		
}

Element * ItemFactory::createElement( Element::eElement element )
{
    factoryMethod method = getMethod(element);

    cl_assert ( method );

    Element * pElement = (this->*method)();

    return pElement;
}


ItemFactory::factoryMethod 
ItemFactory::getMethod(Element::eElement element) const
{
    switch(element)
    {
    case Element::EREGULARITEM:
	return &ItemFactory::createRegularItem;
    case Element::ESPECIALITEM:
	return &ItemFactory::createSpecialItem;
    case Element::ESYSTEMITEM:
	return &ItemFactory::createSystemItem;
    case Element::ERUNE:
	return &ItemFactory::createRune;
    case Element::EUNIQUEWEAPON:
	return &ItemFactory::createUniqueWeapon;
    case Element::EUNIQUEARMOR:
	return &ItemFactory::createUniqueArmor;
    case Element::EWEAPONTYPEREF:
	return &ItemFactory::createWeaponTypeRef;
    case Element::EWEAPONCLASSREF:
	return &ItemFactory::createWeaponClassRef;
    case Element::EARMORTYPEREF:
	return &ItemFactory::createArmorTypeRef;
    case Element::EARMORCLASSREF:
	return &ItemFactory::createArmorClassRef;
    case Element::EWEAPONREF:
	return &ItemFactory::createWeaponRef;
    case Element::EARMORREF:
	return &ItemFactory::createArmorRef;
    case Element::ERUNETYPE:
	return &ItemFactory::createRuneType;
    case Element::ESPELLREF:
	return &ItemFactory::createSpellRef;
    case Element::EWEAPONENHANCER:
	return &ItemFactory::createWeaponEnhancer;
    case Element::EARMORENHANCER:
	return &ItemFactory::createArmorEnhancer;
    case Element::EATTRIBUTEENHANCER:
	return &ItemFactory::createAttributeEnhancer;
    case Element::EWEAPONCLASS:
	return &ItemFactory::createWeaponClass;
    case Element::EWEAPONTYPE:
	return &ItemFactory::createWeaponType;
    case Element::EARMORCLASS:
	return &ItemFactory::createArmorClass;
    case Element::EARMORTYPE:
	return &ItemFactory::createArmorType;
    case Element::ENAMEDITEMELEMENT:
	return &ItemFactory::createNamedItemElement;
    case Element::EWEAPONDAMAGECATEGORY:
	return &ItemFactory::createWeaponDamageCategory;
    case Element::EMAGICDAMAGECATEGORY:
	return &ItemFactory::createMagicDamageCategory;
    case Element::ESTATUSEFFECTMODIFIER:
	return &ItemFactory::createStatusEffectModifier;
	case Element::EWEAPONTYPEEXCLUSIONLIST:
		return &ItemFactory::createWeaponTypeExclusionList;
	case Element::EARMORTYPEEXCLUSIONLIST:
		return &ItemFactory::createArmorTypeExclusionList;
	case Element::EICONREF:
		return &ItemFactory::createIconRef;
    default:
	return NULL;
    }
}
