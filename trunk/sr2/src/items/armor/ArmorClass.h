#ifndef SR_ARMOR_CLASS_H
#define SR_ARMOR_CLASS_H

#include "Element.h"
#include "ArmorEnhancer.h"
#include "StatusEffectModifier.h"

namespace StoneRing{
    class ArmorClass : public Element, public SteelType::IHandle
    {
    public:
        ArmorClass();
        ArmorClass(CL_DomElement * pElement);
        ~ArmorClass();
        virtual eElement WhichElement() const{ return EARMORCLASS; }
        std::string GetName() const;
        std::string GetDescription() const;
        int GetValueAdd() const;
        float GetValueMultiplier() const;
        bool Imbuement() const { return m_bImbuement; }

        std::list<AttributeModifier*>::const_iterator GetAttributeModifiersBegin();
        std::list<AttributeModifier*>::const_iterator GetAttributeModifiersEnd();

        std::list<ArmorEnhancer*>::const_iterator GetArmorEnhancersBegin();
        std::list<ArmorEnhancer*>::const_iterator GetArmorEnhancersEnd();

        std::list<StatusEffectModifier*>::const_iterator GetStatusEffectModifiersBegin() { return m_status_effect_modifiers.begin(); }
        std::list<StatusEffectModifier*>::const_iterator GetStatusEffectModifiersEnd() { return m_status_effect_modifiers.end(); }
        std::list<StatusEffectInfliction*>::const_iterator GetStatusEffectInflictionsBegin() { return m_status_effect_inflictions.begin(); }
        std::list<StatusEffectInfliction*>::const_iterator GetStatusEffectInflictionsEnd() { return m_status_effect_inflictions.end(); }

        void ExecuteScript(const ParameterList& params);
        bool EquipCondition(const ParameterList& params);
        void OnEquipScript(const ParameterList& params);
        void OnUnequipScript(const ParameterList& params);

        bool IsExcluded ( const std::string& armorType );
        bool IsExcluded ( const ArmorTypeRef& armorType );
        bool operator==(const ArmorClass &lhs );

    private:

        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap attributes) ;
        std::string m_name;
        std::string m_desc;
        int m_nValueAdd;
        float m_fValueMultiplier;
        bool m_bImbuement;
        void AddStatusEffectModifier(StatusEffectModifier* pModifier ) { m_status_effect_modifiers.push_back ( pModifier ); }
        void AddStatusEffectInfliction(StatusEffectInfliction* pInfliction ) { m_status_effect_inflictions.push_back (pInfliction); } 
        ScriptElement *m_pScript;
        std::list<AttributeModifier*> m_attribute_modifiers;
        std::list<ArmorEnhancer*> m_armor_enhancers;
        std::list<ArmorTypeRef*> m_excluded_types;
        std::list<StatusEffectModifier*> m_status_effect_modifiers;
        std::list<StatusEffectInfliction*> m_status_effect_inflictions;
        NamedScript *m_pEquipScript;
        NamedScript *m_pUnequipScript;
        ScriptElement *m_pConditionScript;
    };
    
};

#endif



