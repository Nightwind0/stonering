#ifndef SR_ARMOR_H
#define SR_ARMOR_H

#include "Equipment.h"

namespace StoneRing{

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
};

#endif

