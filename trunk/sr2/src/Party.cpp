
#include "Party.h"
#include "Level.h"

using StoneRing::Party;




Party::Party()
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
	return 0;
}

uint Party::getLevelY() const
{
	return 0;
}


