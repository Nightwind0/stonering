#ifndef SR_ITEM_H
#define SR_ITEM_H

#include <string>
#include "Element.h"


namespace StoneRing{


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

	
    private:

    };

    bool   operator < ( const Item &lhs, const Item &rhs );
    
    union SpellOrRuneRef
    {
	SpellRef *mpSpellRef;
	RuneType * mpRuneType;
    };

    class Equipment
    {
    public:
	Equipment();
	virtual ~Equipment();


	SpellRef * getSpellRef() const;
	RuneType * getRuneType() const;
	
	bool hasSpell() const ;
	bool hasRuneType() const;

	// Apply any attribute enhancements 
	void equip();

	// Remove any attribute enhancements
        void unequip();


	std::list<AttributeEnhancer*>::const_iterator getAttributeEnhancersBegin() const;
	std::list<AttributeEnhancer*>::const_iterator getAttributeEnhancersEnd() const;

    protected:
	void clearAttributeEnhancers();
	void addAttributeEnhancer( AttributeEnhancer * pAttr );
	void setSpellRef ( SpellRef * pRef );
	void setRuneType ( RuneType * pType );


    private:
	std::list<AttributeEnhancer*> mAttributeEnhancers;
	SpellOrRuneRef  mSpellOrRuneRef;
	enum eMagic { NONE, SPELL, RUNE };
	eMagic meMagic;
	
    };


    class Weapon : public Equipment
    {
    public:
	Weapon();
	virtual ~Weapon();

	virtual WeaponType * getWeaponType() const = 0;
	virtual bool isRanged() const = 0;

	enum eAttribute
	{
	    ATTACK,
	    HIT,
	    POISON,
	    STONE,
	    DEATH,
	    CONFUSE,
	    BERSERK,
	    SLOW,
	    WEAK,
	    BREAK, 
	    SILENCE,
	    SLEEP,
	    BLIND,
	    STEAL_HP,
	    STEAL_MP,
	    DROPSTR,
	    DROPDEX,
	    DROPMAG
	};
	

	static eAttribute attributeForString(const std::string str);	 
	int modifyWeaponAttribute( eAttribute attr, int current );
	float modifyWeaponAttribute ( eAttribute attr, float current );

	// Getters for weapon enhancers. need 'em.

    protected:
	
	void clearWeaponEnhancers();
	void addWeaponEnhancer (WeaponEnhancer * pEnhancer);

    private:
	std::list<WeaponEnhancer*> mWeaponEnhancers;
	
    };

    class Armor : public Equipment
    {
    public:
	Armor();
	virtual ~Armor();
	
	virtual ArmorType *getArmorType() const = 0;

	enum eAttribute
	{
	    AC,
	    POISON,
	    STONE,
	    DEATH,
	    CONFUSE,
	    BERSERK,
	    SLOW,
	    WEAK,
	    BREAK, 
	    SILENCE,
	    SLEEP,
	    BLIND,
	    STEAL_MP,
	    STEAL_HP,
	    DROPSTR,
	    DROPDEX,
	    DROPMAG,
	    ELEMENTAL_RESIST,
	    RESIST, // All magic
	    STATUS // Resistance against ANY status affect
	};
	
	 
	int modifyArmorAttribute( eAttribute attr, int current );
	float modifyArmorAttribute ( eAttribute attr, float current );
	static eAttribute attributeForString ( const std::string str );
	
    protected:
	void clearArmorEnhancers();
	void addArmorEnhancer (ArmorEnhancer * pEnhancer);
    private:
	std::list<ArmorEnhancer*> mArmorEnhancers;
    };

    class NamedItemElement : public Element
    {
    public:
	NamedItemElement();
	NamedItemElement (CL_DomElement * pElement);
	~NamedItemElement();

	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	NamedItem * getNamedItem() const;

	std::string getIconRef() const;

	uint getMaxInventory() const;
	Item::eDropRarity getDropRarity() const;
	std::string getName() const;

	

    private:
	NamedItem * mpNamedItem;
	std::string mName;
	std::string mIconRef;
	Item::eDropRarity meDropRarity;
	uint mnMaxInventory;
    };


    class NamedItem : public Item, public Element
    {
    public:
	NamedItem();
	virtual ~NamedItem();

	virtual std::string getIconRef() const;

	virtual std::string getName() const;
	virtual uint getMaxInventory() const ;
	virtual eDropRarity getDropRarity() const;

	virtual void loadItem ( CL_DomElement * pElement ) = 0;

	virtual CL_DomElement  createDomElement(CL_DomDocument&) const=0;
	
	void setIconRef(const std::string &ref);
	void setName ( const std::string &name );
	void setMaxInventory ( uint max );
	void setDropRarity( Item::eDropRarity rarity );
    private:
	std::string mName;
	std::string mIconRef;
	uint mnMaxInventory;
	Item::eDropRarity meDropRarity;
    };


    // Concrete Named Item classes

    class RegularItem: public NamedItem
    {
    public:
	RegularItem();
	virtual ~RegularItem();


	void invoke(); // Execute all actions.

	enum eUseType {BATTLE, WORLD, BOTH };
	enum eTargetable { ALL, SINGLE, EITHER, SELF_ONLY };
	enum eDefaultTarget { PARTY, MONSTERS };
	eUseType getUseType() const;
	eTargetable getTargetable() const;
	eDefaultTarget getDefaultTarget() const;
	bool isReusable() const;
	
	virtual eItemType getItemType() const { return REGULAR_ITEM; }


	virtual uint getValue() const ; // Price to buy, and worth when calculating drops.
	virtual uint getSellValue() const ;
	virtual void loadItem ( CL_DomElement * pElement );
	
	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	static eUseType UseTypeFromString ( const std::string &str );
	static eTargetable TargetableFromString ( const std::string &str );

    private:
	std::list<Action*> mActions;
	eUseType meUseType;
	eTargetable meTargetable;
	uint mnValue;
	uint mnSellValue;
	bool mbReusable;
	eDefaultTarget meDefaultTarget;
    };

    class SpecialItem : public NamedItem
    {
    public:
	SpecialItem();
	virtual ~SpecialItem();
	
	virtual uint getValue() const { return 0;} // No value to special items. cant sell 'em.
	virtual uint getSellValue() const { return 0; }

	
	// We're overriding whatever was specified in the XML. Never drop a special item, ever.
	virtual eDropRarity getDropRarity() const { return NEVER; } 

	virtual eItemType getItemType() const { return SPECIAL ; }

	virtual void loadItem ( CL_DomElement * pElement );
	
	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

    private:
    };


    class SystemItem : public NamedItem
    {
    public:
	SystemItem();
	virtual ~SystemItem();
	
	virtual uint getValue() const { return 0;} // No value to system items. cant sell 'em.
	virtual uint getSellValue() const { return 0; }

	
	// We're overriding whatever was specified in the XML. Never drop a system item, ever.
	virtual eDropRarity getDropRarity() const { return NEVER; } 

	virtual eItemType getItemType() const { return SYSTEM ; }

	virtual void loadItem ( CL_DomElement * pElement );
	
	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

    private:
    };


    class Rune : public NamedItem
    {
    public:
	Rune();
	virtual ~Rune();


	virtual uint getValue() const ;
	virtual uint getSellValue() const ;

	// We're overriding whatever was specified in the XML. Never drop a rune unless specified by the monster
	virtual eDropRarity getDropRarity() const { return NEVER; } 
	virtual eItemType getItemType() const { return RUNE ; }

	SpellRef * getSpellRef() const;

	virtual void loadItem ( CL_DomElement * pElement );
	
	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

    private:
	SpellRef *mpSpellRef;
    };


    class UniqueWeapon : public NamedItem, public Weapon
    {
    public:
	UniqueWeapon();
	~UniqueWeapon();

	virtual uint getValue() const ;
	virtual uint getSellValue() const ;


	WeaponType *getWeaponType() const ;
	bool isRanged() const ;
	
	virtual eItemType getItemType() const { return WEAPON ; }
	
	virtual void loadItem ( CL_DomElement * pElement );

	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

    private:
	
	WeaponType * mpWeaponType;

	bool mbRanged;
	uint mnValue;
	
	
    };

    class UniqueArmor : public NamedItem, public Armor
    {
    public:
	UniqueArmor();
	virtual ~UniqueArmor();

	virtual uint getValue() const ;
	virtual uint getSellValue() const ;

	ArmorType * getArmorType() const ;
	
	
	virtual eItemType getItemType() const { return ARMOR ; }

	virtual void loadItem ( CL_DomElement * pElement );

	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
	
    private:
	ArmorType *mpArmorType;

	uint mnValue;

    };


    class GeneratedWeapon : public Item, public Weapon
    {
    public:
	GeneratedWeapon();
	virtual ~GeneratedWeapon();


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
	bool isRanged() const ;


	void generate( WeaponType * pType, WeaponClass * pClass, 
		       SpellRef *pSpell = NULL, RuneType *pRune = NULL);

    private:
	std::string mName; //generated at generate time :)
	WeaponClass *mpClass;
	WeaponType *mpType;
    };


    class GeneratedArmor : public Item, public Armor
    {
    public:
	GeneratedArmor();
	virtual ~GeneratedArmor();

	virtual std::string getIconRef() const;

	virtual std::string getName() const;
	virtual uint getMaxInventory() const ;
	virtual eDropRarity getDropRarity() const;

	virtual uint getValue() const ;
	virtual uint getSellValue() const ;

	virtual eItemType getItemType() const { return WEAPON ; }

	ArmorType * getArmorType() const ;


	void generate( ArmorType * pType, ArmorClass * pClass, 
		       SpellRef *pSpell = NULL, RuneType *pRune = NULL);
	
    private:
	std::string mName;
	ArmorType  *mpType;
	ArmorClass  *mpClass;
    };




    class WeaponTypeRef : public Element
    {
    public:
	WeaponTypeRef();
	WeaponTypeRef(CL_DomElement * pElement );
	virtual ~WeaponTypeRef();

	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	std::string getName() const;

	void setName(const std::string &name) { mName = name; }

    private:
	std::string mName;
    };
    

    class WeaponClassRef : public Element
    {
    public:
	WeaponClassRef();
	WeaponClassRef(CL_DomElement *pElement);
	virtual ~WeaponClassRef();

	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	std::string getName() const;

	void setName(const std::string &name){ mName = name; }
    private:
	std::string mName;
    };


    class ArmorTypeRef : public Element
    {
    public:
	ArmorTypeRef();
	ArmorTypeRef(CL_DomElement * pElement );
	virtual ~ArmorTypeRef();

	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	std::string getName() const;

	void setName(const std::string &name){ mName = name; }

    private:
	std::string mName;
    };
    

    class ArmorClassRef : public Element
    {
    public:
	ArmorClassRef();
	ArmorClassRef(CL_DomElement *pElement);
	virtual ~ArmorClassRef();

	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	std::string getName() const;

	void setName(const std::string &name){ mName = name; }
    private:
	std::string mName;
    };


    class WeaponRef : public Element
    {
    public:
	WeaponRef();
	WeaponRef(CL_DomElement * pElement );
	~WeaponRef();

	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	WeaponType * getWeaponType() const;
	WeaponClass  * getWeaponClass() const;
	SpellRef * getSpellRef() const;
	RuneType * getRuneType() const;

    private:

	WeaponType *mpWeaponType;
	WeaponClass *mpWeaponClass;
	SpellRef * mpSpellRef;
	RuneType * mpRuneType;

    };


    class ArmorRef : public Element
    {
    public:
	ArmorRef();
	ArmorRef(CL_DomElement * pElement );
	~ArmorRef();

	virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	ArmorType * getArmorType() const;
	ArmorClass * getArmorClass() const;
	SpellRef * getSpellRef() const;
	RuneType * getRuneType() const;

    private:

	ArmorType * mpArmorType;
	ArmorClass * mpArmorClass;
	SpellRef * mpSpellRef;
	RuneType * mpRuneType;

    };


    class RuneType : public Element
    {
    public:
	RuneType();
	RuneType(CL_DomElement * pElement );
	virtual ~RuneType();

	virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

	enum eRuneType { NONE, RUNE, ULTRA_RUNE };

	eRuneType getRuneType() const;

	std::string getRuneTypeAsString() const;

	void setRuneType ( eRuneType type) { meRuneType = type; }

    private:
	eRuneType meRuneType;
    };

    class SpellRef : public Element
    {
    public:
	SpellRef();
	SpellRef( CL_DomElement * pElement );
	virtual ~SpellRef();

	enum eSpellType { ELEMENTAL, WHITE, OTHER, STATUS };

	eSpellType getSpellType() const;

	std::string getName() const;

	virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

    private:
	eSpellType meSpellType;
	std::string mName;

    };


    class WeaponEnhancer : public Element
    {
    public:
	WeaponEnhancer();
	WeaponEnhancer(CL_DomElement * pElement );
	~WeaponEnhancer();
	
	virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

	Weapon::eAttribute getAttribute() const;
	int getAdd() const;
	float getMultiplier() const;
	
    private:
	Weapon::eAttribute meAttribute;
	int mnAdd;
	float mfMultiplier;
    };


    class ArmorEnhancer : public Element
    {
    public:
	ArmorEnhancer();
	ArmorEnhancer(CL_DomElement * pElement );
	~ArmorEnhancer();
	
	virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

	Armor::eAttribute getAttribute() const;
	int getAdd() const;
	float getMultiplier() const;
	
    private:
	Armor::eAttribute meAttribute;
	int mnAdd;
	float mfMultiplier;
    };


    class AttributeEnhancer : public Element
    {
    public:
	AttributeEnhancer();
	AttributeEnhancer(CL_DomElement * pElement );
	virtual ~AttributeEnhancer();


	std::string getAttribute() const;
	int getAdd() const;
	float getMultiplier() const;

	// Uses IParty::modifyAttribute to modify the CURRENT player,
	// Meaning that the system must select the proper current player
	// when invoking. (By calling equip on the armor/weapon...)
	void invoke();

	// Uses IParty::modifyAttribute to modify the CURRENT player,
	// Meaning that the system must select the proper current player
	// when revoking. (By calling unequip on the armor/weapon...)
	void revoke();

	virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

    private:
	// Used to make sure that when we multiply the value to get it
	// back to what it was, we end up with the right values.
	int mnOriginalAttribute;
	int mnAdd;
	float mfMultiplier;
	std::string mAttribute;
	
    };


    class WeaponClass : public Element
    {
    public:
	WeaponClass();
	WeaponClass(CL_DomElement * pElement);
	~WeaponClass();

	virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

	std::string getName() const;
	int getValueAdd() const;
	float getValueMultiplier() const;

	std::list<AttributeEnhancer*>::const_iterator getAttributeEnhancersBegin();
	std::list<AttributeEnhancer*>::const_iterator getAttributeEnhancersEnd();
	
	std::list<WeaponEnhancer*>::const_iterator getWeaponEnhancersBegin();
	std::list<WeaponEnhancer*>::const_iterator getWeaponEnhancersEnd();

	bool isExcluded ( const WeaponTypeRef &weaponType );

    private:
	std::string mName;
	int mnValueAdd;
	float mfValueMultiplier;
	std::list<AttributeEnhancer*> mAttributeEnhancers;
	std::list<WeaponEnhancer*> mWeaponEnhancers;
	std::list<WeaponTypeRef*> mExcludedTypes;
    };


    class ArmorClass : public Element
    {
    public:
	ArmorClass();
	ArmorClass(CL_DomElement * pElement);
	~ArmorClass();

	virtual CL_DomElement createDomElement ( CL_DomDocument &) const;	

	std::string getName() const;
	int getValueAdd() const;
	float getValueMultiplier() const;

	std::list<AttributeEnhancer*>::const_iterator getAttributeEnhancersBegin();
	std::list<AttributeEnhancer*>::const_iterator getAttributeEnhancersEnd();
	
	std::list<ArmorEnhancer*>::const_iterator getArmorEnhancersBegin();
	std::list<ArmorEnhancer*>::const_iterator getArmorEnhancersEnd();

	bool isExcluded ( const ArmorTypeRef &weaponType );

    private:
	std::string mName;
	int mnValueAdd;
	float mfValueMultiplier;
	std::list<AttributeEnhancer*> mAttributeEnhancers;
	std::list<ArmorEnhancer*> mArmorEnhancers;
	std::list<ArmorTypeRef*> mExcludedTypes;
    };



    class WeaponType : public Element
    {
    public:
	WeaponType();
	WeaponType(CL_DomElement * pElement );
	~WeaponType();

	virtual CL_DomElement createDomElement ( CL_DomDocument &) const;	

	std::string getName() const;
	std::string getIconRef() const;

	uint getBaseAttack() const;
	float getBaseHit() const;

	uint getBasePrice() const;
	
	bool isRanged() const;

    private:
	std::string mName;
	std::string mIconRef;
	uint mnBasePrice;
	uint mnBaseAttack;
	float mfBaseHit;
	bool mbRanged;
	
    };

    class ArmorType: public Element
    {
    public:
	ArmorType();
	ArmorType(CL_DomElement * pElement );
	~ArmorType();

	virtual CL_DomElement createDomElement ( CL_DomDocument &) const;	

	std::string getName() const;
	std::string getIconRef() const;

	uint getBasePrice() const;
	int getBaseAC() const;

	enum eSlot { HEAD, BODY, SHIELD, FEET, LEFT_HAND, RIGHT_HAND };

	eSlot getSlot() const;
	
    private:
	std::string mName;
	std::string mIconRef;
	uint mnBasePrice;
	int mnBaseAC;

	eSlot meSlot;

    };

};




#endif
