#ifndef SR_ITEM_H
#define SR_ITEM_H

#include <string>
#include "sr_defines.h"
#include "Element.h"
#include "ItemRef.h"



namespace StoneRing{

    class Spell;
    class SpellRef;
    class RuneType;
    class AttributeEnhancer;
    class WeaponEnhancer;
    class ArmorEnhancer;
    class WeaponTypeRef;
    class ArmorTypeRef;
    class WeaponClassRef;
    class ArmorClassRef;
    class Action;
    class WeaponType;
    class ArmorType;
    class WeaponClass;
    class ArmorClass;
    class NamedItem;
    class DamageCategory;
    class WeaponDamageCategory;
    class MagicDamageCategory;
    class StatusEffect;
    class StatusEffectModifier;

    class Item
	{
	public:
	    Item();
	    virtual ~Item();
	    enum eItemType { REGULAR_ITEM, WEAPON, ARMOR, RUNE, SPECIAL, SYSTEM };
	    enum eDropRarity { NEVER, COMMON, UNCOMMON, RARE };
        
	    virtual std::string getName() const = 0;
	    virtual eItemType getItemType() const = 0;
	    virtual uint getMaxInventory() const = 0;
	    virtual eDropRarity getDropRarity() const = 0;

	    virtual std::string getIconRef() const = 0;

	    // These next two do not apply to special or system items.
	    virtual uint getValue() const = 0; // Price to buy, and worth when calculating drops.
	    virtual uint getSellValue() const = 0;
	    static std::string ItemTypeAsString ( Item::eItemType type );
	    static eDropRarity DropRarityFromString(const std::string &str);

	    virtual bool operator== ( const ItemRef &ref )=0;
        
	private:

	};

    bool   operator < ( const Item &lhs, const Item &rhs );
    
    union SpellOrRuneRef
    {
        SpellRef *mpSpellRef;
        RuneType * mpRuneType;
    };


  

};




#endif
