#include "ArmorTypeRef.h"

using namespace StoneRing;

ArmorTypeRef::ArmorTypeRef()
{
}



ArmorTypeRef::~ArmorTypeRef()
{
}

void ArmorTypeRef::handleText(const std::string &text)
{
	mName = text;
}

bool ArmorTypeRef::operator==(const ArmorTypeRef &lhs)
{
	if( mName == lhs.mName) return true;
	else return false;
}


CL_DomElement  
ArmorTypeRef::createDomElement(CL_DomDocument &doc) const
{
	CL_DomElement element(doc,"armorTypeRef");

	CL_DomText text(doc, mName );

	text.set_node_value ( mName );

	element.append_child ( text );

	return element;
}



std::string ArmorTypeRef::getName() const
{
	return mName;
}