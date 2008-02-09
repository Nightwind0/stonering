#include "SpriteRef.h"

bool StoneRing::SpriteRef::handleElement(eElement element, Element * pElement)
{
    return false;
}

void StoneRing::SpriteRef::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    if(hasAttr("type",pAttributes))
    {
        std::string type = getString("type",pAttributes);
        
        if(type == "still") meType = SPR_STILL;
        else if(type == "twoway") meType = SPR_TWO_WAY;
        else if(type == "fourway")  meType = SPR_FOUR_WAY;
        else if(type == "idle") meType = SPR_BATTLE_IDLE;
        else if(type == "recoil") meType = SPR_BATTLE_RECOIL;
        else if(type == "weak") meType = SPR_BATTLE_WEAK;
        else if(type == "use") meType = SPR_BATTLE_USE;
        else if(type == "dead") meType = SPR_BATTLE_DEAD;
        else throw CL_Error("Bad type on spriteRef");

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

