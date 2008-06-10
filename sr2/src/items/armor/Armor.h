#ifndef SR_ARMOR_H
#define SR_ARMOR_H

#include "Equipment.h"

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
        int ModifyArmorAttribute( eAttribute attr, int current );
        float ModifyArmorAttribute ( eAttribute attr, float current );
        static eAttribute AttributeForString ( const std::string str );

        static std::string CreateArmorName(ArmorType *pType, ArmorClass *pClass, 
            SpellRef *pSpell, RuneType *pRune);
    protected:
        void Clear_Armor_Enhancers();
        void Add_Armor_Enhancer (ArmorEnhancer * pEnhancer);
    private:
        std::list<ArmorEnhancer*> m_armor_enhancers;
    };

};

#endif



