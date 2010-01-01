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

        virtual bool IsArmor() const { return false; }
        /*
        virtual void Equip(ICharacter *);
        virtual void Unequip(ICharacter *);
        */
#if 1 // Disabling weapon attribute, why not just have them modify the characters attributes?
        enum eAttribute
        {
            ATTACK,
            HIT,
            STEAL_HP, // Needs some other way to do this
            STEAL_MP,
            CHANGE_BP,
            CRITICAL
        };


        static eAttribute AttributeForString(const std::string str);

        /*
        * These are based on the weapon type, and any enhancers added by the class
        * or the unique enhancements.
        * example, if the WeaponType's baseAttack is 100, and this particular weapon
        * has a 1.5 multiplier and +7 add, this will return 157
        */
        double GetWeaponAttribute ( eAttribute attr );
#endif


        static std::string CreateWeaponName(WeaponType *pType, WeaponClass *pClass,
            SpellRef *pSpell, RuneType *pRune);

        // Getters for weapon enhancers. need 'em.
    protected:

        void Clear_Weapon_Enhancers();
        void Add_Weapon_Enhancer (WeaponEnhancer * pEnhancer);

    private:
        std::list<WeaponEnhancer*> m_weapon_enhancers;
    };


}

#endif



