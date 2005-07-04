
#include "Party.h"
#include "Level.h"

using StoneRing::Party;




Party::Party():mX(0),mY(0)
{
}

Party::~Party()
{
}


bool Party::getGold() const
{
	return 0;
}

bool Party::hasItem(Item::eItemType type, const std::string &item) const
{
	return false;
}

bool Party::hasItem(ItemRef *pItemRef) const
{
	return hasItem ( pItemRef->getItemType(), pItemRef->getItemName() );
}

bool Party::didEvent(const std::string &event) const
{
	return false;
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
#ifndef NDEBUG
    std::cout << "Do Event: " << name << std::endl;
#endif
}


void Party::giveItem(ItemRef *pItemRef)
{
}

void Party::takeItem(ItemRef *pItemRef)
{
}

void Party::giveGold(int amount)
{
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
