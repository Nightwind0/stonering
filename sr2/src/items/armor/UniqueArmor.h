#ifndef SR_UNIQUE_ARMOR_H
#define SR_UNIQUE_ARMOR_H

#include "NamedItem.h"
#include "Armor.h"
#include "ScriptElement.h"

namespace StoneRing{
    class UniqueArmor : public NamedItem, public Armor
    {
    public:
        UniqueArmor();
        virtual ~UniqueArmor();
        virtual eElement whichElement() const{ return EUNIQUEARMOR; }   
        virtual uint getValue() const ;
        virtual uint getSellValue() const ;

        ArmorType * getArmorType() const ;
        
        virtual void executeScript();
        virtual bool equipCondition();
        virtual eItemType getItemType() const { return ARMOR ; }
        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
        
    private:
        virtual void onEquipScript();
        virtual void onUnequipScript();
        virtual bool handleElement(eElement element, Element * pElement);
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
        virtual void loadFinished();
        ArmorType *mpArmorType;
        float mValueMultiplier;
        uint mnValue;
        ScriptElement *mpScript;
        ScriptElement *mpEquipScript;
        ScriptElement *mpUnequipScript;
        ScriptElement *mpConditionScript;
    };
};

#endif



