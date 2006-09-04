#ifndef SR_WEAPON_TYPE_H
#define SR_WEAPON_TYPE_H



#include "Element.h"
#include "DamageCategory.h"

namespace StoneRing{
    class WeaponType : public Element
	{
	public:
	    WeaponType();
	    WeaponType(CL_DomElement * pElement );
	    ~WeaponType();

		virtual eElement whichElement() const{ return EWEAPONTYPE; }	
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
	    virtual bool handleElement(eElement element, Element * pElement );
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
};

#endif