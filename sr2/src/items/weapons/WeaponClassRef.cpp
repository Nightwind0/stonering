#include "WeaponClassRef.h"

using namespace StoneRing;

WeaponClassRef::WeaponClassRef()
{
}

void WeaponClassRef::handleText(const std::string &text)
{
	mName = text;
}

WeaponClassRef::~WeaponClassRef()
{
}

bool WeaponClassRef::operator==(const WeaponClassRef &lhs)
{
	if(mName == lhs.mName) return true;
	else return false;
}

CL_DomElement  WeaponClassRef::createDomElement(CL_DomDocument &doc) const
{
	CL_DomElement element(doc,"weaponClassRef");

	CL_DomText text(doc, mName );

	text.set_node_value ( mName );

	element.append_child ( text );

	return element;
}


std::string WeaponClassRef::getName() const
{
	return mName;
}




