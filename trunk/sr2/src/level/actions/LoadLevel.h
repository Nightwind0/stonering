#ifndef SR_LOAD_LEVEL_H
#define SR_LOAD_LEVEL_H

#include "Action.h"
#include "Element.h"


namespace StoneRing{ 
	class LoadLevel : public Action, public Element
	{
	public:
		LoadLevel();
		virtual ~LoadLevel();

		virtual void invoke();
		virtual eElement whichElement() const{ return ELOADLEVEL; }	
		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		std::string mName;
		ushort mStartY;
		ushort mStartX;
	};
};
#endif

