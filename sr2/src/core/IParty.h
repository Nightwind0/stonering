

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


        virtual bool getGold() const=0;
        virtual bool hasItem(ItemRef *pItemRef, uint count =1 )const=0;
        virtual bool didEvent(const std::string &event) const=0;
        virtual void doEvent(const std::string &name, bool bRemember)=0;
        virtual void giveItem(ItemRef * pItemRef, uint count = 1)=0;
        virtual void takeItem(ItemRef * pItemRef, uint count = 1)=0;
        virtual void giveGold(int amount)=0;


        // ICharacterGroup interface
        virtual uint getCharacterCount() const = 0;
        virtual uint getSelectedCharacterIndex() const = 0;
        virtual ICharacter * getCharacter(uint index) const = 0;
        virtual ICharacter * getSelectedCharacter() const = 0;

    private:

    };

};


#endif




