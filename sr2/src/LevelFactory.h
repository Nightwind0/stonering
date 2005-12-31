#ifndef SR_LEVEL_FACTORY_H
#define SR_LEVEL_FACTORY_H



#include <ClanLib/core.h>
#include "IFactory.h"

namespace StoneRing
{


    class DirectionBlock;
    class Action;
    class Check;
    class Tilemap;
    class SpriteRef;
    class AttributeModifier;
    class Movement;
    class ItemRef;
    class HasGold;
    class HasItem;
    class DidEvent;
    class And;
    class Or;
    class Operator;
    class Condition;
    class Event;
    class PlayAnimation;
    class PlaySound;
    class LoadLevel;
    class StartBattle;
    class InvokeShop;
    class Pause;
    class Say;
    class Give;
    class Take;
    class GiveGold;
    class Tile;
    class MappableObject;
    class Level;
    class Option;
    class Choice;
    class NamedItemRef;

    class LevelFactory : public IFactory
	{
	public:
	    LevelFactory(){}
	    virtual ~LevelFactory(){}

	    virtual bool canCreate( Element::eElement element );
	    virtual Element * createElement( Element::eElement element );

	protected:


	    virtual    Element * createDirectionBlock()const;
	    virtual    Element * createTilemap()const;
	    virtual    Element * createSpriteRef()const;
	    virtual    Element * createMovement()const;
	    virtual    Element *  createItemRef()const;
	    virtual    Element * createAttributeModifier()const;
	    virtual    Element * createHasGold()const;
	    virtual    Element * createHasItem()const;
	    virtual    Element * createDidEvent()const;
	    virtual    Element * createAnd()const;
	    virtual    Element * createOr()const;
	    virtual    Element * createOperator()const;
	    virtual    Element * createCondition()const;
	    virtual    Element * createEvent()const;
	    virtual    Element * createPlayAnimation()const;
	    virtual    Element * createPlaySound()const;
	    virtual    Element * createLoadLevel()const;
	    virtual    Element * createStartBattle()const;
	    virtual    Element * createInvokeShop()const;
	    virtual    Element * createPause()const;
	    virtual    Element * createSay()const;
	    virtual    Element * createGive()const;
	    virtual    Element * createTake()const;
	    virtual    Element * createGiveGold()const;
	    virtual    Element * createTile()const;
	    virtual    Element * createMappableObject()const;
	  //  virtual    Element * createLevel()const;
	    virtual    Element * createOption()const;
	    virtual    Element * createChoice()const;
	    virtual    Element * createNamedItemRef()const;

	private:
	    typedef Element * (LevelFactory::*factoryMethod)() const;

	    factoryMethod getMethod(Element::eElement element) const;

	};


};

#endif
