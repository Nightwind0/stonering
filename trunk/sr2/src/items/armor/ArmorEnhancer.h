#ifndef SR_ARMOR_ENHANCER_H
#define SR_ARMOR_ENHANCER_H

#include "Element.h"
#include "Armor.h"


namespace StoneRing{
    class ArmorEnhancer : public Element
	{
	public:
	    ArmorEnhancer();
	    ~ArmorEnhancer();
		virtual eElement whichElement() const{ return EARMORENHANCER; }	        
	    virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

	    Armor::eAttribute getAttribute() const;
	    int getAdd() const;
	    float getMultiplier() const;
        
	private:
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    Armor::eAttribute meAttribute;
	    int mnAdd;
	    float mfMultiplier;
	};
};

#endif

