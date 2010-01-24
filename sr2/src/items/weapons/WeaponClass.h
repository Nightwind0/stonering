#ifndef SR_WEAPON_CLASS_H
#define SR_WEAPON_CLASS_H

#include "Element.h"
#include "AttributeModifier.h"
#include "WeaponEnhancer.h"
#include "StatusEffectModifier.h"
#include <list>

namespace StoneRing{
    class WeaponClass : public Element
    {
    public:
        WeaponClass();
        WeaponClass(CL_DomElement * pElement);
        ~WeaponClass();
        virtual eElement WhichElement() const{ return EWEAPONCLASS; }
        std::string GetName() const;
        int GetValueAdd() const;
        float GetValueMultiplier() const;
        Weapon::eScriptMode GetScriptMode() const { return m_eScriptMode; }

        std::list<AttributeModifier*>::const_iterator GetAttributeModifiersBegin();
        std::list<AttributeModifier*>::const_iterator GetAttributeModifiersEnd();

        std::list<WeaponEnhancer*>::const_iterator GetWeaponEnhancersBegin();
        std::list<WeaponEnhancer*>::const_iterator GetWeaponEnhancersEnd();

        std::list<StatusEffectModifier*>::const_iterator GetStatusEffectModifiersBegin() { return m_status_effect_modifiers.begin(); }
        std::list<StatusEffectModifier*>::const_iterator GetStatusEffectModifiersEnd() { return m_status_effect_modifiers.end(); }

        void ExecuteScript();
        bool EquipCondition();
        void OnEquipScript();
        void OnUnequipScript();

        bool IsExcluded ( const WeaponTypeRef &weaponType );
        bool operator==(const WeaponClass &lhs);

    private:

        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap attributes) ;
        void add_status_effect_modifier(StatusEffectModifier *pModifier ){ m_status_effect_modifiers.push_back ( pModifier ); }
        ScriptElement *m_pScript;
        std::string m_name;
        int m_nValueAdd;
        Weapon::eScriptMode m_eScriptMode;
        float m_fValueMultiplier;
        std::list<AttributeModifier*> m_attribute_modifiers;
        std::list<WeaponEnhancer*> m_weapon_enhancers;
        std::list<WeaponTypeRef*> m_excluded_types;
        std::list<StatusEffectModifier*> m_status_effect_modifiers;
        NamedScript *m_pEquipScript;
        NamedScript *m_pUnequipScript;
        ScriptElement *m_pConditionScript;
    };
};

#endif



