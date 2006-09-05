#ifndef SR_ARMOR_TYPE_H
#define SR_ARMOR_TYPE_H

#include "Element.h"

namespace StoneRing{
    class ArmorType: public Element
	{
	public:
	    ArmorType();
	    ArmorType(CL_DomElement * pElement );
	    ~ArmorType();
		virtual eElement whichElement() const{ return EARMORTYPE; }	
	    virtual CL_DomElement createDomElement ( CL_DomDocument &) const;       

	    std::string getName() const;
	    std::string getIconRef() const;

	    uint getBasePrice() const;
	    int getBaseAC() const;
	    int getBaseRST() const;

	    enum eSlot { HEAD, BODY, SHIELD, FEET, HANDS };

	    eSlot getSlot() const;

	    bool operator==(const ArmorType &lhs );
        
	private:
	    virtual bool handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    std::string mName;
	    std::string mIconRef;
	    uint mnBasePrice;
	    int mnBaseAC;
	    int mnBaseRST;
	    eSlot meSlot;

	};
};
#endif

