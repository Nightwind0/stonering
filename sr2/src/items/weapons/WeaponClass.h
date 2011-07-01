#ifndef SR_WEAPON_CLASS_H
#define SR_WEAPON_CLASS_H

#include "Element.h"
#include "AttributeModifier.h"
#include "WeaponEnhancer.h"
#include "StatusEffectModifier.h"
#include <list>

namespace StoneRing{
    class WeaponClass : public Element, public SteelType::IHandle
    {
    public:
        WeaponClass();
        WeaponClass(CL_DomElement * pElement);
        virtual ~WeaponClass();
        virtual eElement WhichElement() const{ return EWEAPONCLASS; }
        std::string GetName() const;
        std::string GetDescription() const;
        int GetValueAdd() const;
        float GetValueMultiplier() const;
        bool Imbuement() const { return m_bImbuement; }
        Weapon::eScriptMode GetScriptMode() const { return m_eScriptMode; }

        std::list<AttributeModifier*>::const_iterator GetAttributeModifiersBegin();
        std::list<AttributeModifier*>::const_iterator GetAttributeModifiersEnd();

        std::list<WeaponEnhancer*>::const_iterator GetWeaponEnhancersBegin();
        std::list<WeaponEnhancer*>::const_iterator GetWeaponEnhancersEnd();

        std::list<StatusEffectModifier*>::const_iterator GetStatusEffectModifiersBegin() { return m_status_effect_modifiers.begin(); }
        std::list<StatusEffectModifier*>::const_iterator GetStatusEffectModifiersEnd() { return m_status_effect_modifiers.end(); }
        std::list<StatusEffectInfliction*>::const_iterator GetStatusEffectInflictionsBegin() { return m_status_effect_inflictions.begin(); }
        std::list<StatusEffectInfliction*>::const_iterator GetStatusEffectInflictionsEnd() { return m_status_effect_inflictions.end(); }        

        void ExecuteScript(const ParameterList& params);
        bool EquipCondition(const ParameterList& params);
        void OnEquipScript(const ParameterList& params);
        void OnUnequipScript(const ParameterList& params);

        bool IsExcluded ( const WeaponTypeRef &weaponType );
        bool IsExcluded ( const std::string &type_name );        
        bool operator==(const WeaponClass &lhs);

    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap attributes) ;
        void add_status_effect_modifier(StatusEffectModifier *pModifier ){ m_status_effect_modifiers.push_back ( pModifier ); }
        void add_status_effect_infliction(StatusEffectInfliction* pInfliction ) { m_status_effect_inflictions.push_back ( pInfliction ); }
        ScriptElement *m_pScript;
        std::string m_name;
        std::string m_desc;
        int m_nValueAdd;
        Weapon::eScriptMode m_eScriptMode;
        float m_fValueMultiplier;
        std::list<AttributeModifier*> m_attribute_modifiers;
        std::list<WeaponEnhancer*> m_weapon_enhancers;
        std::list<WeaponTypeRef*> m_excluded_types;
        std::list<StatusEffectModifier*> m_status_effect_modifiers;
        std::list<StatusEffectInfliction*> m_status_effect_inflictions;
        NamedScript *m_pEquipScript;
        NamedScript *m_pUnequipScript;
        ScriptElement *m_pConditionScript;
        bool m_bImbuement;
    };

};

#endif



