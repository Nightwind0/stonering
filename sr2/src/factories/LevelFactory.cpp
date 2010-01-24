#include "Level.h"
#include "ScriptElement.h"
#include "AttributeModifier.h"
#include "LevelFactory.h"
#include <ClanLib/core.h>

using namespace StoneRing;







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




