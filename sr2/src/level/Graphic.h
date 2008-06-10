#ifndef GRAPHIC_H
#define GRAPHIC_H 

#include "Element.h"

namespace StoneRing{

    class Graphic : public Element
    {
    public:
        Graphic();
        virtual ~Graphic();

        virtual uint GetX() const=0;
        virtual uint GetY() const=0;
        virtual CL_Rect GetRect()=0;
        virtual bool IsSprite() const=0;
        virtual void Draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)=0;
        virtual void Update()=0;
        virtual int GetDirectionBlock() const=0;
        virtual bool IsTile() const=0;

    private:
    };

    class SpriteRef;
    class Tilemap;

    union SpriteRefOrTilemap
    {
        SpriteRef* asSpriteRef;
        Tilemap * asTilemap;
    };


};

#endif



