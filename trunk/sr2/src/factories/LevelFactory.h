#ifndef SR_LEVEL_FACTORY_H
#define SR_LEVEL_FACTORY_H



#include <ClanLib/core.h>
#include "IFactory.h"

namespace StoneRing
{


    class LevelFactory : public IFactory
    {
    public:
        LevelFactory(){}
        virtual ~LevelFactory(){}

        virtual bool canCreate( Element::eElement element );
        virtual Element * createElement( Element::eElement element );

    protected:

        virtual    Element * createScriptElement() const;
        virtual    Element * createConditionScript() const;
        virtual    Element * createDirectionBlock()const;
        virtual    Element * createTilemap()const;
        virtual    Element * createSpriteRef()const;
        virtual    Element * createMovement()const;
        virtual    Element * createItemRef()const;
        virtual    Element * createAttributeModifier()const;
        virtual    Element * createEvent()const;
        virtual    Element * createTile()const;
        virtual    Element * createMappableObject()const;
        //  virtual    Element * createLevel()const;
        virtual    Element * createNamedItemRef()const;

    private:
        typedef Element * (LevelFactory::*factoryMethod)() const;

        factoryMethod getMethod(Element::eElement element) const;

    };


};

#endif


