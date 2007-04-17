

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
  
        virtual bool getGold() const;
        virtual bool hasItem(ItemRef *pItemRef, uint count = 1 )const;
        virtual bool didEvent(const std::string &event) const;
        virtual void doEvent(const std::string &event, bool bRemember);
        virtual void giveItem(ItemRef * pItemRef, uint count =1);
        virtual void takeItem(ItemRef * pItemRef, uint count =1);
        virtual void giveGold(int amount);
        virtual bool hasItem(Item *pItem, uint count) const;
        virtual bool giveItem(Item *pItem, uint count);
        virtual bool takeItem(Item *pItem, uint count);

        // ICharacterGroup interface
        virtual uint getCharacterCount() const ;
        virtual uint getSelectedCharacterIndex() const;
        virtual uint getCasterCharacterIndex() const;
        virtual ICharacter * getCharacter(uint index) const ;
        virtual ICharacter * getSelectedCharacter() const ;
        virtual ICharacter * getCasterCharacter() const ;

  

    private:

        std::set<std::string> mEvents;

        std::map<Item*,int> mItems;

        uint mnGold;

    };


};


#endif




