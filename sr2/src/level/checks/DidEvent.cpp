#include "DidEvent.h"
#include "IApplication.h"

using namespace StoneRing;

DidEvent::DidEvent()
{
}

CL_DomElement  DidEvent::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"didEvent");

    if(mbNot) element.set_attribute("not","true");

    CL_DomText text ( doc, mEvent );
    text.set_node_value ( mEvent );

    element.append_child ( text);

    return element;
}

void DidEvent::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mbNot = getImpliedBool("not",pAttributes,false);
}

void DidEvent::handleText(const std::string &text)
{
    mEvent = text;
}

bool DidEvent::handleElement(eElement element, Element *pElement)
{
	return false;
}

DidEvent::~DidEvent()
{
}

bool DidEvent::evaluate()
{
    if(mbNot) return ! IApplication::getInstance()->getParty()->didEvent ( mEvent );
    else  return  IApplication::getInstance()->getParty()->didEvent ( mEvent );
}
