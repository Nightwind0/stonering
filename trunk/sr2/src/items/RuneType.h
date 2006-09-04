#ifndef SR_RUNETYPE_H
#define SR_RUNETYPE_H

#include "Element.h"

namespace StoneRing{

    class RuneType : public Element
	{
	public:
	    RuneType();
	    virtual ~RuneType();
		virtual eElement whichElement() const{ return ERUNETYPE; }	
	    virtual CL_DomElement createDomElement ( CL_DomDocument &) const;

	    enum eRuneType { NONE, RUNE, ULTRA_RUNE };

	    eRuneType getRuneType() const;

	    std::string getRuneTypeAsString() const;

	    void setRuneType ( eRuneType type) { meRuneType = type; }

	    bool operator==(const RuneType &lhs);

	private:
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
	    eRuneType meRuneType;
	};
};

#endif