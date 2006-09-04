#ifndef SR_CHOICE_H
#define SR_CHOICE_H

#include "Action.h"
#include "Element.h"

namespace StoneRing{

	class Condition;

	class Option : public Element
	{
	public:
		Option();
		virtual ~Option();
		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
		virtual eElement whichElement() const{ return EOPTION; }	
		virtual std::string getText() const;

		virtual bool evaluateCondition() const;

		virtual void choose();
	protected:
		virtual bool handleElement(eElement element, Element * pElement );      
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
	private:
		std::string mText;
		std::list<Action*> mActions;
		Condition * mpCondition;

	};

	class Choice : public Action, public Element
	{
	public:
		Choice();
		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;
		virtual eElement whichElement() const{ return ECHOICE; }	
		virtual void invoke();

		virtual std::string getText() const;

		virtual uint getOptionCount() const;

		Option * getOption(uint index );

		// To be called by application when choice is made.
		void chooseOption( uint index);

	protected:
		virtual bool handleElement(eElement element, Element * pElement );
		virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
		virtual void loadFinished();

	private:
		std::vector<Option*> mOptions;
		std::string mText;
	};
};

#endif