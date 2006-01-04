
#include "Party.h"
#include "Level.h"
#include "Character.h"
#include "ItemManager.h"
#include <sstream>
#include "IParty.h"


using StoneRing::Party;
using StoneRing::IParty;
using StoneRing::ICharacter;



Party::Party():mX(0),mY(0),mnGold(0)
{
}

Party::~Party()
{
}

IParty::eDirection Party::getDirection()const
{
	return meDirection;
}

bool Party::isAligned() const
{
        return (mX /32 == 0) && (mY /32 == 0);
}

void Party::move()
{
        uint speed  = 1;

		switch(meDirection)
		{
		case DNORTH:
			mY -= speed;
			break;
		case DSOUTH:
			mY += speed;
			break;
		case DWEST:
			mX -= speed;
			break;
		case DEAST:
			mX += speed;
			break;
		}
}

void Party::changeDirection(eDirection direction)
{
	meDirection = direction;
}

void Party::resetPosition(uint levelX, uint levelY)
{
	mX = levelX;
	mY = levelY;
}

bool Party::getGold() const
{
	return mnGold;
}


bool Party::hasItem(ItemRef *pItemRef, uint count) const
{

    Item * pItem = pItemRef->getItem();

    if( mItems.count ( pItem ) && mItems.find( pItem )->second >= count)
	return true;
    else return false;
}

bool Party::didEvent(const std::string &event) const
{
    return mEvents.count(event) != 0;
}

uint Party::getLevelX() const
{
	return mX;
}

uint Party::getLevelY() const
{
	return mY;
}

uint Party::getCellWidth() const
{
    return 1;
}

uint Party::getCellHeight() const
{
    return 1;
}

uint Party::getCellX() const
{
  return mX / 32;
}

uint Party::getCellY() const
{
  return mY / 32;
}

void Party::setLevelX(uint x)
{
    mX = x;
}

void Party::setLevelY(uint y)
{
    mY = y;
}

void Party::doEvent(const std::string &name, bool bRemember)
{

    if(bRemember)
	mEvents.insert ( name );
}


void Party::giveItem(ItemRef *pItemRef, uint count)
{
    std::ostringstream os;
    std::string speaker = "Item received!"; 
    IApplication * pApplication = IApplication::getInstance();
    Item * pItem = pItemRef->getItem();

    os << pItem->getName();
    
    if( !mItems.count(pItem) )
    {
	mItems [ pItem ] = 0;
    }
    
    
    if( mItems [ pItem ] + count <= pItem->getMaxInventory())
    {
	mItems [ pItem ] += count;
	
	if(count > 1)
	    os << " x" << count;
    }
    else 
    {
	count = pItem->getMaxInventory() - mItems[ pItem ];
	
	if( count < 1 )
	{
	    speaker = "**Item Lost** Inventory Full";
	}
	else 
	{
	    mItems [ pItem ] += count;

	    os << " x" << count;
	}
    }
    


    pApplication->say(speaker, os.str());

}

void Party::takeItem(ItemRef *pItemRef, uint count)
{
    std::ostringstream os;
    IApplication * pApplication = IApplication::getInstance();
    Item * pItem = pItemRef->getItem();
    
    os << pItem->getName();
    if( mItems.count(pItem ))
    {
	if ( mItems [ pItem ] < count )
	{
	    count = mItems [ pItem ];
	}

	mItems[ pItem ] -= count;

	if ( count > 1 )
	    os << " x" << count;
    }
#ifndef NDEBUG
    else
    {
	std::cerr <<  " An attempt was made to take an item you didn't have.";
    }
#endif

    pApplication->say("Item Taken", os.str());

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

uint Party::getSelectedCharacterIndex() const
{
    return 0;
}

uint Party::getCasterCharacterIndex() const
{
    return 0;
}

ICharacter * Party::getCharacter(uint index) const 
{
    return NULL;
}

ICharacter * Party::getSelectedCharacter() const 
{
    return NULL;
}

ICharacter * Party::getCasterCharacter() const 
{
    return NULL;
}






