#ifndef SR_UNIQUE_ARMOR_H
#define SR_UNIQUE_ARMOR_H

#include "NamedItem.h"
#include "Armor.h"
#include "ScriptElement.h"

namespace StoneRing{
    class NamedScript;

    class UniqueArmor : public NamedItem, public Armor
    {
    public:
        UniqueArmor();
        virtual ~UniqueArmor();
        virtual eElement WhichElement() const{ return EUNIQUEARMOR; }   
        virtual uint GetValue() const ;
        virtual uint GetSellValue() const ;

        ArmorType * GetArmorType() const ;
        
        virtual void ExecuteScript();
        virtual bool EquipCondition();
        virtual eItemType GetItemType() const { return ARMOR ; }
        
    private:
        virtual void OnEquipScript();
        virtual void OnUnequipScript();
        virtual bool handle_element(eElement element, Element * pElement);
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes) ;
        virtual void load_finished();
        ArmorType *m_pArmorType;
        float m_value_multiplier;
        uint m_nValue;
        ScriptElement *m_pScript;
        NamedScript *m_pEquipScript;
        NamedScript *m_pUnequipScript;
        ScriptElement *m_pConditionScript;
    };
};

#endif



