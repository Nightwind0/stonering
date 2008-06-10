#ifndef SR_WEAPON_H
#define SR_WEAPON_H

#include "Equipment.h"

namespace StoneRing{
    class Weapon : public Equipment
    {
    public:
        Weapon();
        virtual ~Weapon();
        virtual WeaponType * GetWeaponType() const = 0;
        virtual bool IsRanged() const = 0;
        virtual bool IsTwoHanded() const = 0;

        enum eAttribute
        {
            ATTACK,
            HIT,            
            STEAL_HP,
            STEAL_MP,
            CRITICAL
        };
        

        static eAttribute AttributeForString(const std::string str);     
        int ModifyWeaponAttribute( eAttribute attr, int current );
        float ModifyWeaponAttribute ( eAttribute attr, float current );

        static std::string CreateWeaponName(WeaponType *pType, WeaponClass *pClass, 
            SpellRef *pSpell, RuneType *pRune);

        // Getters for weapon enhancers. need 'em.
    protected:
        
        void Clear_Weapon_Enhancers();
        void Add_Weapon_Enhancer (WeaponEnhancer * pEnhancer);

    private:
        std::list<WeaponEnhancer*> m_weapon_enhancers;
    };


};

#endif



