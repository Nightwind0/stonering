#include "Pause.h"
#include "IApplication.h"

using namespace StoneRing;

Pause::Pause()
{
}

CL_DomElement  Pause::createDomElement(CL_DomDocument &doc) const
{
	CL_DomElement  element(doc,"pause");

	CL_DomText text(doc,IntToString(mMs));

	//    text.set_node_value( IntToString (mMs ) );

	element.append_child ( text );

	return element;
}


void Pause::handleText(const std::string &text)
{
	mMs = atoi ( text.c_str() );
}

Pause::~Pause()
{
}

void Pause::invoke()
{
	IApplication::getInstance()->pause ( mMs ) ;
}


