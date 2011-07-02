#include <ClanLib/core.h>
#include "ItemManager.h"
#include "IApplication.h"
#include "StatusEffect.h"
#include "WeaponClass.h"
#include "WeaponType.h"
#include "ArmorType.h"
#include "ArmorClass.h"
#include "NamedItem.h"
#include "WeaponTypeRef.h"
#include "WeaponRef.h"
#include "ArmorRef.h"
#include "GeneratedWeapon.h"
#include "RuneType.h"
#include "ArmorTypeRef.h"
#include "GeneratedArmor.h"
#include "WeaponClassRef.h"
#include "ArmorClassRef.h"
#include "DamageCategory.h"
#include "StatusEffectModifier.h"
#include <cassert>


#include <algorithm>
#include <iomanip>
#include <functional>


using namespace StoneRing;


template <class Class>
class CompareEquipmentClasses: public std::binary_function<Class*, Class*, bool>
{
public:
    CompareEquipmentClasses(double basePrice):m_base_price(basePrice){}
    ~CompareEquipmentClasses(){}
    
    bool operator()(const Class *a, const Class *b)
    {
        return (a->GetValueMultiplier() * m_base_price + a->GetValueAdd()) <
                (b->GetValueMultiplier() * m_base_price + b->GetValueAdd());
    }
    
    bool operator()(const Class *a, double b)
    {
        return (a->GetValueMultiplier() * m_base_price + a->GetValueAdd()) < b;
    }
    
    bool operator()(double a, Class *b)
    {
        return a <     (b->GetValueMultiplier() * m_base_price + b->GetValueAdd());
    }
    
 private:
    double m_base_price;
};
template <class Class>
class CompareEquipmentClassWithChosenClass: public std::binary_function<Class*,Class*,bool>
{
public:
    CompareEquipmentClassWithChosenClass(double basePrice, Class* pClass):
    m_base_price(basePrice),m_pClass(pClass){}
    ~CompareEquipmentClassWithChosenClass(){}
    bool operator()(const Class *a, const Class *b)
    {
        return (a->GetValueMultiplier() * m_pClass->GetValueMultiplier() 
                * m_base_price + a->GetValueAdd() + m_pClass->GetValueAdd()) <
                (b->GetValueMultiplier() * m_pClass->GetValueMultiplier()
                * m_base_price + b->GetValueAdd() + m_pClass->GetValueAdd());
    }
    
    bool operator()(const Class *a, double b)
    {
        return (a->GetValueMultiplier() * m_pClass->GetValueMultiplier() 
                * m_base_price + a->GetValueAdd() + m_pClass->GetValueAdd())  < b;
    }
    
    bool operator()(double a, Class *b)
    {
        return a <  (b->GetValueMultiplier() * m_pClass->GetValueMultiplier()
                * m_base_price + b->GetValueAdd() + m_pClass->GetValueAdd());
    }
private:
    double m_base_price;
    Class *m_pClass;
};


template <class Type, class Class>
class EquipmentTypeClassFilter { 
public:
    EquipmentTypeClassFilter(Type* pType):m_type(pType){}
    ~EquipmentTypeClassFilter(){}
    
    bool operator()(Class* pClass)
    {
        return pClass->IsExcluded(m_type->GetName());
    }
private:
    Type* m_type;
};





ItemManager::ItemManager()
{
}

ItemManager::~ItemManager()
{
    // Clean up items..
}


void ItemManager::LoadItemFile ( CL_DomDocument &doc )
{
    IFactory * pItemFactory = IApplication::GetInstance()->GetElementFactory();

    CL_DomElement itemsNode = doc.named_item("items").to_element();
    CL_DomElement weaponClassesNode = itemsNode.named_item("weaponClasses").to_element();

    CL_DomElement weaponClassNode = weaponClassesNode.get_first_child().to_element();

    while(!weaponClassNode.is_null())
    {
        WeaponClass * pWeaponClass = dynamic_cast<WeaponClass*>( pItemFactory->createElement("weaponClass") );
        pWeaponClass->Load(weaponClassNode);
        if(pWeaponClass->Imbuement())
            m_weapon_imbuements.push_back( pWeaponClass );
        else
            m_weapon_classes.push_back ( pWeaponClass );
        weaponClassNode = weaponClassNode.get_next_sibling().to_element();
    }
    
    // Sort classes & imbuements
    //std::sort(m_weapon_classes.begin(),m_weapon_classes.end(),compare_weapons);
    //std::sort(m_weapon_imbuements.begin(),m_weapon_imbuements.end(),compare_weapons);

    CL_DomElement weaponTypesNode = itemsNode.named_item("weaponTypes").to_element();
    CL_DomElement weaponTypeNode = weaponTypesNode.get_first_child().to_element();

    while(!weaponTypeNode.is_null())
    {
        WeaponType * pWeaponType = dynamic_cast<WeaponType*>( pItemFactory->createElement("weaponType") );
        pWeaponType->Load(weaponTypeNode);
        m_weapon_types.push_back ( pWeaponType );

        weaponTypeNode = weaponTypeNode.get_next_sibling().to_element();
    }

    CL_DomElement armorClassesNode = itemsNode.named_item("armorClasses").to_element();

    CL_DomElement armorClassNode = armorClassesNode.get_first_child().to_element();

    while(!armorClassNode.is_null())
    {
        ArmorClass * pArmorClass = dynamic_cast<ArmorClass*>( pItemFactory->createElement("armorClass") );
        pArmorClass->Load(armorClassNode);
        if(pArmorClass->Imbuement())
            m_armor_imbuements.push_back ( pArmorClass );
        else 
            m_armor_classes.push_back ( pArmorClass);

        armorClassNode = armorClassNode.get_next_sibling().to_element();
    }

    // Sort classes & imbuements
   // std::sort(m_armor_classes.begin(),m_armor_classes.end(),compare_armor);
   // std::sort(m_armor_imbuements.begin(),m_armor_imbuements.end(),compare_armor);
    
    CL_DomElement armorTypesNode = itemsNode.named_item("armorTypes").to_element();
    CL_DomElement armorTypeNode = armorTypesNode.get_first_child().to_element();

    while(!armorTypeNode.is_null())
    {
        ArmorType * pArmorType = dynamic_cast<ArmorType*>( pItemFactory->createElement("armorType") );
        pArmorType->Load(armorTypeNode);
        m_armor_types.push_back ( pArmorType);

        armorTypeNode = armorTypeNode.get_next_sibling().to_element();
    }

    CL_DomElement itemListNode = itemsNode.named_item("itemList").to_element();
    CL_DomElement namedItemNode = itemListNode.get_first_child().to_element();

#ifndef NDEBUG
    uint namedItemCount = 0;
#endif

    while(!namedItemNode.is_null())
    {
#ifndef NDEBUG
        namedItemCount++;
#endif
        NamedItemElement* pElement = dynamic_cast<NamedItemElement*>(pItemFactory->createElement ( namedItemNode.get_node_name() ));

        pElement->Load(namedItemNode);
	
	switch(pElement->WhichElement())
	{
	    case Element::EREGULARITEM:
	    case Element::ESYSTEMITEM:
	    case Element::ESPECIALITEM:
	    case Element::EUNIQUEARMOR:
	    case Element::EUNIQUEWEAPON:
		Item * pItem = dynamic_cast<Item*>(pElement);
		m_named_items[pItem->GetName()] = pItem;
		break;
	}


        namedItemNode = namedItemNode.get_next_sibling().to_element();
    }


}
#if 0
void ItemManager::generateWeapons()
{
    for(std::list<WeaponType*>::iterator iter = mWeaponTypes.begin();
        iter != mWeaponTypes.end();
        iter++)
    {
        WeaponType *pType = *iter;
#ifndef NDEBUG
        std::cout << "Generating weapons for type: " << pType->getName() << std::endl;
#endif

        for(std::list<WeaponClass*>::iterator classIter = mWeaponClasses.begin();
            classIter != mWeaponClasses.end();
            classIter++)
        {
            WeaponClass * pClass = *classIter;

            WeaponTypeRef typeRef;
            typeRef.setName ( pType->getName() );

            if(!pClass->isExcluded( typeRef ) )
            {
                // Create all the spell combinations.
                for(std::list<Spell*>::const_iterator spellIter = AbilityManager::getSpellsBegin();
                    spellIter != AbilityManager::getSpellsEnd();
                    spellIter++)
                {
                    Spell * pSpell = *spellIter;

                    if(pSpell->appliesToWeapons())
                    {
                        GeneratedWeapon * pWeapon = new GeneratedWeapon();
                        pWeapon->generate(pType, pClass, pSpell->createSpellRef(), NULL);
                        mItems.push_back ( pWeapon );
                    }
                }

                // Create the no spell, no rune weapon

                GeneratedWeapon * pPlainWeapon = new GeneratedWeapon();
                pPlainWeapon->generate( pType, pClass, NULL, NULL );

                mItems.push_back ( pPlainWeapon );


                // Create the rune version.
                GeneratedWeapon * pRuneWeapon = new GeneratedWeapon();
                RuneType * runeType = new RuneType();
                runeType->setRuneType( RuneType::RUNE );
                pRuneWeapon->generate( pType, pClass, NULL, runeType );

                mItems.push_back ( pRuneWeapon );

            }
        }
    }
}

void ItemManager::generateArmor()
{

    for(std::list<ArmorType*>::const_iterator iter = mArmorTypes.begin();
        iter != mArmorTypes.end();
        iter++)
    {
        ArmorType *pType = *iter;

#ifndef NDEBUG
        std::cout << "Generating armor for type: " << pType->getName() << std::endl;
#endif

        for(std::list<ArmorClass*>::const_iterator classIter = mArmorClasses.begin();
            classIter != mArmorClasses.end();
            classIter++)
        {
            ArmorClass * pClass = *classIter;

            ArmorTypeRef typeRef;
            typeRef.setName ( pType->getName() );

            if(!pClass->isExcluded( typeRef ) )
            {
                // Create all the spell combinations.
                for(std::list<Spell*>::const_iterator spellIter = AbilityManager::getSpellsBegin();
                    spellIter != AbilityManager::getSpellsEnd();
                    spellIter++)
                {
                    Spell * pSpell = *spellIter;

                    if(pSpell->appliesToArmor())
                    {
                        GeneratedArmor * pArmor = new GeneratedArmor();
                        pArmor->generate(pType, pClass, pSpell->createSpellRef(), NULL);
                        mItems.push_back ( pArmor );
                    }
                }




                // Create the no spell, no rune Armor

                GeneratedArmor * pPlainArmor = new GeneratedArmor();
                pPlainArmor->generate( pType, pClass );

                mItems.push_back ( pPlainArmor );


                // Create the rune version.
                GeneratedArmor * pRuneArmor = new GeneratedArmor();
                RuneType * runeType = new RuneType();
                runeType->setRuneType( RuneType::RUNE );
                pRuneArmor->generate( pType, pClass, NULL, runeType );

                mItems.push_back ( pRuneArmor );

                // Create the ultra rune version

                GeneratedArmor * pUltraRuneArmor = new GeneratedArmor();
                RuneType * ultraRuneType = new RuneType();
                ultraRuneType->setRuneType( RuneType::ULTRA_RUNE );

                pUltraRuneArmor->generate( pType, pClass, NULL, ultraRuneType );

                mItems.push_back ( pUltraRuneArmor );


            }
        }
    }
}

#endif

WeaponType * ItemManager::GetWeaponType(const WeaponTypeRef &ref) const
{
    for(std::list<WeaponType*>::const_iterator iter = m_weapon_types.begin();
        iter != m_weapon_types.end();
        iter++)
    {
        if( ref.GetName() == (*iter)->GetName())
            return *iter;
    }

    assert(0);
    return NULL;
}

ArmorType  * ItemManager::GetArmorType ( const ArmorTypeRef &ref) const
{
    for(std::list<ArmorType*>::const_iterator iter = m_armor_types.begin();
        iter != m_armor_types.end();
        iter++)
    {
        if( ref.GetName() == (*iter)->GetName())
            return *iter;
    }

    assert ( 0 && "Armor type not found");
    return NULL;
}

WeaponClass * ItemManager::GetWeaponClass ( const WeaponClassRef & ref ) const
{
    for(std::vector<WeaponClass*>::const_iterator iter = m_weapon_classes.begin();
        iter != m_weapon_classes.end();
        iter++)
    {
        if( ref.GetName() == (*iter)->GetName())
            return *iter;
    }
    assert(0 && "Weapon class not found");
    return NULL;
}

ArmorClass  * ItemManager::GetArmorClass ( const ArmorClassRef & ref ) const
{
    for(std::vector<ArmorClass*>::const_iterator iter = m_armor_classes.begin();
        iter != m_armor_classes.end();
        iter++)
    {
        if( ref.GetName() == (*iter)->GetName())
            return *iter;
    }

    assert ( 0 && "Armor class not found.");
    return NULL;
}

WeaponClass * ItemManager::GetWeaponImbuement ( const WeaponImbuementRef & ref ) const
{
    for(std::vector<WeaponClass*>::const_iterator iter = m_weapon_imbuements.begin();
        iter != m_weapon_imbuements.end();
        iter++)
    {
        if( ref.GetName() == (*iter)->GetName())
            return *iter;
    }
    assert(0 && "Weapon imbuement not found");
    return NULL;
}

ArmorClass  * ItemManager::GetArmorImbuement ( const ArmorImbuementRef & ref ) const
{
    for(std::vector<ArmorClass*>::const_iterator iter = m_armor_imbuements.begin();
        iter != m_armor_imbuements.end();
        iter++)
    {
        if( ref.GetName() == (*iter)->GetName())
            return *iter;
    }

    assert ( 0 && "Armor imbuement not found.");
    return NULL;
}

Item * ItemManager::GetNamedItem( const std::string &name ) const
{
    NamedItemMap::const_iterator iter = m_named_items.find(name);

    if(iter != m_named_items.end())
    {
        return iter->second;
    }

    throw CL_Exception("Couldn't find item by name: " + name);
    return NULL;
}

Item * ItemManager::GetItem( const ItemRef & ref )
{
    ItemMap::const_iterator iter = m_items.find(ref);

    if(iter != m_items.end())
    {
        // Already have this one created.
        return iter->second;
    }
    else
    {
        switch(ref.GetType())
        {
        case ItemRef::NAMED_ITEM:
            {
                NamedItemRef *pRef = ref.GetNamedItemRef();
                return GetNamedItem(pRef->GetItemName());
            }
        case ItemRef::WEAPON_REF:
            {
                WeaponRef *pRef = ref.GetWeaponRef();
                Weapon *pWeapon = createWeapon(pRef);
                m_items[ref] = pWeapon;
                return pWeapon;
            }
        case ItemRef::ARMOR_REF:
            {
                ArmorRef *pRef = ref.GetArmorRef();
                Armor *pArmor = createArmor(pRef);
                m_items[ref] = pArmor;
                return pArmor;
            }
        default:
            assert(0);
        }
    }

    throw CL_Exception("Couldn't find item based on ref.");
    return NULL;
}

Weapon * ItemManager::createWeapon(WeaponRef *pRef) const
{
    GeneratedWeapon * pWeapon = new GeneratedWeapon();
    pWeapon->Generate(pRef->GetWeaponType(), pRef->GetWeaponClass(),
        pRef->GetWeaponImbuement(),pRef->GetRuneType());
    return pWeapon;
}

Armor * ItemManager::createArmor(ArmorRef *pRef) const
{
    GeneratedArmor * pArmor = new GeneratedArmor();
    pArmor->Generate(pRef->GetArmorType(),pRef->GetArmorClass(),
        pRef->GetArmorImbuement(),pRef->GetRuneType());
    return pArmor;
}

#if 0
void ItemManager::dumpItemList()
{
    for(std::list<Item*>::iterator iter = mItems.begin();
        iter != mItems.end();
        iter++)
    {
        Item * pItem = *iter;

        std::cout << '[' << Item::ItemTypeAsString(pItem->getItemType()) << ']' << ' ';
        std::cout << pItem->getName();
        std::cout << " (" << pItem->getDropRarity() << ") ";
        std::cout << '$' << pItem->getValue();
        std::cout << std::endl;


        switch(pItem->getItemType())
        {
        case Item::WEAPON:
        {
            Weapon * pWeapon = dynamic_cast<Weapon*>(pItem);
            WeaponType * pType = pWeapon->getWeaponType();
            std::cout << '\t' << "ATK: " << std::setw(5) <<  pWeapon->modifyWeaponAttribute(Weapon::ATTACK, (int)pType->getBaseAttack());
            std::cout << ' ' << "Hit% " << std::setw(4) << pWeapon->modifyWeaponAttribute(Weapon::HIT, pType->getBaseHit()) * 100;
            std::cout << ' ' << "Critical% " << std::setw(4) << pWeapon->modifyWeaponAttribute(Weapon::CRITICAL, pType->getBaseCritical()) * 100 << std::endl;
            DamageCategory * pDamageCategory = pType->getDamageCategory();

            if(pDamageCategory->getClass() != DamageCategory::WEAPON) throw CL_Exception("What? This weapon has a magic damage category.");

            WeaponDamageCategory * pWDC = dynamic_cast<WeaponDamageCategory*>(pDamageCategory);


            switch ( pWDC->getType() )
            {
            case WeaponDamageCategory::pierce:
                std::cout << "\t[pierce]";
                break;
            case WeaponDamageCategory::BASH:
                std::cout << "\t[BASH]";
                break;
            case WeaponDamageCategory::SLASH:
                std::cout << "\t[SLASH]";
                break;

            }


            if(pWeapon->isTwoHanded()) std::cout << "(Two Handed)";

            if(pWeapon->isRanged()) std::cout << "(Ranged)" ;

            std::cout << std::endl;

            // If there are attribute enhancers, lets list them.
            printAttributeEnhancers(pWeapon);
            printStatusModifiers(pWeapon);

            break;
        }
        case Item::ARMOR:
        {
            Armor * pArmor = dynamic_cast<Armor*>(pItem);

            ArmorType * pType = pArmor->getArmorType();
            std::cout << '\t' << "AC: " << std::setw(5) <<  pArmor->modifyArmorAttribute(Armor::AC, (int)pType->getBaseAC());
            std::cout << ' ' << "RST " << std::setw(4) << pArmor->modifyArmorAttribute(Armor::RESIST, pType->getBaseRST()) << std::endl;

            switch( pType->getSlot())
            {
            case Equipment::EHEAD:
                std::cout << "\t[HEAD]";
                break;
            case Equipment::EBODY:
                std::cout << "\t[BODY]";
                break;
            case Equipment::EFEET:
                std::cout << "\t[FEET]";
                break;
            case Equipment::EOFFHAND:
                break;
            case Equipment::EHANDS:
                std::cout << "\t[HANDS]";
                break;
            case Equipment::EFINGER1:
                break;
            }

            std::cout << std::endl;

            printAttributeEnhancers(pArmor);
            printStatusModifiers(pArmor);

            break;
        }

        }

    }
}

void ItemManager::printAttributeEnhancers(Equipment * pItem )
{
    if( pItem->getAttributeEnhancersBegin() != pItem->getAttributeEnhancersEnd() )
    {
        std::cout << "\tAttribute Enhancers:"  << std::endl;

        for(std::list<attributeModifier*>::const_iterator iter = pItem->getAttributeEnhancersBegin();
            iter != pItem->getAttributeEnhancersEnd();
            iter++)
        {
            attributeModifier * pEnhancer = *iter;
            std::cout << "\t\t" << pEnhancer->getAttribute() << ' ';
            if(pEnhancer->getMultiplier() != 1)
            {
                if ( pEnhancer->getMultiplier() > 1)
                    std::cout << (pEnhancer->getMultiplier() - 1) * 100 << "% BONUS";
                else std::cout << abs(pEnhancer->getMultiplier() - 1) * 100 << "% PENALTY";
            }
            if(pEnhancer->getAdd() != 0)
                std::cout << " +" << pEnhancer->getAdd();
            std::cout << std::endl;
        }
    }

}

void ItemManager::printStatusModifiers(Equipment *pItem)
{
    if ( pItem->getStatusEffectModifiersBegin() != pItem->getStatusEffectModifiersEnd() )
    {
        std::cout << "\tStatus Modifiers:" << std::endl;

        for(std::list<StatusEffectModifier*>::const_iterator iter = pItem->getStatusEffectModifiersBegin();
            iter != pItem->getStatusEffectModifiersEnd();
            iter++)
        {
            StatusEffectModifier * pModifier = *iter;
            std::cout << "\t\t" << pModifier->getStatusEffect()->getName() << ": " << pModifier->getModifier() * 100 << "%" ;
            std::cout << std::endl;
        }

    }

}

#endif


Armor* ItemManager::GenerateRandomGeneratedArmor(Item::eDropRarity rarity, int min_value, int max_value ) const
{
    // Here is the full blown algorithm that comes up with all the possible options.
    // Could get slow if there are lots of types, classes and imbuements....
    // In which case, maybe pick random classes and imbuements at the same time,
    // then look for any fitting types, if none, try again.
#ifndef NDEBUG
    std::cout << std::endl << (m_armor_types.size() * m_armor_classes.size() * m_armor_imbuements.size())
        << " total possible armor" << std::endl;
#endif    
    std::vector<Armor*> m_options;

    for(std::list<ArmorType*>::const_iterator type_iter = m_armor_types.begin();
        type_iter != m_armor_types.end(); 
        type_iter++) {
    
        for(std::vector<ArmorClass*>::const_iterator class_iter = m_armor_classes.begin();
            class_iter != m_armor_classes.end(); 
            class_iter++) {
            if((*class_iter)->IsExcluded((*type_iter)->GetName()))
                continue;            
            if(rarity == Item::UNCOMMON){
                int value = (*type_iter)->GetBasePrice() * (*class_iter)->GetValueMultiplier() + 
                                (*class_iter)->GetValueAdd();
                if(value >= min_value && value <= max_value){
                    GeneratedArmor *pArmor = new GeneratedArmor();
                    pArmor->Generate(*type_iter,*class_iter,NULL,NULL);
                    m_options.push_back(pArmor);
                }
            }else if(rarity == Item::RARE){
                for(std::vector<ArmorClass*>::const_iterator imbuement_iter = m_armor_imbuements.begin();
                    imbuement_iter != m_armor_imbuements.end();
                    imbuement_iter++){
                    if((*imbuement_iter)->IsExcluded((*type_iter)->GetName()))
                        continue;                    
                    int value = (*type_iter)->GetBasePrice() * (*class_iter)->GetValueMultiplier() * (*imbuement_iter)->GetValueMultiplier()
                                + (*class_iter)->GetValueAdd() + (*imbuement_iter)->GetValueAdd();
                    if(value >= min_value && value <= max_value){
                        GeneratedArmor *pArmor = new GeneratedArmor();
                        pArmor->Generate(*type_iter,*class_iter,*imbuement_iter,NULL);
                        m_options.push_back(pArmor);
                    }                                
                }
            }
        }
    }
    
    if(m_options.size()){
        uint selection = rand() % m_options.size();
        for(uint i=0;i<m_options.size();i++){
            if(selection != i)
                delete m_options[i];
        }
        
        return m_options[selection];
    }
    
#ifndef NDEBUG
    std::cout << m_options.size() 
        << " options applied" << std::endl;
#endif

    return NULL;
}

Weapon* ItemManager::GenerateRandomGeneratedWeapon(Item::eDropRarity rarity, int min_value, int max_value ) const
{
    // Here is the full blown algorithm that comes up with all the possible options.
    // Could get slow if there are lots of types, classes and imbuements....
    // In which case, maybe pick random classes and imbuements at the same time,
    // then look for any fitting types, if none, try again.
#ifndef NDEBUG
    std::cout << std::endl << (m_weapon_types.size() * m_weapon_classes.size() * m_weapon_imbuements.size())
        << " total possible weapons" << std::endl;
#endif
    std::vector<Weapon*> m_options;

    for(std::list<WeaponType*>::const_iterator type_iter = m_weapon_types.begin();
        type_iter != m_weapon_types.end(); 
        type_iter++) {
    
        for(std::vector<WeaponClass*>::const_iterator class_iter = m_weapon_classes.begin();
            class_iter != m_weapon_classes.end(); 
            class_iter++) {
            if((*class_iter)->IsExcluded((*type_iter)->GetName()))
                continue;
            if(rarity == Item::UNCOMMON){
                int value = (*type_iter)->GetBasePrice() * (*class_iter)->GetValueMultiplier() + 
                                (*class_iter)->GetValueAdd();
                if(value >= min_value && value <= max_value){
                    GeneratedWeapon *pWeapon = new GeneratedWeapon();
                    pWeapon->Generate(*type_iter,*class_iter,NULL,NULL);
                    m_options.push_back(pWeapon);
                }
            }else if(rarity == Item::RARE){
                for(std::vector<WeaponClass*>::const_iterator imbuement_iter = m_weapon_imbuements.begin();
                    imbuement_iter != m_weapon_imbuements.end();
                    imbuement_iter++){
                    if((*imbuement_iter)->IsExcluded((*type_iter)->GetName()))
                        continue;
                    int value = (*type_iter)->GetBasePrice() * (*class_iter)->GetValueMultiplier() * (*imbuement_iter)->GetValueMultiplier()
                                + (*class_iter)->GetValueAdd() + (*imbuement_iter)->GetValueAdd();
                    if(value >= min_value && value <= max_value){
                        GeneratedWeapon *pWeapon = new GeneratedWeapon();
                        pWeapon->Generate(*type_iter,*class_iter,*imbuement_iter,NULL);
                        m_options.push_back(pWeapon);
                    }                                
                }
            }
        }
    }
    
#ifndef NDEBUG
    std::cout << m_options.size() 
        << " options applied" << std::endl;
#endif
    
    if(m_options.size()){
        uint selection = rand() % m_options.size();
        for(uint i=0;i<m_options.size();i++){
            if(selection != i)
                delete m_options[i];
        }
        
        return m_options[selection];
    }
    
    
    return NULL;
}

Item* ItemManager::GenerateRandomItem(Item::eDropRarity rarity, int min_value, int max_value ) const
{
    return NULL;
}


