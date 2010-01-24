#include "SpriteRef.h"

bool StoneRing::SpriteRef::handle_element(Element::eElement element, Element * pElement)
{
    return false;
}

void StoneRing::SpriteRef::load_attributes(CL_DomNamedNodeMap attributes)
{
    if(has_attribute("type",attributes))
    {
        std::string type = get_string("type",attributes);

        if(type == "still") m_eType = SPR_STILL;
        else if(type == "twoway") m_eType = SPR_TWO_WAY;
        else if(type == "fourway")  m_eType = SPR_FOUR_WAY;
        else if(type == "idle") m_eType = SPR_BATTLE_IDLE;
        else if(type == "recoil") m_eType = SPR_BATTLE_RECOIL;
        else if(type == "weak") m_eType = SPR_BATTLE_WEAK;
        else if(type == "attack") m_eType = SPR_BATTLE_ATTACK;
        else if(type == "use") m_eType = SPR_BATTLE_USE;
        else if(type == "dead") m_eType = SPR_BATTLE_DEAD;
        else throw CL_Exception("Bad type on spriteRef");

    }

    m_ref = get_string("ref",attributes);
}

void StoneRing::SpriteRef::handle_text(const std::string &text)
{
}


StoneRing::SpriteRef::SpriteRef( ):m_eType(SPR_NONE)
{

}

StoneRing::SpriteRef::~SpriteRef()
{
}

std::string StoneRing::SpriteRef::GetRef() const
{
    return m_ref;
}

StoneRing::SpriteRef::eType
StoneRing::SpriteRef::GetType() const
{
    return m_eType;
}

