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

        void loadItemFile ( CL_DomDocument &doc );

        WeaponType *getWeaponType( const WeaponTypeRef &ref ) const;
        ArmorType  *getArmorType ( const ArmorTypeRef &ref ) const;
        WeaponType *getWeaponType( const std::string &name ) const;
        ArmorType  *getArmorType ( const std::string &name ) const;

        WeaponClass *getWeaponClass ( const WeaponClassRef & ref ) const;
        ArmorClass  *getArmorClass ( const ArmorClassRef & ref ) const;
        WeaponClass *getWeaponClass ( const std::string &name ) const;
        ArmorClass *getArmorClass ( const std::string &name ) const;

        Item * getNamedItem( const std::string &name ) const;
        virtual Item * getItem( const ItemRef & ref );
#ifndef NDEBUG
        void dumpItemList();
        void printAttributeEnhancers(Equipment * pItem );
        void printStatusModifiers(Equipment * pItem);
#endif
    private:
        typedef std::map<ItemRef,Item*> ItemMap;
        typedef std::map<std::string,Item*> NamedItemMap;

        Weapon * createWeapon(WeaponRef *pRef)const;
        Armor * createArmor(ArmorRef *pRef)const;

        std::list<WeaponClass*> mWeaponClasses;
        std::list<ArmorClass*> mArmorClasses;
        std::list<WeaponType*> mWeaponTypes;
        std::list<ArmorType*> mArmorTypes;
        ItemMap mItems;
        NamedItemMap mNamedItems;
    };
};
#endif




