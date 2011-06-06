#ifndef SR_ARMOR_ENHANCER_H
#define SR_ARMOR_ENHANCER_H

#include "Element.h"
#include "Armor.h"


namespace StoneRing{
    class ArmorEnhancer : public Element
    {
    public:
        ArmorEnhancer();
        virtual ~ArmorEnhancer();
        virtual eElement WhichElement() const{ return EARMORENHANCER; }
        enum eType {
            ARMOR_ATTRIBUTE,
            DAMAGE_CATEGORY
        };
        eType GetType() const;
        Armor::eAttribute GetAttribute() const;
        DamageCategory::eDamageCategory GetDamageCategory() const;
        int GetAdd() const;
        float GetMultiplier() const;
    private:
        virtual void load_attributes(CL_DomNamedNodeMap attributes) ;
        eType m_eType;
        union {
            Armor::eAttribute m_eAttribute;
            DamageCategory::eDamageCategory m_dmgCategory;
        };
        int m_nAdd;
        float m_fMultiplier;
    };
};

#endif



