#ifndef SR_UNIQUE_ARMOR_H
#define SR_UNIQUE_ARMOR_H

#include "NamedItem.h"
#include "Armor.h"
#include "ScriptElement.h"

namespace StoneRing{
    class NamedScript;

    class UniqueArmor : public NamedItemElement, public Armor
    {
    public:
        UniqueArmor();
        virtual ~UniqueArmor();
        virtual eElement WhichElement() const{ return EUNIQUEARMOR; }
        virtual uint GetValue() const ;
        virtual uint GetSellValue() const ;
		virtual std::string GetName() const { return NamedItemElement::GetName(); }
		virtual CL_Image GetIcon() const { return NamedItemElement::GetIcon(); }
		virtual uint GetMaxInventory() const { return NamedItemElement::GetMaxInventory(); }
		virtual eDropRarity GetDropRarity() const { return NamedItemElement::GetDropRarity(); }
		virtual std::string GetDescription() const { return m_description; }

        ArmorType * GetArmorType() const ;

        virtual void Invoke(const ParameterList& params);
        virtual bool EquipCondition(const ParameterList& params);
        virtual eItemType GetItemType() const { return ARMOR ; }

		virtual bool operator==(const ItemRef& ref);
    private:
        virtual void OnEquipScript(const ParameterList& params);
        virtual void OnUnequipScript(const ParameterList& params);
        virtual bool handle_element(eElement element, Element * pElement);
        virtual void load_attributes(CL_DomNamedNodeMap attributes) ;
        virtual void load_finished();
        ArmorType *m_pArmorType;
        float m_value_multiplier;
        uint m_nValue;
        std::string m_description;
        ScriptElement *m_pScript;
        NamedScript *m_pEquipScript;
        NamedScript *m_pUnequipScript;
        ScriptElement *m_pConditionScript;
    };
};

#endif



