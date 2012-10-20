#ifndef GRAPHIC_H
#define GRAPHIC_H

#include "Element.h"

namespace StoneRing{

    class Graphic
    {
    public:
        Graphic();
        virtual ~Graphic();
        virtual CL_Rect GetRect()const=0;
        virtual bool IsSprite() const=0;
        virtual void Update()=0;
        virtual int GetSideBlock() const=0;
        virtual bool IsTile() const=0;
		virtual short GetZOrder() const = 0;
		virtual void Draw(CL_GraphicContext& gc, const CL_Point& offset)=0;
    private:
    };

    class SpriteRef;
    class Tilemap;

}

#endif



