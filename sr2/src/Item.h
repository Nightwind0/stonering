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

    class Equipment : public virtual Item
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

	    std::list<StatusEffectModifier*>::const_iterator getStatusEffectModifiersBegin() const { return mStatusEffectModifiers.begin(); }
	    std::list<StatusEffectModifier*>::const_iterator getStatusEffectModifiersEnd() const { return mStatusEffectModifiers.end(); }
	protected:
	    void clearAttributeEnhancers();
	    void addAttributeEnhancer( AttributeEnhancer * pAttr );
	    void setSpellRef ( SpellRef * pRef );
	    void setRuneType ( RuneType * pType );
	    void addStatusEffectModifier(StatusEffectModifier *pModifier) { mStatusEffectModifiers.push_back ( pModifier ) ; }
	
	private:
	    std::list<AttributeEnhancer*> mAttributeEnhancers;
	    SpellOrRuneRef  mSpellOrRuneRef;
	    enum eMagic { NONE, SPELL, RUNE };
	    eMagic meMagic;
	    std::list<StatusEffectModifier*> mStatusEffectModifiers;
        
	};


    class Weapon : public Equipment
	{
	public:
	    Weapon();
	    virtual ~Weapon();

	    virtual WeaponType * getWeaponType() const = 0;
	    virtual bool isRanged() const = 0;
	    virtual bool isTwoHanded() const = 0;

	    enum eAttribute
		{
		    ATTACK,
		    HIT,            
		    STEAL_HP,
		    STEAL_MP,
		    CRITICAL
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
            
		    STEAL_MP,
		    STEAL_HP,
		    ELEMENTAL_RESIST, // Your AC for elemental magic
		    SLASH_AC, // Extra AC against slash attacks (Multiplier)
		    JAB_AC, // Extra AC against jab attacks (Multiplier)
		    BASH_AC, // Extra AC against bash attacks (Multiplier)
		    RESIST, // Resist is your AC for magic attacks
		    WHITE_RESIST, // Your AC against white magic. (hey, its a valid type!)
		    STATUS, // Chance of failure for a particular status effect
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
	    ~NamedItemElement();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	    NamedItem * getNamedItem() const;

	    std::string getIconRef() const;

	    uint getMaxInventory() const;
	    Item::eDropRarity getDropRarity() const;
	    std::string getName() const;

        

	private:
	    virtual void handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
		virtual void loadFinished();
	    NamedItem * mpNamedItem;
	    std::string mName;
	    std::string mIconRef;
	    Item::eDropRarity meDropRarity;
	    uint mnMaxInventory;
	};


    class NamedItem : public virtual Item, public Element
	{
	public:
	    NamedItem();
	    virtual ~NamedItem();
		
	    std::string getIconRef() const;

	    virtual std::string getName() const;
	    virtual uint getMaxInventory() const ;
	    virtual eDropRarity getDropRarity() const;

             
	    void setIconRef(const std::string &ref);
	    void setName ( const std::string &name );
	    void setMaxInventory ( uint max );
	    void setDropRarity( Item::eDropRarity rarity );

	    virtual bool operator== ( const ItemRef &ref );

	private:
	    std::string mName;
	    std::string mIconRef;
	    uint mnMaxInventory;
	    Item::eDropRarity meDropRarity;
	};

	class IconRef : public Element
	{
	public:
		IconRef();
		virtual ~IconRef();

		std::string getIcon() const;
		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;	

	private:
	    virtual void handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
		virtual void handleText(const std::string &text);
		std::string mIcon;
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
	    virtual void handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
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

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	private:
	    virtual void handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
		virtual void loadFinished();
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
	    bool isTwoHanded() const;
        
	    virtual eItemType getItemType() const { return WEAPON ; }
 
	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	private:
	    virtual void handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
		virtual void loadFinished();
	    WeaponType * mpWeaponType;
		float mValueMultiplier;
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
	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
        
	private:
	    virtual void handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
		virtual void loadFinished();
	    ArmorType *mpArmorType;
		float mValueMultiplier;
	    uint mnValue;

	};


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




    class WeaponTypeRef : public Element
	{
	public:
	    WeaponTypeRef();
	    virtual ~WeaponTypeRef();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	    std::string getName() const;

	    void setName(const std::string &name) { mName = name; }

	    bool operator== ( const WeaponTypeRef &lhs );
	private:
		virtual void handleText(const std::string &text);
	    std::string mName;
	};
    

    class WeaponClassRef : public Element
	{
	public:
	    WeaponClassRef();
	    virtual ~WeaponClassRef();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	    std::string getName() const;

	    void setName(const std::string &name){ mName = name; }
	    bool operator== (const WeaponClassRef &lhs );
	private:
		virtual void handleText(const std::string &text);
	    std::string mName;
	};


    class ArmorTypeRef : public Element
	{
	public:
	    ArmorTypeRef();
	    virtual ~ArmorTypeRef();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	    std::string getName() const;

	    void setName(const std::string &name){ mName = name; }
	    bool operator==(const ArmorTypeRef &lhs );
	private:
		virtual void handleText(const std::string &text);
	    std::string mName;
	};
    

    class ArmorClassRef : public Element
	{
	public:
	    ArmorClassRef();
	    virtual ~ArmorClassRef();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	    std::string getName() const;

	    void setName(const std::string &name){ mName = name; }
	    bool operator==(const ArmorClassRef &lhs );
	private:
		virtual void handleText(const std::string &text);
	    std::string mName;
	};


    class WeaponRef : public Element
	{
	public:
	    WeaponRef();
	    WeaponRef ( WeaponType *pType, WeaponClass *pClass, 
			SpellRef * pSpell, RuneType *pRune );
	    virtual ~WeaponRef();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	    WeaponType * getWeaponType() const;
	    WeaponClass  * getWeaponClass() const;
	    SpellRef * getSpellRef() const;
	    RuneType * getRuneType() const;

	    bool operator==(const WeaponRef &lhs);

	private:
	    virtual void handleElement(eElement element, Element * pElement );

	    WeaponType *mpWeaponType;
	    WeaponClass *mpWeaponClass;
	    WeaponTypeRef mType;
	    WeaponClassRef mClass;
	    SpellRef * mpSpellRef;
	    RuneType * mpRuneType;

	};


    class ArmorRef : public Element
	{
	public:
	    ArmorRef();
	    ArmorRef ( ArmorType *pType, ArmorClass *pClass, 
		       SpellRef * pSpell, RuneType *pRune );

	    ~ArmorRef();

	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	    ArmorType * getArmorType() const;
	    ArmorClass * getArmorClass() const;
	    SpellRef * getSpellRef() const;
	    RuneType * getRuneType() const;

	    bool operator==(const ArmorRef &lhs );

	private:
	    virtual void handleElement(eElement element, Element * pElement );

	    ArmorType * mpArmorType;
	    ArmorClass * mpArmorClass;
	    ArmorTypeRef mType;
	    ArmorClassRef mClass;
	    SpellRef * mpSpellRef;
	    RuneType * mpRuneType;

	};


    class RuneType : public Element
	{
	public:
	    RuneType();
	    virtual ~RuneType();

	    virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

	    enum eRuneType { NONE, RUNE, ULTRA_RUNE };

	    eRuneType getRuneType() const;

	    std::string getRuneTypeAsString() const;

	    void setRuneType ( eRuneType type) { meRuneType = type; }

	    bool operator==(const RuneType &lhs);

	private:
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    eRuneType meRuneType;
	};

    class SpellRef : public Element
	{
	public:
	    SpellRef();
	    virtual ~SpellRef();

	    enum eSpellType { ELEMENTAL, WHITE, OTHER, STATUS };

	    eSpellType getSpellType() const;

	    std::string getName() const;

	    virtual CL_DomElement createDomElement ( CL_DomDocument &) const;
        
	    bool operator==(const SpellRef &lhs);

	    void setType(eSpellType type){ meSpellType = type; }
	    void setName(const std::string &name){ mName = name; }

	private:
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
		virtual void handleText(const std::string &text);
	    eSpellType meSpellType;
	    std::string mName;

	};


    class WeaponEnhancer : public Element
	{
	public:
	    WeaponEnhancer();
	    ~WeaponEnhancer();
        
	    virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

	    Weapon::eAttribute getAttribute() const;
	    int getAdd() const;
	    float getMultiplier() const;
        
	private:
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    Weapon::eAttribute meAttribute;
	    int mnAdd;
	    float mfMultiplier;
	};


    class ArmorEnhancer : public Element
	{
	public:
	    ArmorEnhancer();
	    ~ArmorEnhancer();
        
	    virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

	    Armor::eAttribute getAttribute() const;
	    int getAdd() const;
	    float getMultiplier() const;
        
	private:
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    Armor::eAttribute meAttribute;
	    int mnAdd;
	    float mfMultiplier;
	};


    class AttributeEnhancer : public Element
	{
	public:
	    AttributeEnhancer();
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
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    // Used to make sure that when we multiply the value to get it
	    // back to what it was, we end up with the right values.
	    int mnDelta;
	    int mnAdd;
	    float mfMultiplier;
	    std::string mAttribute;
        
	};

    class WeaponTypeExclusionList: public Element
	{
	public:
	    WeaponTypeExclusionList();
	    virtual ~WeaponTypeExclusionList();
	    virtual CL_DomElement createDomElement ( CL_DomDocument &) const;


	    std::list<WeaponTypeRef*>::const_iterator getWeaponTypeRefsBegin();
	    std::list<WeaponTypeRef*>::const_iterator getWeaponTypeRefsEnd();

	    virtual void handleElement(eElement element, Element * pElement);
	private:
	    std::list<WeaponTypeRef*> mWeaponTypes;

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

	    std::list<StatusEffectModifier*>::const_iterator getStatusEffectModifiersBegin() { return mStatusEffectModifiers.begin(); }
	    std::list<StatusEffectModifier*>::const_iterator getStatusEffectModifiersEnd() { return mStatusEffectModifiers.end(); }


	    bool isExcluded ( const WeaponTypeRef &weaponType );

	    bool operator==(const WeaponClass &lhs);

	private:
	    virtual void handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    void addStatusEffectModifier(StatusEffectModifier *pModifier ){ mStatusEffectModifiers.push_back ( pModifier ); }
	    std::string mName;
	    int mnValueAdd;
	    float mfValueMultiplier;
	    std::list<AttributeEnhancer*> mAttributeEnhancers;
	    std::list<WeaponEnhancer*> mWeaponEnhancers;
	    std::list<WeaponTypeRef*> mExcludedTypes;
	    std::list<StatusEffectModifier*> mStatusEffectModifiers;
	};

	class ArmorTypeExclusionList: public Element
	{
	public:
	    ArmorTypeExclusionList();
	    virtual ~ArmorTypeExclusionList();
	    virtual CL_DomElement createDomElement ( CL_DomDocument &) const;


	    std::list<ArmorTypeRef*>::const_iterator getArmorTypeRefsBegin();
	    std::list<ArmorTypeRef*>::const_iterator getArmorTypeRefsEnd();

	    virtual void handleElement(eElement element, Element * pElement);
	private:
	    std::list<ArmorTypeRef*> mArmorTypes;

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

	    std::list<StatusEffectModifier*>::const_iterator getStatusEffectModifiersBegin() { return mStatusEffectModifiers.begin(); }
	    std::list<StatusEffectModifier*>::const_iterator getStatusEffectModifiersEnd() { return mStatusEffectModifiers.end(); }


	    bool isExcluded ( const ArmorTypeRef &weaponType );

	    bool operator==(const ArmorClass &lhs );

	private:
	    virtual void handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    std::string mName;
	    int mnValueAdd;
	    float mfValueMultiplier;
	    void addStatusEffectModifier(StatusEffectModifier *pModifier ) { mStatusEffectModifiers.push_back ( pModifier ); }
	    std::list<AttributeEnhancer*> mAttributeEnhancers;
	    std::list<ArmorEnhancer*> mArmorEnhancers;
	    std::list<ArmorTypeRef*> mExcludedTypes;
	    std::list<StatusEffectModifier*> mStatusEffectModifiers;
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
	    float getBaseCritical() const;

	    uint getBasePrice() const;
        
	    bool isRanged() const;

	    bool isTwoHanded() const;

	    DamageCategory *getDamageCategory () const { return mpDamageCategory; }

	    bool operator==(const WeaponType &lhs);

	private:
	    virtual void handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    DamageCategory * mpDamageCategory;
	    std::string mName;
	    std::string mIconRef;
	    uint mnBasePrice;
	    uint mnBaseAttack;
	    float mfBaseHit;
	    float mfBaseCritical;
	    bool mbRanged;
	    bool mbTwoHanded;
        
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
	    int getBaseRST() const;

	    enum eSlot { HEAD, BODY, SHIELD, FEET, HANDS };

	    eSlot getSlot() const;

	    bool operator==(const ArmorType &lhs );
        
	private:
	    virtual void handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    std::string mName;
	    std::string mIconRef;
	    uint mnBasePrice;
	    int mnBaseAC;
	    int mnBaseRST;
	    eSlot meSlot;

	};

    class DamageCategory
	{
	public:
	    DamageCategory(){}
	    virtual ~DamageCategory(){}

	    enum eClass { WEAPON, MAGIC };

	    virtual eClass getClass() const=0;
	private:
	};

    class WeaponDamageCategory : public Element, public DamageCategory
	{
	public:
	    WeaponDamageCategory();
	    WeaponDamageCategory(CL_DomElement *pElement);
	    virtual ~WeaponDamageCategory();

	    virtual eClass getClass() const { return WEAPON; }

	    enum eType { SLASH, BASH, JAB };

	    eType getType() const;

	    CL_DomElement createDomElement(CL_DomDocument&) const;
	private:
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    eType TypeFromString( const std::string &str );
	    eType meType;
	};

    class MagicDamageCategory : public Element, public DamageCategory
	{
	public:
	    MagicDamageCategory();
	    MagicDamageCategory(CL_DomElement *pElement);
	    virtual ~MagicDamageCategory();

	    virtual eClass getClass() const { return MAGIC; }

	    enum eType { FIRE, EARTH, WIND, WATER, HOLY, OTHER };

	    eType getType() const;

	    CL_DomElement createDomElement(CL_DomDocument&) const;
	private:
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    eType TypeFromString( const std::string &str );
	    eType meType;
	};

    class StatusEffectModifier : public Element
	{
	public:
	    StatusEffectModifier();
	    StatusEffectModifier(CL_DomElement *pElement);
	    virtual ~StatusEffectModifier();

	    StatusEffect * getStatusEffect() const;
	    float getModifier() const;
	    

	    CL_DomElement createDomElement(CL_DomDocument&) const;

	private:
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    StatusEffect *mpStatusEffect;
	    float mfModifier;
	};

};




#endif
