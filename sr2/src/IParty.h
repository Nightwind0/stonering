

#ifndef SR_IPARTY_H
#define SR_IPARTY_H

#include "Item.h"
#include <string>


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
    


 private:


};


};


#endif
