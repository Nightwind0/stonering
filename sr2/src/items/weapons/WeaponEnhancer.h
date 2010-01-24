#ifndef SR_WEAPONENHANCER_H
#define SR_WEAPONENHANCER_H

#include "Element.h"
#include "Weapon.h"

namespace StoneRing{
    class WeaponEnhancer : public Element
    {
    public:
        WeaponEnhancer();
        ~WeaponEnhancer();
        virtual eElement WhichElement() const{ return EWEAPONENHANCER; }
        Weapon::eAttribute GetAttribute() const;
        int GetAdd() const;
        float GetMultiplier() const;

    private:
        virtual void load_attributes(CL_DomNamedNodeMap attributes) ;
        Weapon::eAttribute m_eAttribute;
        float m_fMultiplier;
        int m_nAdd;

    };
};

#endif



