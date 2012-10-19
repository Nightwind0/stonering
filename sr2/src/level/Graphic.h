#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "Element.h"

namespace StoneRing{

    class Graphic : public Element
    {
    public:
        Graphic();
        virtual ~Graphic();

        virtual int GetX() const=0;
        virtual int GetY() const=0;
        virtual CL_Rect GetRect()const=0;
        virtual bool IsSprite() const=0;
        virtual void Update()=0;
        virtual int GetSideBlock() const=0;
        virtual bool IsTile() const=0;

    private:
    };

    class SpriteRef;
    class Tilemap;

}

#endif



