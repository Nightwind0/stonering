#ifndef SR_ABILITY_FACTORY_H
#define SR_ABILITY_FACTORY_H



#include <ClanLib/core.h>

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


class ItemFactory
{
 public:
    ItemFactory(){}
    ~ItemFactory();

    virtual NamedItemElement * createNamedItemElement() const;
    virtual NamedItemElement * createNamedItemElement(CL_DomElement *pElement) const;
    virtual RegularItem * createRegularItem()const;  
    virtual RegularItem * createRegularItem(CL_DomElement * pElement) const;
    virtual SpecialItem * createSpecialItem()const; 
    virtual SpecialItem * createSpecialItem(CL_DomElement * pElement) const; 
    virtual SystemItem * createSystemItem()const; 
    virtual SystemItem * createSystemItem(CL_DomElement * pElement) const; 
    virtual Rune * createRune()const; 
    virtual Rune * createRune(CL_DomElement * pElement) const; 
    virtual UniqueWeapon * createUniqueWeapon()const; 
    virtual UniqueWeapon * createUniqueWeapon(CL_DomElement * pElement) const; 
    virtual UniqueArmor * createUniqueArmor()const; 
    virtual UniqueArmor * createUniqueArmor(CL_DomElement * pElement) const; 
    virtual WeaponTypeRef * createWeaponTypeRef()const; 
    virtual WeaponTypeRef * createWeaponTypeRef(CL_DomElement * pElement) const; 
    virtual WeaponClassRef * createWeaponClassRef()const; 
    virtual WeaponClassRef * createWeaponClassRef(CL_DomElement * pElement) const; 
    virtual ArmorTypeRef * createArmorTypeRef()const; 
    virtual ArmorTypeRef * createArmorTypeRef(CL_DomElement * pElement) const; 
    virtual ArmorClassRef * createArmorClassRef()const; 
    virtual ArmorClassRef * createArmorClassRef(CL_DomElement * pElement) const; 
    virtual WeaponRef * createWeaponRef()const; 
    virtual WeaponRef * createWeaponRef(CL_DomElement * pElement) const; 
    virtual ArmorRef * createArmorRef()const; 
    virtual ArmorRef * createArmorRef(CL_DomElement * pElement) const; 
    virtual RuneType * createRuneType()const; 
    virtual RuneType * createRuneType(CL_DomElement * pElement) const; 
    virtual SpellRef * createSpellRef()const; 
    virtual SpellRef * createSpellRef(CL_DomElement * pElement) const; 
    virtual WeaponEnhancer * createWeaponEnhancer()const; 
    virtual WeaponEnhancer * createWeaponEnhancer(CL_DomElement * pElement) const; 
    virtual ArmorEnhancer * createArmorEnhancer()const; 
    virtual ArmorEnhancer * createArmorEnhancer(CL_DomElement * pElement) const; 
    virtual AttributeEnhancer * createAttributeEnhancer()const; 
    virtual AttributeEnhancer * createAttributeEnhancer(CL_DomElement * pElement) const; 
    virtual WeaponClass * createWeaponClass()const; 
    virtual WeaponClass * createWeaponClass(CL_DomElement * pElement) const; 
    virtual WeaponType * createWeaponType()const; 
    virtual WeaponType * createWeaponType(CL_DomElement * pElement) const; 
    virtual ArmorClass * createArmorClass()const; 
    virtual ArmorClass * createArmorClass(CL_DomElement * pElement) const; 
    virtual ArmorType * createArmorType()const; 
    virtual ArmorType * createArmorType(CL_DomElement * pElement) const; 
    virtual WeaponDamageCategory *createWeaponDamageCategory() const;
    virtual WeaponDamageCategory *createWeaponDamageCategory(CL_DomElement *pElement) const;
    virtual MagicDamageCategory *createMagicDamageCategory() const;
    virtual MagicDamageCategory *createMagicDamageCategory(CL_DomElement *pElement) const;


 private:
};


};

#endif
