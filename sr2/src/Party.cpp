
#include "Party.h"
#include "Level.h"
#include "Character.h"

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

bool Party::hasItem(Item::eItemType type, const std::string &item, uint count) const
{

    return true;
}

bool Party::hasItem(ItemRef *pItemRef, uint count) const
{
    return true;
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
#if 0
    IApplication * pApplication = IApplication::getInstance();

    pApplication->say("Item received!", item.getName());
#endif

}

void Party::takeItem(ItemRef *pItemRef, uint count)
{




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
