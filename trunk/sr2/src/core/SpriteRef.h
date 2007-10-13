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
        enum eType {SPR_NONE, SPR_STILL, SPR_TWO_WAY, SPR_FOUR_WAY };

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

