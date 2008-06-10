
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
    if( m_items.count ( pItem ) && m_items.find( pItem )->second >= count)
        return true;
    else return false;
    
}

bool Party::GiveItem(Item *pItem, uint count)
{
   
    if( !m_items.count(pItem) )
    {
        m_items [ pItem ] = 0;
    }
    
    
    if( m_items [ pItem ] + count <= pItem->GetMaxInventory())
    {
        m_items [ pItem ] += count;
    }
    else 
    {
        count = pItem->GetMaxInventory() - m_items[ pItem ];
    
        if( count < 1 )
        {
            return false;
        }
        else 
        {
            m_items [ pItem ] += count;

        }
    }
    
    return true;

}

bool Party::TakeItem(Item *pItem, uint count)
{
    if( m_items.count(pItem ))
    {
        if ( m_items [ pItem ] < count )
        {
            count = m_items [ pItem ];
        }

        m_items[ pItem ] -= count;

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
            throw CL_Error("Attempt to take more gold than we had.");
    }

    m_nGold += amount;
}


uint Party::GetCharacterCount() const 
{
    uint count = m_characters.size();
    return count;
}

uint Party::GetTargetCharacterIndex() const
{
    return 0;
}

uint Party::GetActorCharacterIndex() const
{
    return 0;
}

ICharacter * Party::GetCharacter(uint index) const 
{
    return m_characters[index];
}

ICharacter * Party::GetTargetCharacter() const 
{
    return NULL;
}

ICharacter * Party::GetActorCharacter() const 
{
    return NULL;
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










