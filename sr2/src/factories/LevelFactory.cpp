#include "Level.h"
#include "Action.h"
#include "Check.h"
#include "LevelFactory.h"
#include "Checks.h"
#include "Actions.h"
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


Element * LevelFactory::createHasGold()const
{
    return new HasGold();
}


Element * LevelFactory::createHasItem()const
{
    return new HasItem();
}


Element * LevelFactory::createDidEvent()const
{
    return new DidEvent();
}


Element * LevelFactory::createAnd()const
{
    return new And();
}

Element * LevelFactory::createOr()const
{
    return new Or();
}


Element * LevelFactory::createOperator()const
{
    return new Operator();
}


Element * LevelFactory::createCondition()const
{
    return new Condition();
}


Element * LevelFactory::createEvent()const
{
    return new Event();
}



Element * LevelFactory::createPlayScene()const
{
    return new PlayScene();
}



Element * LevelFactory::createPlaySound()const
{
    return new StoneRing::PlaySound();
}


Element * LevelFactory::createLoadLevel()const
{
    return new LoadLevel();
}


Element * LevelFactory::createStartBattle()const
{
    return new StartBattle();
}


Element * LevelFactory::createInvokeShop()const
{
    return new InvokeShop();
}


Element * LevelFactory::createPause()const
{
    return new Pause();
}



Element * LevelFactory::createSay()const
{
    return new Say();
}



Element * LevelFactory::createGive()const
{
    return new Give();
}


Element * LevelFactory::createTake()const
{
    return new Take();
}


Element * LevelFactory::createGiveGold()const
{
    return new GiveGold();
}


Element * LevelFactory::createTile()const
{
    return new Tile();
}


Element * LevelFactory::createMappableObject()const
{
    return new MappableObject();
}


Element * LevelFactory::createOption()const
{
    return new Option();
}


Element * LevelFactory::createChoice()const
{
    return new Choice();
}



Element * LevelFactory::createNamedItemRef()const
{
    return new NamedItemRef();
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
    case Element::EHASGOLD:
        return &LevelFactory::createHasGold;
    case Element::EHASITEM:
        return &LevelFactory::createHasItem;
    case Element::EDIDEVENT:
        return &LevelFactory::createDidEvent;
    case Element::EAND:
        return &LevelFactory::createAnd;
    case Element::EOR:
        return &LevelFactory::createOr;
    case Element::EOPERATOR:
        return &LevelFactory::createOperator;
    case Element::ECONDITION:
        return &LevelFactory::createCondition;
    case Element::EEVENT:
        return &LevelFactory::createEvent;
    case Element::EPLAYSCENE:
        return &LevelFactory::createPlayScene;
    case Element::EPLAYSOUND:
        return &LevelFactory::createPlaySound;
    case Element::ELOADLEVEL:
        return &LevelFactory::createLoadLevel;
    case Element::ESTARTBATTLE:
        return &LevelFactory::createStartBattle;
    case Element::EINVOKESHOP:
        return &LevelFactory::createInvokeShop;
    case Element::EPAUSE:
        return &LevelFactory::createPause;
    case Element::ESAY:
        return &LevelFactory::createSay;
    case Element::EGIVE:
        return &LevelFactory::createGive;
    case Element::ETAKE:
        return &LevelFactory::createTake;
    case Element::EGIVEGOLD:
        return &LevelFactory::createGiveGold;
    case Element::ETILE:
        return &LevelFactory::createTile;
    case Element::EMAPPABLEOBJECT:
        return &LevelFactory::createMappableObject;
  
    case Element::EOPTION:
        return &LevelFactory::createOption;
    case Element::ECHOICE:
        return &LevelFactory::createChoice;
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


