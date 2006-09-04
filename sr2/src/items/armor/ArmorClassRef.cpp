#include "ArmorClassRef.h"


using namespace StoneRing;

ArmorClassRef::ArmorClassRef()
{
}


ArmorClassRef::~ArmorClassRef()
{
}

void ArmorClassRef::handleText(const std::string &text)
{
	mName = text;
}

bool ArmorClassRef::operator==(const ArmorClassRef &lhs)
{
	if( mName == lhs.mName) return true;
	else return false;
}

CL_DomElement  
ArmorClassRef::createDomElement(CL_DomDocument &doc) const
{
	CL_DomElement element(doc,"armorClassRef");

	CL_DomText text(doc, mName );

	text.set_node_value ( mName );

	element.append_child ( text );

	return element;
}


std::string 
ArmorClassRef::getName() const
{
	return mName;
}