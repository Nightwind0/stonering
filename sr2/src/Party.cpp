
#include "Party.h"

using StoneRing::Party;

Party * Party::mInstance;


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


Party * Party::getInstance()
{

	if( mInstance == NULL)
		mInstance = new Party();
	

	return mInstance;

}
