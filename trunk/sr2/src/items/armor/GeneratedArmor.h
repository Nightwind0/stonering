#ifndef SR_GENERATED_ARMOR_H
#define SR_GENERATED_ARMOR_H

#include "Item.h"
#include "Armor.h"


namespace StoneRing{
    class GeneratedArmor : public virtual Item, public Armor
	{
	public:
	    GeneratedArmor();
	    virtual ~GeneratedArmor();

	    ArmorRef generateArmorRef() const;

	    virtual std::string getIconRef() const;
	    virtual std::string getName() const;
	    virtual uint getMaxInventory() const ;
	    virtual eDropRarity getDropRarity() const;
	    virtual uint getValue() const ;
	    virtual uint getSellValue() const ;
	    virtual eItemType getItemType() const { return ARMOR ; }

	    ArmorType * getArmorType() const ;
	    ArmorClass * getArmorClass() const { return mpClass; }

	    virtual bool operator== ( const ItemRef &ref );

	    void generate( ArmorType * pType, ArmorClass * pClass, 
			   SpellRef *pSpell = NULL, RuneType *pRune = NULL);
        
	private:
	    std::string mName;
	    ArmorType  *mpType;
	    ArmorClass  *mpClass;
	};

};
#endif