

#ifndef SR_PARTY_H
#define SR_PARTY_H

#include "Item.h"
#include <string>
#include <set>
#include <map>
#include <fstream>
#ifdef _WIN32
#include "SteelType.h"
#else
#include "steel/SteelType.h"
#endif
#include "sr_defines.h"
#include "ICharacter.h"
#include "ICharacterGroup.h"

namespace StoneRing{

    class ItemRef;
    class Character;
    class Omega;

    class Party : public ICharacterGroup
    {
    public:
        Party();
        ~Party();

        virtual uint GetGold() const;
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
        virtual Character*  RemoveCharacter(const std::string &name);
        virtual Character*  GetMapCharacter()const;
		virtual void IterateItems( ItemVisitor & f);
        virtual uint GetMinutesPlayed()const;
        virtual bool EquipOmega(uint slot, Omega*);
        virtual void UnequipOmega(uint slot);
        virtual Omega* GetOmega(uint slot);
        virtual double GetCommonAttribute(ICharacter::eCommonAttribute attr)const;
        virtual bool   GetCommonToggle(ICharacter::eCommonAttribute attr)const;
        virtual double GetStatusEffectModifier(const std::string &statuseffect)const;         
        virtual double GetCharacterAttributeAdd(ICharacter::eCharacterAttribute attr)const;
        virtual double GetCharacterAttributeMultiplier(ICharacter::eCharacterAttribute attr)const;
        virtual bool   GetCharacterAttributeToggle(ICharacter::eCharacterAttribute attr, bool current)const;
        // ICharacterGroup interface
        virtual uint GetCharacterCount() const ;
        virtual ICharacter * GetCharacter(uint index) const ;
		
		void Clear(); // Start fresh, as in after a game over

        void Serialize(std::ostream & out);
        void Deserialize(std::istream & in);
        
      private:
          
          struct ItemEntry {
              ItemEntry() {
                  m_pItem = NULL;
                  m_count = 0;
              }
              Item* m_pItem;
              int m_count;
          };

          std::vector<Character*> m_characters;
          std::set<std::string> m_events;
          std::map<std::string,ItemEntry> m_items;
          std::map<uint,Omega*> m_omegas;
          uint m_nGold;
          uint m_nMinutes;

    };


}


#endif




