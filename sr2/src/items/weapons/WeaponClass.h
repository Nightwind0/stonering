#ifndef SR_WEAPON_CLASS_H
#define SR_WEAPON_CLASS_H

#include "Element.h"
#include "AttributeEnhancer.h"
#include "WeaponEnhancer.h"
#include "StatusEffectModifier.h"
#include <list>

namespace StoneRing{
    class WeaponClass : public Element
    {
    public:
        WeaponClass();
        WeaponClass(CL_DomElement * pElement);
        ~WeaponClass();
        virtual eElement whichElement() const{ return EWEAPONCLASS; }   
        virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

        std::string getName() const;
        int getValueAdd() const;
        float getValueMultiplier() const;

        std::list<AttributeEnhancer*>::const_iterator getAttributeEnhancersBegin();
        std::list<AttributeEnhancer*>::const_iterator getAttributeEnhancersEnd();
        
        std::list<WeaponEnhancer*>::const_iterator getWeaponEnhancersBegin();
        std::list<WeaponEnhancer*>::const_iterator getWeaponEnhancersEnd();

        std::list<StatusEffectModifier*>::const_iterator getStatusEffectModifiersBegin() { return mStatusEffectModifiers.begin(); }
        std::list<StatusEffectModifier*>::const_iterator getStatusEffectModifiersEnd() { return mStatusEffectModifiers.end(); }


        bool isExcluded ( const WeaponTypeRef &weaponType );

        bool operator==(const WeaponClass &lhs);

    private:
        virtual bool handleElement(eElement element, Element * pElement );
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
        void addStatusEffectModifier(StatusEffectModifier *pModifier ){ mStatusEffectModifiers.push_back ( pModifier ); }
        std::string mName;
        int mnValueAdd;
        float mfValueMultiplier;
        std::list<AttributeEnhancer*> mAttributeEnhancers;
        std::list<WeaponEnhancer*> mWeaponEnhancers;
        std::list<WeaponTypeRef*> mExcludedTypes;
        std::list<StatusEffectModifier*> mStatusEffectModifiers;
    };
};

#endif

