#ifndef SR_EVENT_H
#define SR_EVENT_H

#include "Element.h"
#include "Condition.h"

namespace StoneRing{

	class Event : public Element
	{
	public:
		Event();
		virtual ~Event();

		enum eTriggerType { STEP, TALK, ACT };
		virtual eElement whichElement() const{ return EEVENT; }	
		std::string getName() const;
		eTriggerType getTriggerType();
		bool repeatable();
		inline bool remember();
		bool invoke();

		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;


	protected:
		virtual bool handleElement(eElement element, Element * pElement );
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		std::string mName;
		bool mbRepeatable;
		bool mbRemember;
		eTriggerType meTriggerType;
		Condition *mpCondition;
		std::list<Action*> mActions;

	};


};


#endif

