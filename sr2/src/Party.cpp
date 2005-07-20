
#include "Party.h"
#include "Level.h"
#include "Character.h"
#include "ItemManager.h"
#include <sstream>

using StoneRing::Party;
using StoneRing::ICharacter;



Party::Party():mX(0),mY(0),mnGold(0)
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

uint Party::getWidth() const
{
    return 64;
}

uint Party::getHeight() const
{
    return 64;
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

ICharacter * Party::getCharacter(uint index) const 
{
    return NULL;
}

ICharacter * Party::getSelectedCharacter() const 
{
    return NULL;
}


CL_Rect Party::getCollisionRect(uint atX, uint atY) const
{
    
    int offX = getWidth() / 2;
    int offY = getHeight() / 2;

    int quartX = offX / 2;

    return CL_Rect(atX + quartX, atY + offY, 
		   atX + offX + quartX,
		   atY + offY + offY);


}

CL_Rect Party::getCollisionRect() const
{

    return getCollisionRect(getLevelX(), getLevelY());
}
