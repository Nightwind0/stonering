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

        static void initialize();

        static void LoadItemFile ( CL_DomDocument &doc );

        static WeaponType*  GetWeaponType( const WeaponTypeRef &ref );
        static ArmorType*   GetArmorType ( const ArmorTypeRef &ref );
        static WeaponType*  GetWeaponType( const std::string &name );
        static ArmorType*   GetArmorType ( const std::string &name );

        static WeaponClass* GetWeaponClass ( const WeaponClassRef & ref );
        static ArmorClass * GetArmorClass  ( const ArmorClassRef & ref );
        static WeaponClass* GetWeaponClass ( const std::string &name );
        static ArmorClass * GetArmorClass  ( const std::string &name );
        static WeaponClass* GetWeaponImbuement ( const WeaponImbuementRef & ref );
        static ArmorClass * GetArmorImbuement  ( const ArmorImbuementRef & ref );
        static WeaponClass* GetWeaponImbuement ( const std::string& name );
        static ArmorClass* GetArmorImbuement  ( const std::string& name );
        
        static Armor*       GenerateRandomGeneratedArmor     ( Item::eDropRarity rarity, int min_value, int max_value );
        static Weapon*      GenerateRandomGeneratedWeapon    ( Item::eDropRarity rarity, int min_value, int max_value );
        static Item*        GenerateRandomItem               ( Item::eDropRarity rarity, int in_value, int max_value );

        static Item * GetNamedItem( const std::string &name );
        static Item * GetItem( const ItemRef & ref );
        
        static void SerializeItem(std::ostream& out, Item* pItem);
        static Item* DeserializeItem(std::istream& in);
#ifndef NDEBUG
        void DumpItemList();
        void PrintAttributeEnhancers(Equipment * pItem );
        void PrintStatusModifiers(Equipment * pItem);
#endif
    private:
        typedef std::map<ItemRef,Item*> ItemMap;
        typedef std::map<std::string,Item*> NamedItemMap;

        static Weapon * createWeapon(WeaponRef *pRef);
        static Armor * createArmor(ArmorRef *pRef);

        std::vector<WeaponClass*> m_weapon_classes;
        std::vector<ArmorClass*> m_armor_classes;
        std::list<WeaponType*> m_weapon_types;
        std::list<ArmorType*> m_armor_types;
        std::vector<WeaponClass*> m_weapon_imbuements;
        std::vector<ArmorClass*> m_armor_imbuements;
        ItemMap m_items;
        NamedItemMap m_named_items;
        
        static ItemManager * m_pInstance;
        ItemManager();
        ~ItemManager();
        
    };
}
#endif




