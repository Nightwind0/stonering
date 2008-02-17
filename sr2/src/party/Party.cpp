
#include "Party.h"
#include "Level.h"
#include "Character.h"
#include "ItemManager.h"
#include <sstream>
#include "IParty.h"


using StoneRing::Party;
using StoneRing::IParty;
using StoneRing::ICharacter;
using StoneRing::ItemRef;
using StoneRing::Item;
using StoneRing::Character;

Party::Party():mnGold(0)
{
}

Party::~Party()
{
}


bool Party::getGold() const
{
    return mnGold;
}


bool Party::hasItem(ItemRef *pItemRef, uint count) const
{
    Item * pItem = pItemRef->getItem();
    return hasItem(pItem,count);
}

bool Party::didEvent(const std::string &event) const
{
    return mEvents.count(event) != 0;
}


void Party::doEvent(const std::string &name, bool bRemember)
{
    if(bRemember)
        mEvents.insert ( name );
}


void Party::giveItem(ItemRef *pItemRef, uint count)
{
    Item * pItem = pItemRef->getItem();

    giveItem(pItem,count);
}

void Party::takeItem(ItemRef *pItemRef, uint count)
{
    Item * pItem = pItemRef->getItem();

    takeItem(pItem,count);
}

bool Party::hasItem(Item * pItem,uint count) const
{
    if( mItems.count ( pItem ) && mItems.find( pItem )->second >= count)
        return true;
    else return false;
    
}

bool Party::giveItem(Item *pItem, uint count)
{
   
    if( !mItems.count(pItem) )
    {
        mItems [ pItem ] = 0;
    }
    
    
    if( mItems [ pItem ] + count <= pItem->getMaxInventory())
    {
        mItems [ pItem ] += count;
    }
    else 
    {
        count = pItem->getMaxInventory() - mItems[ pItem ];
    
        if( count < 1 )
        {
            return false;
        }
        else 
        {
            mItems [ pItem ] += count;

        }
    }
    
    return true;

}

bool Party::takeItem(Item *pItem, uint count)
{
    if( mItems.count(pItem ))
    {
        if ( mItems [ pItem ] < count )
        {
            count = mItems [ pItem ];
        }

        mItems[ pItem ] -= count;

    }
    else
    {
        return false;
    }
    return true;
}

void Party::giveGold(int amount)
{

    if(amount <0 )
    {
        // They are taking..
        if(mnGold < amount)
            throw CL_Error("Attempt to take more gold than we had.");
    }

    mnGold += amount;
    
}


uint Party::getCharacterCount() const 
{
    return 1;
}

uint Party::getTargetCharacterIndex() const
{
    return 0;
}

uint Party::getActorCharacterIndex() const
{
    return 0;
}

ICharacter * Party::getCharacter(uint index) const 
{
    return NULL;
}

ICharacter * Party::getTargetCharacter() const 
{
    return NULL;
}

ICharacter * Party::getActorCharacter() const 
{
    return NULL;
}

void Party::addCharacter(Character *pCharacter)
{
    mCharacters.push_back(pCharacter);
}

void Party::removeCharacter(const std::string &name)
{

}

Character * Party::getMapCharacter()const
{
    if(mCharacters.size()) 
    {
        return mCharacters[0];
    }
    else
    {
        return NULL;
    }
}










