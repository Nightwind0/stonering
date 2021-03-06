#include "SpriteRef.h"
#include "GraphicsManager.h"


namespace StoneRing { 

bool SpriteRef::handle_element(Element::eElement element, Element * pElement)
{
    return false;
}

void SpriteRef::load_attributes(clan::DomNamedNodeMap attributes)
{
    if(has_attribute("type",attributes))
    {
        std::string type = get_string("type",attributes);

        if(type == "idle") m_eType = SPR_BATTLE_IDLE;
        else if(type == "recoil") m_eType = SPR_BATTLE_RECOIL;
        else if(type == "weak") m_eType = SPR_BATTLE_WEAK;
        else if(type == "attack") m_eType = SPR_BATTLE_ATTACK;
        else if(type == "use") m_eType = SPR_BATTLE_USE;
        else if(type == "dead") m_eType = SPR_BATTLE_DEAD;
        //else throw clan::Exception("Bad type on spriteRef");

    }

    m_ref = get_string("ref",attributes);
}

void SpriteRef::handle_text(const std::string &text)
{
}

std::string SpriteRef::TypeName(SpriteRef::eType type)
{
    switch(type){
        case SPR_BATTLE_IDLE:
            return "idle";
        case SPR_BATTLE_RECOIL:
            return "recoil";
        case SPR_BATTLE_WEAK:
            return "weak";
        case SPR_BATTLE_ATTACK:
            return "attack";
        case SPR_BATTLE_USE:
            return "use";
        case SPR_BATTLE_DEAD:
            return "dead";
            
    }
	//assert(0);
	return "";
}



SpriteRef::SpriteRef( )
{

}

SpriteRef::~SpriteRef()
{
}

std::string SpriteRef::GetRef() const
{
    return m_ref;
}

/*
SpriteRef::eType
SpriteRef::GetType() const
{
    return m_eType;
}
*/

clan::Sprite SpriteRef::CreateSprite() const
{
    return GraphicsManager::CreateSprite( m_ref );
}


#if SR2_EDITOR
clan::DomElement SpriteRef::CreateDomElement(clan::DomDocument& doc)const
{
    clan::DomElement element(doc,"spriteRef");
    element.set_attribute("type",TypeName(m_eType));
    element.set_attribute("ref",m_ref);
    return element;
}
#endif
}