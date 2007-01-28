#include "Level.h"
#include "ScriptElement.h"
#include "AttributeModifier.h"
#include "LevelFactory.h"
#include <ClanLib/core.h>

using namespace StoneRing;



Element * LevelFactory::createDirectionBlock()const
{
    return new DirectionBlock();

}



Element * LevelFactory::createTilemap()const
{
    return new Tilemap();
}


Element * LevelFactory::createSpriteRef()const
{
    return new SpriteRef();
}


Element * LevelFactory::createMovement()const
{
    return new Movement();
}


Element * LevelFactory::createItemRef()const
{
    return new ItemRef();
}


Element * LevelFactory::createAttributeModifier()const
{
    return new AttributeModifier();
}




Element * LevelFactory::createEvent()const
{
    return new Event();
}


Element * LevelFactory::createTile()const
{
    return new Tile();
}


Element * LevelFactory::createMappableObject()const
{
    return new MappableObject();
}


Element * LevelFactory::createNamedItemRef()const
{
    return new NamedItemRef();
}

Element * LevelFactory::createScriptElement()const
{
    return new ScriptElement(false);
}

Element * LevelFactory::createConditionScript()const
{
    return new ScriptElement(true);
}



//    Tiles * createTiles();
//    MappableObjects * createMappableObjects();
bool LevelFactory::canCreate( Element::eElement element )
{
    factoryMethod method = getMethod(element);

    if(method == NULL) return false;
    else return true;
        
}

LevelFactory::factoryMethod 
LevelFactory::getMethod(Element::eElement element) const
{
    switch(element)
    {   
    case Element::ESCRIPT:
        return &LevelFactory::createScriptElement;
    case Element::ECONDITIONSCRIPT:
        return &LevelFactory::createConditionScript;
    case Element::EDIRECTIONBLOCK:
        return &LevelFactory::createDirectionBlock;
    case Element::ETILEMAP:
        return &LevelFactory::createTilemap;
    case Element::ESPRITEREF:
        return &LevelFactory::createSpriteRef;
    case Element::EMOVEMENT:
        return &LevelFactory::createMovement;
    case Element::EITEMREF:
        return &LevelFactory::createItemRef;
    case Element::EATTRIBUTEMODIFIER:
        return &LevelFactory::createAttributeModifier;
    case Element::EEVENT:
        return &LevelFactory::createEvent;
    case Element::ETILE:
        return &LevelFactory::createTile;
    case Element::EMAPPABLEOBJECT:
        return &LevelFactory::createMappableObject;
    case Element::ENAMEDITEMREF:
        return &LevelFactory::createNamedItemRef;
    default:
        return NULL;
    }
}

Element * LevelFactory::createElement( Element::eElement element )
{
    factoryMethod method = getMethod(element);

    cl_assert ( method );

    Element * pElement = (this->*method)();

    return pElement;
}


