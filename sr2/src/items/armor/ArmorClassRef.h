#ifndef SR_ARMORCLASS_REF
#define SR_ARMORCLASS_REF

#include "Element.h"

namespace StoneRing{


    class ArmorClassRef : public Element
	{
	public:
	    ArmorClassRef();
	    virtual ~ArmorClassRef();
		virtual eElement whichElement() const{ return EARMORCLASSREF; }	
	    virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	    std::string getName() const;

	    void setName(const std::string &name){ mName = name; }
	    bool operator==(const ArmorClassRef &lhs );
	private:
		virtual void handleText(const std::string &text);
	    std::string mName;
	};

};


#endif


