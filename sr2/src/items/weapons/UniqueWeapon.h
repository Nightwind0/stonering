#ifndef SR_UNIQUE_WEAPON_H
#define SR_UNIQUE_WEAPON_H

#include "NamedItem.h"
#include "Weapon.h"
#include "ScriptElement.h"
#include "NamedScript.h"

namespace StoneRing{
    class UniqueWeapon : public NamedItem, public Weapon
    {
    public:
        UniqueWeapon();
        ~UniqueWeapon();

        virtual eElement whichElement() const{ return EUNIQUEWEAPON; }  
        virtual uint getValue() const ;
        virtual uint getSellValue() const ;

        virtual void executeScript();
        virtual bool equipCondition();
        WeaponType *getWeaponType() const ;
        bool isRanged() const ;
        bool isTwoHanded() const;
        
        virtual eItemType getItemType() const { return WEAPON ; }
    private:
        virtual void onEquipScript();
        virtual void onUnequipScript();
        virtual bool handleElement(eElement element, Element * pElement);
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
        virtual void loadFinished();
        WeaponType * mpWeaponType;
        float mValueMultiplier;
        uint mnValue;
        ScriptElement *mpScript;
        NamedScript *mpEquipScript;
        NamedScript *mpUnequipScript;
        ScriptElement *mpConditionScript;
        
    };
};
#endif



