#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "Element.h"

namespace StoneRing{

    class Graphic
    {
    public:
        Graphic();
        virtual ~Graphic();
        virtual clan::Rect GetRect()const=0;
        virtual bool IsSprite() const=0;
        virtual void Update()=0;
        virtual int GetSideBlock() const=0;
        virtual bool IsTile() const=0;
		virtual int GetZOrder() const = 0;
		virtual void Draw(clan::Canvas& gc, const clan::Point& offset)=0;
    private:
    };

    class SpriteRef;
    class Tilemap;

}

#endif



