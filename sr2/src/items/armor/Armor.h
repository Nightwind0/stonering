#ifndef SR_ARMOR_H
#define SR_ARMOR_H

#include "Equipment.h"
#include "DamageCategory.h"

namespace StoneRing{

    class Armor : public Equipment
    {
    public:
        Armor();
        virtual ~Armor();

        virtual ArmorType *GetArmorType() const = 0;

        enum eAttribute
        {
            AC,
            RST,
            STEAL_MP,
            STEAL_HP,
            CHANGE_BP,
            STATUS // Chance of failure for a particular status effect
        };

        virtual bool IsArmor() const { return true; }

        /*
        * These are based on the armor type, and any enhancers added by the class
        * or the unique enhancements.
        * example, if the ArmorType's baseResist is 100, and this particular armor
        * has a 1.5 multiplier and +7 add, this will return 157
        */
        double GetArmorAttribute ( eAttribute attr );
        
        double GetResistance ( DamageCategory::eDamageCategory category );
        //int GetResistanceAdd ( DamageCategory::eDamageCategory category );

        static eAttribute AttributeForString ( const std::string str );
        static std::string CreateArmorName(ArmorType *pType, ArmorClass *pClass,
            SpellRef *pSpell, RuneType *pRune);
    protected:
        void Clear_Armor_Enhancers();
        void Add_Armor_Enhancer (ArmorEnhancer * pEnhancer);
    private:
        std::list<ArmorEnhancer*> m_armor_enhancers;
    };

}

#endif



