#ifndef SR_LEVEL_FACTORY_H
#define SR_LEVEL_FACTORY_H



#include <ClanLib/core.h>

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


class LevelFactory
{
 public:
    LevelFactory(){}
    ~LevelFactory();

    virtual    DirectionBlock * createDirectionBlock()const;
    virtual    DirectionBlock * createDirectionBlock(CL_DomElement * pElement)const;
    virtual    Tilemap * createTilemap()const;
    virtual    Tilemap * createTilemap(CL_DomElement * pElement )const;
    virtual    SpriteRef * createSpriteRef()const;
    virtual    SpriteRef * createSpriteRef(CL_DomElement *pElement )const;
    virtual    Movement * createMovement()const;
    virtual    Movement * createMovement(CL_DomElement * pElement )const;
    virtual    ItemRef *  createItemRef()const;
    virtual    ItemRef *  createItemRef(CL_DomElement * pElement)const;
    virtual    AttributeModifier * createAttributeModifier()const;
    virtual    AttributeModifier * createAttributeModifier(CL_DomElement * pElement)const;
    virtual    HasGold * createHasGold()const;
    virtual    HasGold * createHasGold(CL_DomElement * pElement)const;
    virtual    HasItem * createHasItem()const;
    virtual    HasItem * createHasItem(CL_DomElement * pElement)const;
    virtual    DidEvent * createDidEvent()const;
    virtual    DidEvent * createDidEvent(CL_DomElement * pElement)const;
    virtual    And * createAnd()const;
    virtual    And * createAnd(CL_DomElement *pElement)const;
    virtual    Or * createOr()const;
    virtual    Or * createOr(CL_DomElement *pElement)const;
    virtual    Operator * createOperator()const;
    virtual    Operator * createOperator(CL_DomElement* pElement)const;
    virtual    Condition * createCondition()const;
    virtual    Condition * createCondition(CL_DomElement* pElement)const;
    virtual    Event * createEvent()const;
    virtual    Event * createEvent(CL_DomElement *pElement)const;
    virtual    PlayAnimation * createPlayAnimation()const;
    virtual    PlayAnimation * createPlayAnimation(CL_DomElement*)const;
    virtual    PlaySound * createPlaySound()const;
    virtual    PlaySound * createPlaySound(CL_DomElement* pElement)const;
    virtual    LoadLevel * createLoadLevel()const;
    virtual    LoadLevel * createLoadLevel(CL_DomElement* pElement)const;
    virtual    StartBattle * createStartBattle()const;
    virtual    StartBattle * createStartBattle(CL_DomElement* pElement)const;
    virtual    InvokeShop * createInvokeShop()const;
    virtual    InvokeShop * createInvokeShop(CL_DomElement* pElement)const;
    virtual    Pause * createPause()const;
    virtual    Pause * createPause(CL_DomElement* pElement)const;
    virtual    Say * createSay()const;
    virtual    Say * createSay(CL_DomElement* pElement)const;
    virtual    Give * createGive()const;
    virtual    Give * createGive(CL_DomElement* pElement)const;
    virtual    Take * createTake()const;
    virtual    Take * createTake(CL_DomElement* pElement)const;
    virtual    GiveGold * createGiveGold()const;
    virtual    GiveGold * createGiveGold(CL_DomElement* pElement)const;
    virtual    Tile * createTile()const;
    virtual    Tile * createTile(CL_DomElement* pElement)const;
    virtual    MappableObject * createMappableObject()const;
    virtual    MappableObject * createMappableObject(CL_DomElement* pElement)const;
    virtual    Level * createLevel()const;
    virtual    Level * createLevel(CL_DomDocument &document)const;
//    virtual    Tiles * createTiles();
//    virtual    MappableObjects * createMappableObjects();
 private:
};


};

#endif
