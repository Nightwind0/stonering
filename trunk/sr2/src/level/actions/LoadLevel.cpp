#include "LoadLevel.h"

using namespace StoneRing;

LoadLevel::LoadLevel()
{
}


CL_DomElement  LoadLevel::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"loadLevel");

    element.set_attribute("startx", IntToString (mStartX ) ) ;
    element.set_attribute("starty", IntToString (mStartY ) );
    element.set_attribute("name", mName );

    return element;
}

void LoadLevel::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mStartX = getRequiredInt("startx",pAttributes);
    mStartY = getRequiredInt("starty",pAttributes);

    mName = getRequiredString("name",pAttributes);
}

LoadLevel::~LoadLevel()
{
}


