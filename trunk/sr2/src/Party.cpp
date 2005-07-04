
#include "Party.h"
#include "Level.h"

using StoneRing::Party;




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

bool Party::hasItem(Item::eItemType type, const std::string &item, uint count) const
{

    Item theitem;
    
    theitem.setName ( item );
    theitem.setItemType ( type );
    
    
    return mItems.count(theitem) && mItems.find(theitem)->second >= count;
}

bool Party::hasItem(ItemRef *pItemRef, uint count) const
{
    return hasItem ( pItemRef->getItemType(), pItemRef->getItemName(), count );
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
    Item item;

    item.setName ( pItemRef->getItemName() );
    item.setItemType ( pItemRef->getItemType() );

    if(mItems.count(item ))
    {
	mItems[ item ]+=count;
    }
    else
    {
	mItems[ item ] = count;
    }

    IApplication * pApplication = IApplication::getInstance();

    pApplication->say("Item received!", item.getName());


}

void Party::takeItem(ItemRef *pItemRef, uint count)
{
    Item item;

    item.setName ( pItemRef->getItemName() );
    item.setItemType ( pItemRef->getItemType() );

    if(mItems.count(item ))
    {
	mItems[ item ]-=count;

	if(mItems[item] == 0 )
	{
	    // We have none left. Take it out of the map entirely.
	    mItems.erase ( item );
	}
	else if ( mItems[item] < 0)
	{
	    throw CL_Error("Bogus! Tried to take more of item " + item.getName() + " than we had.");
	}
    }
    else
    {
	throw CL_Error("Bogus! Someone tried to take an item we didn't have. Name = " + item.getName());
    }

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

void Party::modifyAttribute(const std::string &attribute, int add, const std::string &target)
{
#ifndef NDEBUG
    std::cout << "Modify Attribue: " << attribute << " by " << add << std::endl;
#endif
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
