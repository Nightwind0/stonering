#ifndef SR_ITEM_MANAGER
#define SR_ITEM_MANAGER

#include "Item.h"
#include "Level.h"
#include "Equipment.h"
#include <ClanLib/core.h>


namespace StoneRing
{
    class Weapon;
    class Armor;

    class ItemManager
    {
    public:
        ItemManager();
        ~ItemManager();

        void LoadItemFile ( CL_DomDocument &doc );

        WeaponType*  GetWeaponType( const WeaponTypeRef &ref ) const;
        ArmorType*   GetArmorType ( const ArmorTypeRef &ref ) const;
        WeaponType*  GetWeaponType( const std::string &name ) const;
        ArmorType*   GetArmorType ( const std::string &name ) const;

        WeaponClass* GetWeaponClass ( const WeaponClassRef & ref ) const;
        ArmorClass * GetArmorClass  ( const ArmorClassRef & ref ) const;
        WeaponClass* GetWeaponClass ( const std::string &name ) const;
        ArmorClass * GetArmorClass  ( const std::string &name ) const;
        WeaponClass* GetWeaponImbuement ( const WeaponImbuementRef & ref ) const;
        ArmorClass * GetArmorImbuement  ( const ArmorImbuementRef & ref ) const;
        
        Armor*       GenerateRandomGeneratedArmor     ( Item::eDropRarity rarity, int min_value, int max_value )const;
        Weapon*      GenerateRandomGeneratedWeapon    ( Item::eDropRarity rarity, int min_value, int max_value )const;
        Item*        GenerateRandomItem               ( Item::eDropRarity rarity, int in_value, int max_value )const;

        Item * GetNamedItem( const std::string &name ) const;
        virtual Item * GetItem( const ItemRef & ref );
#ifndef NDEBUG
        void DumpItemList();
        void PrintAttributeEnhancers(Equipment * pItem );
        void PrintStatusModifiers(Equipment * pItem);
#endif
    private:
     
        typedef std::map<ItemRef,Item*> ItemMap;
        typedef std::map<std::string,Item*> NamedItemMap;

        Weapon * createWeapon(WeaponRef *pRef)const;
        Armor * createArmor(ArmorRef *pRef)const;

        std::vector<WeaponClass*> m_weapon_classes;
        std::vector<ArmorClass*> m_armor_classes;
        std::list<WeaponType*> m_weapon_types;
        std::list<ArmorType*> m_armor_types;
        std::vector<WeaponClass*> m_weapon_imbuements;
        std::vector<ArmorClass*> m_armor_imbuements;
        ItemMap m_items;
        NamedItemMap m_named_items;
    };
}
#endif




