

#ifndef SR_IPARTY_H
#define SR_IPARTY_H

#include "Item.h"
#include "Character.h"
#include <string>
#include <ClanLib/display.h>


namespace StoneRing{


    class ItemRef;  

    // A Party interface defines the interface
    // to the player party. It is  a specialization of charactergroup
    // which includes monster parties.
    class IParty : public ICharacterGroup
    {
    public:
        virtual ~IParty(){}

        virtual bool GetGold() const=0;
        virtual bool HasItem(ItemRef *pItemRef, uint count =1 )const=0;
		virtual bool DidEvent(const std::string &event) const=0;
        virtual void DoEvent(const std::string &name, bool bRemember)=0;
        virtual void GiveItem(ItemRef * pItemRef, uint count = 1)=0;
        virtual void TakeItem(ItemRef * pItemRef, uint count = 1)=0;
        virtual void GiveGold(int amount)=0;
        virtual void AddCharacter(Character *pCharacter)=0;
        virtual void RemoveCharacter(const std::string &name)=0;
        virtual Character * GetMapCharacter()const=0;

        // ICharacterGroup interface
        virtual uint GetCharacterCount() const = 0;
        virtual ICharacter * GetCharacter(uint index) const = 0;

    private:


    };


};


#endif




