#ifndef SR_WEAPON_H
#define SR_WEAPON_H

#include "Equipment.h"

namespace StoneRing{
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
};

#endif

