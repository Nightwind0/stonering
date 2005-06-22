

#ifndef SR_IPARTY_H
#define SR_IPARTY_H

#include "Item.h"
#include <string>
#include <ClanLib/display.h>

typedef unsigned int uint;


namespace StoneRing{


class ItemRef;	


class IParty
{
 public:


    virtual bool getGold() const=0;
    virtual bool hasItem(Item::eItemType type, const std::string &item) const=0;
    virtual bool hasItem(ItemRef *pItemRef )const=0;
    virtual bool didEvent(const std::string &event) const=0;
    virtual uint getLevelX() const=0;
    virtual uint getLevelY() const=0;
    virtual uint getWidth() const=0;
    virtual uint getHeight() const =0;
    virtual CL_Rect getCollisionRect() const=0;
    virtual CL_Rect getCollisionRect(uint atX, uint atY) const=0;
    virtual void doEvent(const std::string &name)=0;
    virtual void giveItem(ItemRef * pItemRef)=0;
    virtual void takeItem(ItemRef * pItemRef)=0;
    virtual void giveGold(int amount)=0;
    virtual void modifyAttribute(const std::string &attribute, int add, const std::string &target)=0;


 private:


};


};


#endif
