

#ifndef SR_PARTY_H
#define SR_PARTY_H

#include "Item.h"
#include <string>
#include "IParty.h"

typedef unsigned int uint;


namespace StoneRing{

class ItemRef;	

 class Party : public IParty
{
 public:
    Party();
    ~Party();

    virtual bool getGold() const;
    virtual bool hasItem(Item::eItemType type, const std::string &item) const;
    virtual bool hasItem(ItemRef *pItemRef )const;
    virtual bool didEvent(const std::string &event) const;
    virtual uint getLevelX() const;
    virtual uint getLevelY() const;


 private:


};


};


#endif
