

#ifndef SR_PARTY_H
#define SR_PARTY_H

#include "Item.h"
#include <string>
#include <set>
#include <map>
#include "SteelType.h"
#include "IParty.h"
#include "sr_defines.h"


namespace StoneRing{

    class ItemRef;  
    class Character;

    class Party : public IParty
    {
    public:
        Party();
        ~Party();
  
        virtual bool GetGold() const;
        virtual bool HasItem(ItemRef *pItemRef, uint count = 1 )const;
        virtual bool DidEvent(const std::string &event) const;
        virtual void DoEvent(const std::string &event, bool bRemember);
        virtual void GiveItem(ItemRef * pItemRef, uint count =1);
        virtual void TakeItem(ItemRef * pItemRef, uint count =1);
        virtual void GiveGold(int amount);
        virtual bool HasItem(Item *pItem, uint count) const;
        virtual bool GiveItem(Item *pItem, uint count);
        virtual bool TakeItem(Item *pItem, uint count);
        virtual void AddCharacter(Character *pCharacter);
        virtual void RemoveCharacter(const std::string &name);
        virtual Character * GetMapCharacter()const;

        // ICharacterGroup interface
        virtual uint GetCharacterCount() const ;
        virtual ICharacter * GetCharacter(uint index) const ;


      private:

          std::vector<Character*> m_characters;
          std::set<std::string> m_events;
          std::map<Item*,int> m_items;
          uint m_nGold;

    };


};


#endif




