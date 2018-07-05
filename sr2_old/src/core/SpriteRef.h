#ifndef SR2_SPRITE_REF_H
#define SR2_SPRITE_REF_H

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "Element.h"


namespace StoneRing
{

    class SpriteRef : public Element
    {
    public:
        SpriteRef();
        virtual ~SpriteRef();
        virtual eElement WhichElement() const{ return ESPRITEREF; }
        enum eType {
            SPR_BATTLE_IDLE,
            SPR_BATTLE_ATTACK,
            SPR_BATTLE_RECOIL,
            SPR_BATTLE_USE, // Generic use ability or item
            SPR_BATTLE_WEAK,
            SPR_BATTLE_DEAD
        };

        eType GetType() const;
        std::string GetRef() const;
        clan::Sprite  CreateSprite() const;
#if SR2_EDITOR
        clan::DomElement CreateDomElement(clan::DomDocument&)const;
#endif
        static std::string TypeName(eType);
		virtual std::string GetDebugId() const { return m_ref; }				
    protected:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(clan::DomNamedNodeMap attributes);
        virtual void handle_text(const std::string &text);

        eType m_eType;
        std::string m_ref;
    };

}

#endif

