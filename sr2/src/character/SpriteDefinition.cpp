#include <cassert>
#include "SpriteDefinition.h"
#include "AbilityManager.h"
#include "IApplication.h"
#include "Animation.h"
#include "WeaponTypeRef.h"
#include "CharacterManager.h"

namespace StoneRing{


StoneRing::SpriteDefinition::SpriteDefinition():mpSpriteRef(NULL),mbHasBindPoints(false)
{
}

StoneRing::SpriteDefinition::~SpriteDefinition()
{
}



bool StoneRing::SpriteDefinition::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESPRITEREF:
        if(mpSpriteRef) throw CL_Error("Sprite Ref already defined for Sprite Definition");
        mpSpriteRef = dynamic_cast<SpriteRef*>(pElement);
    default:
        return false;
    }

    return true;
}

void StoneRing::SpriteDefinition::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mName = getRequiredString("name",pAttributes);

    if(hasAttr("bindPoint1",pAttributes))
    {
        mbHasBindPoints = true;
        mnBindPoint1 = getRequiredInt("bindPoint1",pAttributes);
        mnBindPoint2 = getRequiredInt("bindPoint2",pAttributes);
    }

}

}
