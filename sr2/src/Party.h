

#ifndef SR_PARTY_H
#define SR_PARTY_H

#include "Item.h"
#include <string>
#include <set>
#include <map>
#include "IParty.h"

#ifndef SR_UINT
#define SR_UINT
typedef unsigned int uint;
#endif

namespace StoneRing{

class ItemRef;	

 class Party : public IParty
{
 public:
    Party();
    ~Party();

    virtual bool getGold() const;
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

    // ICharacterGroup interface
    virtual uint getCharacterCount() const ;
    virtual uint getSelectedCharacterIndex() const;
    virtual ICharacter * getCharacter(uint index) const ;
    virtual ICharacter * getSelectedCharacter() const ;

    void setLevelX(uint x);
    void setLevelY(uint y);
    

 private:

    std::set<std::string> mEvents;

    std::map<Item*,int> mItems;

    uint mX;
    uint mY;
    uint mnGold;

};


};


#endif
