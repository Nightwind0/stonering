#ifndef SR_GENERATED_WEAPON_H
#define SR_GENERATED_WEAPON_H



#include "Item.h"
#include "Weapon.h"

namespace StoneRing{
    class GeneratedWeapon : public virtual Item, public Weapon
	{
	public:
	    GeneratedWeapon();
	    virtual ~GeneratedWeapon();

	    WeaponRef generateWeaponRef() const;

	    // Item interface 
	    virtual std::string getIconRef() const;
	    virtual std::string getName() const;
	    virtual uint getMaxInventory() const ;
	    virtual eDropRarity getDropRarity() const;
	    virtual uint getValue() const ;
	    virtual uint getSellValue() const ;
	    virtual eItemType getItemType() const { return WEAPON ; }

	    // Weapon interface
	    WeaponType * getWeaponType() const;
	    WeaponClass * getWeaponClass() const { return mpClass; }
	    bool isRanged() const ;
	    bool isTwoHanded() const;
	    virtual bool operator== ( const ItemRef &ref );

	    void generate( WeaponType * pType, WeaponClass * pClass, 
			   SpellRef *pSpell = NULL, RuneType *pRune = NULL);

	private:

	    std::string mName; //generated at generate time :)
	    WeaponClass *mpClass;
	    WeaponType *mpType;
	};
};

#endif

