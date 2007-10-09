#include "SpriteRef.h"

bool StoneRing::SpriteRef::handleElement(eElement element, Element * pElement)
{
    return false;
}

void StoneRing::SpriteRef::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    if(hasAttr("type",pAttributes))
    {
        std::string direction = getString("type",pAttributes);
        
        if(direction == "still") meType = SPR_STILL;
        if(direction == "twoway") meType = SPR_TWO_WAY;
        if(direction == "fourway")  meType = SPR_FOUR_WAY;

    }

}

void StoneRing::SpriteRef::handleText(const std::string &text)
{
    mRef = text;
}


StoneRing::SpriteRef::SpriteRef( ):meType(SPR_NONE)
{
  
}

StoneRing::SpriteRef::~SpriteRef()
{
}

std::string StoneRing::SpriteRef::getRef() const
{
    return mRef;
}

StoneRing::SpriteRef::eType 
StoneRing::SpriteRef::getType() const
{
    return meType;
}