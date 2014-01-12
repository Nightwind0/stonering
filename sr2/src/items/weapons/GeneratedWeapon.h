#ifndef SR_GENERATED_WEAPON_H
#define SR_GENERATED_WEAPON_H



#include "Item.h"
#include "Weapon.h"

using Steel::ParameterList;
using Steel::ParameterListItem;

namespace StoneRing{
    class GeneratedWeapon : public virtual Item, public Weapon
    {
    public:
        GeneratedWeapon();
        virtual ~GeneratedWeapon();

        WeaponRef GenerateWeaponRef() const;

        // Item interface
        virtual clan::Image GetIcon() const;
        virtual std::string GetName() const;
        virtual uint GetMaxInventory() const ;
        virtual eDropRarity GetDropRarity() const;
        virtual uint GetValue() const ;
        virtual uint GetSellValue() const ;
        virtual eItemType GetItemType() const { return WEAPON ; }
		virtual clan::Sprite GetSprite() const;
        virtual void Invoke(eScriptMode invokeTime, const ParameterList& param);
        virtual bool EquipCondition(const ParameterList& param);
        virtual std::string GetDescription()const;

        // Weapon interface
        WeaponType * GetWeaponType() const;
        WeaponClass * GetWeaponClass() const { return m_pClass; }
        WeaponClass * GetImbuement() const { return m_pImbuement; }
        bool IsRanged() const ;
        bool IsTwoHanded() const;
        virtual bool operator== ( const ItemRef &ref );

        void Generate( WeaponType * pType, WeaponClass * pClass,
                       WeaponClass* pImbuement = NULL, RuneType *pRune = NULL);
		virtual std::string GetDebugId() const { return m_name; }				
		

    private:
        virtual void OnEquipScript(const ParameterList& param);
        virtual void OnUnequipScript(const ParameterList& param);
        std::string m_name; //generated at generate time :)
        WeaponClass *m_pClass;
        WeaponClass *m_pImbuement;
        WeaponType *m_pType;
	clan::Image m_icon;
    };
};

#endif



