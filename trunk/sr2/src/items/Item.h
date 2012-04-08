#ifndef SR_ITEM_H
#define SR_ITEM_H

#include <string>
#ifndef WIN32
#include "steel/SteelType.h"
#else
#include "SteelType.h"
#endif
#include "sr_defines.h"
#include "Element.h"
#include "ItemRef.h"



namespace StoneRing{
    class RuneType;
    class attributeModifier;
    class WeaponEnhancer;
    class ArmorEnhancer;
    class WeaponTypeRef;
    class ArmorTypeRef;
    class WeaponClassRef;
    class ArmorClassRef;
    class WeaponImbuementRef;
    class ArmorImbuementRef;
    class Action;
    class WeaponType;
    class ArmorType;
    class WeaponClass;
    class ArmorClass;
    class NamedItemElement;
    class DamageCategory;
    class WeaponDamageCategory;
    class MagicDamageCategory;
    class StatusEffect;
    class StatusEffectModifier;

    class Item: public SteelType::IHandle
    {
    public:
        Item();
        virtual ~Item();
        enum eItemType {
                        REGULAR_ITEM = 1,
			WEAPON = 2,
			ARMOR = 4,
			RUNE = 8,
			OMEGA = 16,
			SPECIAL = 32,
			SYSTEM  = 64    
			};
        enum eDropRarity { NEVER, COMMON, UNCOMMON, RARE };

        virtual std::string GetName() const = 0;
        virtual eItemType GetItemType() const = 0;
        virtual uint GetMaxInventory() const = 0;
        virtual eDropRarity GetDropRarity() const = 0;
        virtual std::string GetDescription() const = 0;

        virtual CL_Image GetIcon() const = 0;

        // These next two do not apply to special or system items.
        virtual uint GetValue() const = 0; // Price to buy, and worth when calculating drops.
        virtual uint GetSellValue() const = 0;
        static std::string ItemTypeAsString ( Item::eItemType type );
        static eDropRarity DropRarityFromString(const std::string &str);
        virtual bool operator == ( const ItemRef &ref )=0;

    private:

    };
    
    class ItemVisitor{
    public:
	ItemVisitor(){}
	virtual ~ItemVisitor(){}
	virtual void operator()(Item*,int)=0;
    };

    bool   operator < ( const Item &lhs, const Item &rhs );

    union SpellOrRuneRef
    {
        RuneType * mpRuneType;
    };




}




#endif




