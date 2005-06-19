
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

void Party::doEvent(const std::string &name)
{
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
}