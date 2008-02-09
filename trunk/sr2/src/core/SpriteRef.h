#ifndef SR2_SPRITE_REF_H
#define SR2_SPRITE_REF_H   

#include "Element.h"


namespace StoneRing
{

    class SpriteRef : public Element
    {
    public:
        SpriteRef();
        virtual ~SpriteRef();
        virtual eElement whichElement() const{ return ESPRITEREF; } 
        enum eType {
            SPR_NONE, 
            SPR_STILL, 
            SPR_TWO_WAY, 
            SPR_FOUR_WAY, 
            _END_MO_TYPES,
            SPR_BATTLE_IDLE,
            SPR_BATTLE_RECOIL,
            SPR_BATTLE_USE, // Generic use ability or item
            SPR_BATTLE_WEAK,
            SPR_BATTLE_DEAD
        };

        eType getType() const;
        std::string getRef() const;
    protected:
        virtual bool handleElement(eElement element, Element * pElement );      
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        virtual void handleText(const std::string &text);

        eType meType;
        std::string mRef;
    };

}

#endif

