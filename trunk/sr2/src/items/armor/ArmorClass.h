#ifndef SR_ARMOR_CLASS_H
#define SR_ARMOR_CLASS_H

#include "Element.h"
#include "ArmorEnhancer.h"
#include "StatusEffectModifier.h"

namespace StoneRing{
    class ArmorClass : public Element
    {
    public:
        ArmorClass();
        ArmorClass(CL_DomElement * pElement);
        ~ArmorClass();
        virtual eElement WhichElement() const{ return EARMORCLASS; }    
        std::string GetName() const;
        int GetValueAdd() const;
        float GetValueMultiplier() const;

        std::list<AttributeModifier*>::const_iterator GetAttributeModifiersBegin();
        std::list<AttributeModifier*>::const_iterator GetAttributeModifiersEnd();
        
        std::list<ArmorEnhancer*>::const_iterator GetArmorEnhancersBegin();
        std::list<ArmorEnhancer*>::const_iterator GetArmorEnhancersEnd();

        std::list<StatusEffectModifier*>::const_iterator GetStatusEffectModifiersBegin() { return m_status_effect_modifiers.begin(); }
        std::list<StatusEffectModifier*>::const_iterator GetStatusEffectModifiersEnd() { return m_status_effect_modifiers.end(); }

        void ExecuteScript();
        bool EquipCondition();
        void OnEquipScript();
        void OnUnequipScript();

        bool IsExcluded ( const ArmorTypeRef &weaponType );
        bool operator==(const ArmorClass &lhs );

    private:

        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes) ;
        std::string m_name;
        int m_nValueAdd;
        float m_fValueMultiplier;
        void AddStatusEffectModifier(StatusEffectModifier *pModifier ) { m_status_effect_modifiers.push_back ( pModifier ); }
        ScriptElement *m_pScript;
        std::list<AttributeModifier*> m_attribute_modifiers;
        std::list<ArmorEnhancer*> m_armor_enhancers;
        std::list<ArmorTypeRef*> m_excluded_types;
        std::list<StatusEffectModifier*> m_status_effect_modifiers;
        NamedScript *m_pEquipScript;
        NamedScript *m_pUnequipScript;
        ScriptElement *m_pConditionScript;
    };
};

#endif



