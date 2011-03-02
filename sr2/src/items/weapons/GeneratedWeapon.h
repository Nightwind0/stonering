#ifndef SR_GENERATED_WEAPON_H
#define SR_GENERATED_WEAPON_H



#include "Item.h"
#include "Weapon.h"

namespace StoneRing{
    class GeneratedWeapon : public virtual Item, public Weapon
    {
    public:
        GeneratedWeapon();
        virtual ~GeneratedWeapon();

        WeaponRef GenerateWeaponRef() const;

        // Item interface
        virtual CL_Image GetIcon() const;
        virtual std::string GetName() const;
        virtual uint GetMaxInventory() const ;
        virtual eDropRarity GetDropRarity() const;
        virtual uint GetValue() const ;
        virtual uint GetSellValue() const ;
        virtual eItemType GetItemType() const { return WEAPON ; }

        virtual void Invoke();
        virtual bool EquipCondition();

        // Weapon interface
        WeaponType * GetWeaponType() const;
        WeaponClass * GetWeaponClass() const { return m_pClass; }
        bool IsRanged() const ;
        bool IsTwoHanded() const;
        virtual bool operator== ( const ItemRef &ref );

        void Generate( WeaponType * pType, WeaponClass * pClass,
                       SpellRef *pSpell = NULL, RuneType *pRune = NULL);

    private:
        virtual void OnEquipScript();
        virtual void OnUnequipScript();
        std::string m_name; //generated at generate time :)
        WeaponClass *m_pClass;
        WeaponType *m_pType;
	CL_Image m_icon;
    };
};

#endif



