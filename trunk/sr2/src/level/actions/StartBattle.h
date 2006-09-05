#ifndef SR_START_BATTLE_H
#define SR_START_BATTLE_H

#include "Action.h"
#include "Element.h"


namespace StoneRing{

	class StartBattle : public Action, public Element
	{
	public:
		StartBattle();
		virtual ~StartBattle();
		virtual eElement whichElement() const{ return ESTARTBATTLE; }	
		virtual void invoke();

		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
		virtual bool handleElement(eElement element, Element * pElement );
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		std::string mMonster;
		ushort mCount;
		bool mbIsBoss;
	};
};

#endif

