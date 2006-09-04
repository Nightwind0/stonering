#ifndef SR_REGULARITEM_H
#define SR_REGULARITEM_H  

#include "NamedItem.h"

  // Concrete Named Item classes
namespace StoneRing{
    class RegularItem: public NamedItem
	{
	public:
	    RegularItem();
	    virtual ~RegularItem();

		virtual eElement whichElement() const{ return EREGULARITEM; }	 
	    void invoke(); // Execute all actions.

	    enum eUseType {BATTLE, WORLD, BOTH };
	    enum eTargetable { ALL, SINGLE, EITHER, SELF_ONLY };
	    enum eDefaultTarget { PARTY, MONSTERS };
	    eUseType getUseType() const;
	    eTargetable getTargetable() const;
	    eDefaultTarget getDefaultTarget() const;
	    bool isReusable() const;
        
	    virtual eItemType getItemType() const { return REGULAR_ITEM; }


	    virtual uint getValue() const ; // Price to buy, and worth when calculating drops.
	    virtual uint getSellValue() const ;
	    virtual void loadItem ( CL_DomElement * pElement );
        
	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	    static eUseType UseTypeFromString ( const std::string &str );
	    static eTargetable TargetableFromString ( const std::string &str );

	private:
	    virtual bool handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    std::list<Action*> mActions;
	    eUseType meUseType;
	    eTargetable meTargetable;
	    uint mnValue;
	    uint mnSellValue;
	    bool mbReusable;
	    eDefaultTarget meDefaultTarget;
	};
};
#endif