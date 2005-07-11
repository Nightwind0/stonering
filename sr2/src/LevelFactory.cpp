#include "Level.h"
#include "LevelFactory.h"
#include <ClanLib/core.h>

using namespace StoneRing;



DirectionBlock * LevelFactory::createDirectionBlock()const
{
    return new DirectionBlock();

}

DirectionBlock * LevelFactory::createDirectionBlock(CL_DomElement * pElement)const
{
    return new DirectionBlock(pElement);
}


Tilemap * LevelFactory::createTilemap()const
{
    return new Tilemap();
}

Tilemap * LevelFactory::createTilemap(CL_DomElement * pElement )const
{
    return new Tilemap(pElement);

}

SpriteRef * LevelFactory::createSpriteRef()const
{
    return new SpriteRef();
}

SpriteRef * LevelFactory::createSpriteRef(CL_DomElement *pElement )const
{
    return new SpriteRef(pElement);

}

Movement * LevelFactory::createMovement()const
{
    return new Movement();
}

Movement * LevelFactory::createMovement(CL_DomElement * pElement )const
{
    return new Movement(pElement);

}

ItemRef * LevelFactory::createItemRef()const
{
    return new ItemRef();
}

ItemRef * LevelFactory::createItemRef(CL_DomElement * pElement ) const
{
    return new ItemRef(pElement );
}

AttributeModifier * LevelFactory::createAttributeModifier()const
{
    return new AttributeModifier();
}

AttributeModifier * LevelFactory::createAttributeModifier(CL_DomElement * pElement)const
{
    return new AttributeModifier(pElement);

}

HasGold * LevelFactory::createHasGold()const
{
    return new HasGold();
}

HasGold * LevelFactory::createHasGold(CL_DomElement * pElement)const
{
    return new HasGold(pElement);

}

HasItem * LevelFactory::createHasItem()const
{
    return new HasItem();
}

HasItem * LevelFactory::createHasItem(CL_DomElement * pElement)const
{
    return new HasItem(pElement);

}

DidEvent * LevelFactory::createDidEvent()const
{
    return new DidEvent();
}

DidEvent * LevelFactory::createDidEvent(CL_DomElement * pElement)const
{
    return new DidEvent(pElement);

}

And * LevelFactory::createAnd()const
{
    return new And();
}

And * LevelFactory::createAnd(CL_DomElement *pElement)const
{
    return new And(pElement);

}

Or * LevelFactory::createOr()const
{
    return new Or();
}

Or * LevelFactory::createOr(CL_DomElement *pElement)const
{
    return new Or(pElement);

}

Operator * LevelFactory::createOperator()const
{
    return new Operator();
}

Operator * LevelFactory::createOperator(CL_DomElement* pElement)const
{
    return new Operator(pElement);

}

Condition * LevelFactory::createCondition()const
{
    return new Condition();
}

Condition * LevelFactory::createCondition(CL_DomElement* pElement)const
{
    return new Condition(pElement);

}

Event * LevelFactory::createEvent()const
{
    return new Event();
}

Event * LevelFactory::createEvent(CL_DomElement *pElement)const
{
    return new Event(pElement);

}

PlayAnimation * LevelFactory::createPlayAnimation()const
{
    return new PlayAnimation();
}

PlayAnimation * LevelFactory::createPlayAnimation(CL_DomElement* pElement)const
{
    return new PlayAnimation(pElement);

}

StoneRing::PlaySound * LevelFactory::createPlaySound()const
{
    return new StoneRing::PlaySound();
}

StoneRing::PlaySound * LevelFactory::createPlaySound(CL_DomElement* pElement)const
{
    return new StoneRing::PlaySound(pElement);

}

LoadLevel * LevelFactory::createLoadLevel()const
{
    return new LoadLevel();
}

LoadLevel * LevelFactory::createLoadLevel(CL_DomElement* pElement)const
{
    return new LoadLevel(pElement);

}

StartBattle * LevelFactory::createStartBattle()const
{
    return new StartBattle();
}

StartBattle * LevelFactory::createStartBattle(CL_DomElement* pElement)const
{
    return new StartBattle(pElement);

}

InvokeShop * LevelFactory::createInvokeShop()const
{
    return new InvokeShop();
}

InvokeShop * LevelFactory::createInvokeShop(CL_DomElement* pElement)const
{
    return new InvokeShop(pElement);

}

Pause * LevelFactory::createPause()const
{
    return new Pause();
}

Pause * LevelFactory::createPause(CL_DomElement* pElement)const
{
    return new Pause(pElement);

}

Say * LevelFactory::createSay()const
{
    return new Say();
}

Say * LevelFactory::createSay(CL_DomElement* pElement)const
{
    return new Say(pElement);

}

Give * LevelFactory::createGive()const
{
    return new Give();
}

Give * LevelFactory::createGive(CL_DomElement* pElement)const
{
    return new Give(pElement);

}

Take * LevelFactory::createTake()const
{
    return new Take();
}

Take * LevelFactory::createTake(CL_DomElement* pElement)const
{
    return new Take(pElement);

}

GiveGold * LevelFactory::createGiveGold()const
{
    return new GiveGold();
}

GiveGold * LevelFactory::createGiveGold(CL_DomElement* pElement)const
{
    return new GiveGold(pElement);

}

Tile * LevelFactory::createTile()const
{
    return new Tile();
}

Tile * LevelFactory::createTile(CL_DomElement* pElement)const
{
    return new Tile(pElement);

}

MappableObject * LevelFactory::createMappableObject()const
{
    return new MappableObject();
}

MappableObject * LevelFactory::createMappableObject(CL_DomElement* pElement)const
{
    return new MappableObject(pElement);

}

Level * LevelFactory::createLevel()const
{
    return new Level();
}

Level * LevelFactory::createLevel(CL_DomDocument &document)const
{
    return new Level(document);

}

Option * LevelFactory::createOption()const
{
    return new Option();
}

Option * LevelFactory::createOption(CL_DomElement * pElement) const
{
    return new Option(pElement);
}

Choice * LevelFactory::createChoice()const
{
    return new Choice();
}

Choice * LevelFactory::createChoice(CL_DomElement * pElement) const
{
    return new Choice(pElement);
}

NamedItemRef * LevelFactory::createNamedItemRef()const
{
    return new NamedItemRef();
}

NamedItemRef * LevelFactory::createNamedItemRef(CL_DomElement * pElement ) const
{
    return new NamedItemRef ( pElement );
}

//    Tiles * createTiles();
//    MappableObjects * createMappableObjects();
