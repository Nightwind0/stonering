#ifndef SR_UNIQUE_WEAPON_H
#define SR_UNIQUE_WEAPON_H

#include "NamedItem.h"
#include "Weapon.h"
#include "ScriptElement.h"
#include "NamedScript.h"

namespace StoneRing{
    class Animation;
    class UniqueWeapon : public NamedItemElement, public Weapon
    {
    public:
        UniqueWeapon();
        ~UniqueWeapon();

        virtual eElement WhichElement() const{ return EUNIQUEWEAPON; }
        virtual uint GetValue() const ;
        virtual uint GetSellValue() const ;

        virtual void Invoke();
        virtual bool EquipCondition();
        WeaponType *GetWeaponType() const ;
        bool IsRanged() const ;
        bool IsTwoHanded() const;
	virtual std::string GetName() const { return NamedItemElement::GetName(); }
	virtual CL_Image GetIcon() const { return NamedItemElement::GetIcon(); }
	virtual uint GetMaxInventory() const { return NamedItemElement::GetMaxInventory(); }
	virtual eDropRarity GetDropRarity() const { return NamedItemElement::GetDropRarity(); }

        virtual eItemType GetItemType() const { return WEAPON ; }
        virtual bool operator == ( const ItemRef &ref );
    private:
        virtual void OnEquipScript();
        virtual void OnUnequipScript();
        virtual bool handle_element(eElement element, Element * pElement);
        virtual void load_attributes(CL_DomNamedNodeMap attributes) ;
        virtual void load_finished();
        WeaponType * m_pWeaponType;
        float m_value_multiplier;
        uint m_nValue;
        ScriptElement *m_pScript;
        NamedScript *m_pEquipScript;
        NamedScript *m_pUnequipScript;
        ScriptElement *m_pConditionScript;
	};
};
#endif



