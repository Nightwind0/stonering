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

        Armor::eAttribute GetAttribute() const;
        float GetAdd() const;
        float GetMultiplier() const;
		virtual std::string GetDebugId() const { return IntToString(m_eAttribute); }				
    private:
        virtual void load_attributes(clan::DomNamedNodeMap attributes) ;

        Armor::eAttribute m_eAttribute;
        float m_fAdd;
        float m_fMultiplier;
    };
};

#endif



