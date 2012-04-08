#if 0 

#ifndef SR_Party_H
#define SR_Party_H

#include "Item.h"
#include "Character.h"
#include <string>
#include <ClanLib/display.h>
#include "ICharacterGroup.h"

namespace StoneRing{


    class ItemRef;
    class Omega;
  

    // A Party interface defines the interface
    // to the player party. It is  a specialization of charactergroup
    // which includes monster parties.
    class Party : public ICharacterGroup
    {
    public:
        virtual ~Party(){}

        virtual uint GetGold() const=0;
        virtual bool HasItem(ItemRef *pItemRef, uint count =1 )const=0;
	virtual bool DidEvent(const std::string &event) const=0;
        virtual void DoEvent(const std::string &name, bool bRemember)=0;
        virtual void GiveItem(ItemRef * pItemRef, uint count = 1)=0;
        virtual void TakeItem(ItemRef * pItemRef, uint count = 1)=0;
	virtual bool GiveItem(Item *pItem, uint count)=0;
	virtual bool TakeItem(Item *pItem, uint count)=0;
        virtual void GiveGold(int amount)=0;
        virtual void AddCharacter(Character *pCharacter)=0;
        virtual void RemoveCharacter(const std::string &name)=0;
        virtual Character * GetMapCharacter()const=0;
	virtual void IterateItems( ItemVisitor & f)=0;
        virtual uint GetMinutesPlayed()const=0;
        virtual bool EquipOmega(uint slot, Omega*)=0;
        virtual void UnequipOmega(uint slot)=0;
        virtual Omega* GetOmega(uint slot)=0;
        virtual double GetCommonAttribute(ICharacter::eCommonAttribute attr)const=0;
        virtual bool   GetCommonToggle(ICharacter::eCommonAttribute attr)const=0;
        virtual double GetCharacterAttributeAdd(ICharacter::eCharacterAttribute attr)const=0;
        virtual double GetCharacterAttributeMultiplier(ICharacter::eCharacterAttribute attr)const=0;
        virtual bool   GetCharacterAttributeToggle(ICharacter::eCharacterAttribute attr, bool current)const=0;
        virtual double GetStatusEffectModifier(const std::string &statuseffect)const=0; 
        // ICharacterGroup interface
        virtual uint GetCharacterCount() const = 0;
        virtual ICharacter * GetCharacter(uint index) const = 0;

    private:


    };


}


#endif


#endif

