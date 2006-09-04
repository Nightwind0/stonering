#ifndef SR_ARMOR_CLASS_H
#define SR_ARMOR_CLASS_H

#include "Element.h"
#include "AttributeEnhancer.h"
#include "ArmorEnhancer.h"
#include "StatusEffectModifier.h"

namespace StoneRing{
    class ArmorClass : public Element
	{
	public:
	    ArmorClass();
	    ArmorClass(CL_DomElement * pElement);
	    ~ArmorClass();
		virtual eElement whichElement() const{ return EARMORCLASS; }	
	    virtual CL_DomElement createDomElement ( CL_DomDocument &) const;       

	    std::string getName() const;
	    int getValueAdd() const;
	    float getValueMultiplier() const;

	    std::list<AttributeEnhancer*>::const_iterator getAttributeEnhancersBegin();
	    std::list<AttributeEnhancer*>::const_iterator getAttributeEnhancersEnd();
        
	    std::list<ArmorEnhancer*>::const_iterator getArmorEnhancersBegin();
	    std::list<ArmorEnhancer*>::const_iterator getArmorEnhancersEnd();

	    std::list<StatusEffectModifier*>::const_iterator getStatusEffectModifiersBegin() { return mStatusEffectModifiers.begin(); }
	    std::list<StatusEffectModifier*>::const_iterator getStatusEffectModifiersEnd() { return mStatusEffectModifiers.end(); }


	    bool isExcluded ( const ArmorTypeRef &weaponType );

	    bool operator==(const ArmorClass &lhs );

	private:
	    virtual bool handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    std::string mName;
	    int mnValueAdd;
	    float mfValueMultiplier;
	    void addStatusEffectModifier(StatusEffectModifier *pModifier ) { mStatusEffectModifiers.push_back ( pModifier ); }
	    std::list<AttributeEnhancer*> mAttributeEnhancers;
	    std::list<ArmorEnhancer*> mArmorEnhancers;
	    std::list<ArmorTypeRef*> mExcludedTypes;
	    std::list<StatusEffectModifier*> mStatusEffectModifiers;
	};
};

#endif