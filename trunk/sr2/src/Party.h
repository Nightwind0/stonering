

#ifndef SR_PARTY_H
#define SR_PARTY_H

#include "Item.h"
#include <string>
#include <set>
#include <map>
#include "IParty.h"
#include "sr_defines.h"


namespace StoneRing{

class ItemRef;	

 class Party : public IParty
{
 public:
    Party();
    ~Party();


    virtual eDirection getDirection()const;
    virtual void move();
    virtual void changeDirection(eDirection);
    virtual void resetPosition(uint levelX, uint levelY);
    
    virtual bool getGold() const;
    virtual bool hasItem(ItemRef *pItemRef, uint count = 1 )const;
    virtual bool didEvent(const std::string &event) const;
    virtual uint getLevelX() const;
    virtual uint getLevelY() const;
    virtual uint getCellWidth() const;
    virtual uint getCellHeight() const;
    virtual uint getCellX() const;
    virtual uint getCellY() const;
    virtual bool isAligned() const;
    virtual void doEvent(const std::string &event, bool bRemember);
    virtual void giveItem(ItemRef * pItemRef, uint count =1);
    virtual void takeItem(ItemRef * pItemRef, uint count =1);
    virtual void giveGold(int amount);

    // ICharacterGroup interface
    virtual uint getCharacterCount() const ;
    virtual uint getSelectedCharacterIndex() const;
    virtual uint getCasterCharacterIndex() const;
    virtual ICharacter * getCharacter(uint index) const ;
    virtual ICharacter * getSelectedCharacter() const ;
    virtual ICharacter * getCasterCharacter() const ;

    void setLevelX(uint x);
    void setLevelY(uint y);
    

 private:
	eDirection meDirection;

    std::set<std::string> mEvents;

    std::map<Item*,int> mItems;

    uint mX;
    uint mY;
    uint mnGold;

};


};


#endif
