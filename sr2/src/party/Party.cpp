
#include "Party.h"
#include "Level.h"
#include "Character.h"
#include "ItemManager.h"
#include <sstream>
#include "IParty.h"


using StoneRing::Party;
using StoneRing::IParty;
using StoneRing::ICharacter;
using StoneRing::ItemRef;
using StoneRing::Item;
using StoneRing::Character;

Party::Party():m_nGold(0)
{
}

Party::~Party()
{
}


bool Party::GetGold() const
{
    return m_nGold;
}


bool Party::HasItem(ItemRef *pItemRef, uint count) const
{
    Item * pItem = pItemRef->GetItem();
    return HasItem(pItem,count);
}

bool Party::DidEvent(const std::string &event) const
{
    return m_events.count(event) != 0;
}


void Party::DoEvent(const std::string &name, bool bRemember)
{
    if(bRemember)
        m_events.insert ( name );
}


void Party::GiveItem(ItemRef *pItemRef, uint count)
{
    Item * pItem = pItemRef->GetItem();

    GiveItem(pItem,count);
}

void Party::TakeItem(ItemRef *pItemRef, uint count)
{
    Item * pItem = pItemRef->GetItem();

    TakeItem(pItem,count);
}

bool Party::HasItem(Item * pItem,uint count) const
{
    std::map<std::string,ItemEntry>::const_iterator iter = m_items.find(pItem->GetName());    
    if(iter != m_items.end() && iter->second.m_count >= count)
        return true;
    else return false;
}

bool Party::GiveItem(Item *pItem, uint count)
{
    std::map<std::string,ItemEntry>::iterator iter = m_items.find(pItem->GetName());
    if(iter == m_items.end()){
        ItemEntry entry;
        entry.m_pItem = pItem;
        entry.m_count = count;
        m_items[pItem->GetName()] = entry;
    }else {
        iter->second.m_count += count;
    }  

    if( iter->second.m_count > pItem->GetMaxInventory())
    {
        iter->second.m_count = pItem->GetMaxInventory();
        return false;
    }
    
    return true;

}

bool Party::TakeItem(Item *pItem, uint count)
{
    std::map<std::string,ItemEntry>::iterator iter = m_items.find(pItem->GetName());
    if( iter != m_items.end() )
    {
        if ( iter->second.m_count <= count )
        {
            m_items.erase( iter );
	    return true;
        }

        iter->second.m_count -= count;

    }
    else
    {
        return false;
    }
    return true;
}

void Party::GiveGold(int amount)
{

    if(amount <0 )
    {
        // They are taking..
        if(m_nGold < amount)
            throw CL_Exception("Attempt to take more gold than we had.");
    }

    m_nGold += amount;
}


uint Party::GetCharacterCount() const
{
    uint count = m_characters.size();
    return count;
}


ICharacter * Party::GetCharacter(uint index) const
{
    return m_characters[index];
}


void Party::AddCharacter(Character *pCharacter)
{
    m_characters.push_back(pCharacter);
}

void Party::RemoveCharacter(const std::string &name)
{

}

Character * Party::GetMapCharacter()const
{
    if(m_characters.size())
    {
        return m_characters[0];
    }
    else
    {
        return NULL;
    }
}

void Party::IterateItems( StoneRing::ItemVisitor& func )
{
    for(std::map<std::string,ItemEntry>::const_iterator iter= m_items.begin();
	iter != m_items.end(); iter++)
	{
	    func(iter->second.m_pItem,iter->second.m_count);
	}
}









