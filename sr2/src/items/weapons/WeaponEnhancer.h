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
        virtual eElement whichElement() const{ return EWEAPONENHANCER; }          
        virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

        Weapon::eAttribute getAttribute() const;
        int getAdd() const;
        float getMultiplier() const;
        
    private:
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
        Weapon::eAttribute meAttribute;
        int mnAdd;
        float mfMultiplier;
    };
};

#endif

