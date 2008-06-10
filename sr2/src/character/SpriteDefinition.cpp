#include <cassert>
#include "SpriteDefinition.h"
#include "AbilityManager.h"
#include "IApplication.h"
#include "Animation.h"
#include "WeaponTypeRef.h"
#include "CharacterManager.h"

namespace StoneRing{


StoneRing::SpriteDefinition::SpriteDefinition():m_pSpriteRef(NULL),m_bHasBindPoints(false)
{
}

StoneRing::SpriteDefinition::~SpriteDefinition()
{
}



bool StoneRing::SpriteDefinition::handle_element(eElement element, Element * pElement)
{
    switch(element)
    {
    case ESPRITEREF:
        if(m_pSpriteRef) throw CL_Error("Sprite Ref already defined for Sprite Definition");
        m_pSpriteRef = dynamic_cast<SpriteRef*>(pElement);
    default:
        return false;
    }

    return true;
}

void StoneRing::SpriteDefinition::load_attributes(CL_DomNamedNodeMap *pAttributes)
{
    m_name = get_required_string("name",pAttributes);

    if(has_attribute("bindPoint1",pAttributes))
    {
        m_bHasBindPoints = true;
        m_nBindPoint1 = get_required_int("bindPoint1",pAttributes);
        m_nBindPoint2 = get_required_int("bindPoint2",pAttributes);
    }

}

}
