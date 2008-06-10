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
        int GetAdd() const;
        float GetMultiplier() const;
    private:
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes) ;
        Armor::eAttribute m_eAttribute;
        int m_nAdd;
        float m_fMultiplier;
    };
};

#endif



