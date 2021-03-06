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
        float GetAdd() const;
        float GetMultiplier() const;
		virtual std::string GetDebugId() const { return IntToString(m_eAttribute); }				

    private:
        virtual void load_attributes(clan::DomNamedNodeMap attributes) ;
        Weapon::eAttribute m_eAttribute;
        float m_fMultiplier;
        float m_fAdd;

    };
};

#endif



