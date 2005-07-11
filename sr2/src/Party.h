

#ifndef SR_PARTY_H
#define SR_PARTY_H

#include "Item.h"
#include <string>
#include <set>
#include <map>
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
    virtual bool hasItem(Item::eItemType type, const std::string &item, uint count = 1) const;
    virtual bool hasItem(ItemRef *pItemRef, uint count = 1 )const;
    virtual bool didEvent(const std::string &event) const;
    virtual uint getLevelX() const;
    virtual uint getLevelY() const;
    virtual uint getWidth() const;
    virtual uint getHeight() const;
    virtual CL_Rect getCollisionRect() const;
    virtual CL_Rect getCollisionRect(uint atX, uint atY) const;
    virtual void doEvent(const std::string &event, bool bRemember);
    virtual void giveItem(ItemRef * pItemRef, uint count =1);
    virtual void takeItem(ItemRef * pItemRef, uint count =1);
    virtual void giveGold(int amount);
    virtual void modifyAttribute(const std::string &attribute, int add, float multiplier, IParty::eTarget target);
    virtual int getMaxAttribute(const std::string &attribute, eTarget target) const ;
    virtual int getMinAttribute(const std::string &attribute, eTarget target) const ;
    virtual int getAttribute(const std::string &attribute, eTarget target) const ;


    void setLevelX(uint x);
    void setLevelY(uint y);
    

 private:

    std::set<std::string> mEvents;

    uint mX;
    uint mY;
    uint mnGold;

};


};


#endif
