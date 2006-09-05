#include "Choice.h"
#include "Condition.h"
#include "IApplication.h"

using namespace StoneRing;


Option::Option():mpCondition(NULL)
{
}


bool Option::handleElement(eElement element, Element * pElement )
{
	switch(element)
	{
	case ECONDITION:
		mpCondition = dynamic_cast<Condition *>(pElement);
		break;
	case EATTRIBUTEMODIFIER:
	case ESAY:
	case EGIVE:
	case ETAKE:
	case EPLAYSCENE:
	case EPLAYSOUND:
	case ELOADLEVEL:
	case ESTARTBATTLE:
	case EPAUSE:
	case EINVOKESHOP:
	case EGIVEGOLD:
	case ECHOICE:

		mActions.push_back ( dynamic_cast<Action*>(pElement) );
		break;
	default:
		return false;
	}

	return true;
}

void Option::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
	mText = getRequiredString("text",pAttributes);
}


Option::~Option()
{
	delete mpCondition;

	for(std::list<Action*>::const_iterator iter = mActions.begin();
		iter != mActions.end();
		iter++)
	{
		delete *iter;
	}
}

CL_DomElement  Option::createDomElement(CL_DomDocument &document) const
{

	CL_DomElement  element(document,"option");

	element.set_attribute("text",mText );


	for(std::list<Action*>::const_iterator i = mActions.begin();
		i != mActions.end();
		i++)
	{
		Element * pElement = dynamic_cast<Element*>(*i);
		CL_DomElement  e = pElement->createDomElement(document);
		element.append_child(e );

	}

	return element;

}

std::string Option::getText() const
{
	return mText;
}

bool Option::evaluateCondition() const
{
	if(!mpCondition) return true;

	return mpCondition->evaluate();
}

void Option::choose()
{

	if(evaluateCondition())
		std::for_each( mActions.begin(), mActions.end(), std::mem_fun(&Action::invoke) );

}



Choice::Choice()
{
}


bool Choice::handleElement(eElement element, Element * pElement )
{
	switch(element)
	{
	case EOPTION:
		mOptions.push_back ( dynamic_cast<Option*>(pElement) );
		break;
	default:

		return false;
	}

	return true;
}

void Choice::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
	mText = getRequiredString("text",pAttributes);
}

void Choice::loadFinished()
{
	if(!mOptions.size())
		throw CL_Error("Choice missing options!!!");

}

CL_DomElement Choice::createDomElement(CL_DomDocument &doc) const
{

	CL_DomElement  element(doc,"choice");

	element.set_attribute("text",mText );


	for(std::vector<Option*>::const_iterator i = mOptions.begin();
		i != mOptions.end();
		i++)
	{
		CL_DomElement  e = (*i)->createDomElement(doc);
		element.append_child(e );

	}

	return element;

}

void Choice::invoke()
{
	std::vector<std::string> options;


	// fill the "options" vector with the text of each Option object
#ifndef _MSC_VER
	std::transform(mOptions.begin(), mOptions.end(), std::back_inserter(options),
		std::mem_fun(&Option::getText));
#else
	for(std::vector<Option*>::iterator iter = mOptions.begin();
		iter != mOptions.end();
		iter++)
	{   
		options.push_back ( (*iter)->getText() );
	}
#endif

	IApplication::getInstance()->choice( mText, options, this );

}

std::string Choice::getText() const
{
	return mText;
}

uint Choice::getOptionCount() const
{
	return mOptions.size();
}

Option * Choice::getOption(uint index )
{
	return mOptions[index];
}

void Choice::chooseOption( uint index)
{
	mOptions[index]->choose();
}

