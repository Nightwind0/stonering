#ifndef SR_ADDCHARACTER_H
#define SR_ADDCHARACTER_H

#include "Action.h"
#include "Element.h"

namespace StoneRing{	

	class AddCharacter : public Action, public Element
	{
	public:
		AddCharacter();
		virtual ~AddCharacter();
		virtual void invoke();
		virtual eElement whichElement() const{ return EADDCHARACTER; }	
		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
	private:
		virtual bool handleElement(eElement element, Element * pElement );
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);

		//		std::list<StartingEquiptmentRef*> mStartingEquipment;
		std::string mName;
		uint mnLevel;

	};

};

#endif

