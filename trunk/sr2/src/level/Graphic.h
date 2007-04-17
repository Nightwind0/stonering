#ifndef GRAPHIC_H
#define GRAPHIC_H 

#include "Element.h"

namespace StoneRing{

    class Graphic : public Element
    {
    public:
        Graphic();
        virtual ~Graphic();

        virtual uint getX() const=0;
        virtual uint getY() const=0;
        virtual CL_Rect getRect()=0;
        virtual bool isSprite() const=0;
        virtual void draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)=0;
        virtual void update()=0;
        virtual int getDirectionBlock() const=0;
        virtual bool isTile() const=0;

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



