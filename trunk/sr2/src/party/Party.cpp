
#include "Party.h"
#include "Level.h"
#include "Character.h"
#include "ItemManager.h"
#include <sstream>
#include "Party.h"
#include "GeneratedWeapon.h"
#include "GeneratedArmor.h"
#include "UniqueWeapon.h"
#include "UniqueArmor.h"
#include "WeaponType.h"
#include "ArmorType.h"
#include "WeaponClass.h"
#include "ArmorClass.h"
#include "CharacterManager.h"
#include "Omega.h"

using StoneRing::Party;
using StoneRing::ICharacter;
using StoneRing::ItemRef;
using StoneRing::Item;
using StoneRing::Character;
using StoneRing::Omega;

Party::Party():m_nGold(0)
{
}

Party::~Party()
{
}


uint Party::GetGold() const
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
		iter = m_items.find(pItem->GetName());
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
        if(m_nGold + amount < 0)
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

bool Party::EquipOmega ( uint slot, StoneRing::Omega* pOmega)
{
    std::map<uint,Omega*>::const_iterator it = m_omegas.find(slot);
    if(it != m_omegas.end()){
        // They already have somethign in this slot, move it into items
        GiveItem(it->second,1);
    }else{
        if(m_omegas.size() >= GetCommonAttribute(ICharacter::CA_IDOL_SLOTS))
            return false;
    }
    m_omegas[slot] = pOmega;
    TakeItem(pOmega,1);
    return true;
}

double Party::GetCommonAttribute ( ICharacter::eCommonAttribute attr ) const
{
    double value = 1.0;
    for(std::map<uint,Omega*>::const_iterator iter = m_omegas.begin();
        iter != m_omegas.end(); iter++){
        value *= iter->second->GetAttributeMultiplier(iter->first);
    }
    for(std::map<uint,Omega*>::const_iterator iter = m_omegas.end();
        iter != m_omegas.end(); iter++){
        value += iter->second->GetAttributeAdd(iter->first);
    }
    // TODO: eventually have Characters be able to modify common attributes, then process those as well
    return value;
}

bool Party::GetCommonToggle ( ICharacter::eCommonAttribute attr ) const
{
    return false; // TODO: This
}

double Party::GetCharacterAttributeAdd ( ICharacter::eCharacterAttribute attr ) const
{
    double value = 0.0;
    for(std::map<uint,Omega*>::const_iterator iter = m_omegas.end();
        iter != m_omegas.end(); iter++){
        value += iter->second->GetAttributeAdd(attr);
    }
    return value;
}

double Party::GetCharacterAttributeMultiplier ( ICharacter::eCharacterAttribute attr ) const
{
    double value = 1.0;
    for(std::map<uint,Omega*>::const_iterator iter = m_omegas.begin();
        iter != m_omegas.end(); iter++){
        value *= iter->second->GetAttributeMultiplier(attr);
    }
    return value;
}

bool Party::GetCharacterAttributeToggle ( ICharacter::eCharacterAttribute attr, bool current ) const
{
    bool value = current;
    for(std::map<uint,Omega*>::const_iterator iter = m_omegas.begin();
        iter != m_omegas.end(); iter++){
        value = iter->second->GetAttributeToggle(attr,value);        
    }
    return value;
}


StoneRing::Omega* Party::GetOmega ( uint slot )
{
    std::map<uint,Omega*>::const_iterator it = m_omegas.find(slot);
    if(it != m_omegas.end())
        return it->second;
    else return NULL;
}

double Party::GetStatusEffectModifier ( const std::string& statuseffect ) const
{
    double chance = 1.0;
    for(std::map<uint,Omega*>::const_iterator iter = m_omegas.begin();
        iter != m_omegas.end(); iter++){
        chance += iter->second->GetStatusEffectModifier(statuseffect);
    }
    return chance;
}

void Party::UnequipOmega ( uint slot )
{
    std::map<uint,Omega*>::iterator it = m_omegas.find(slot);
    if(it != m_omegas.end()){
        GiveItem(it->second,1);
        m_omegas.erase(it);
    }
}


void Party::Serialize ( std::ostream& out )
{
    uint event_count = m_events.size();
    out.write((char*)&event_count,sizeof(event_count));
    for(std::set<std::string>::const_iterator iter = m_events.begin();
        iter != m_events.end(); iter++)
        {
            WriteString(out,*iter);
        }
    uint items_size = m_items.size();
    out.write((char*)&items_size,sizeof(uint));
    for(std::map<std::string,ItemEntry>::const_iterator iter = m_items.begin();
        iter != m_items.end(); iter++)
        {
            out.write((char*)&iter->second.m_count,sizeof(int));
            ItemManager::SerializeItem(out,iter->second.m_pItem);
       }
        
    out.write((char*)&m_nGold,sizeof(uint));
    
    uint char_count = m_characters.size();
    out.write((char*)&char_count,sizeof(uint));
    for(int i=0;i<m_characters.size();i++){
        WriteString(out,m_characters[i]->GetName());
        m_characters[i]->Serialize(out);
    }
    
    uint minutes = GetMinutesPlayed();
    out.write((char*)&minutes,sizeof(uint));
    
    uint omega_count = m_omegas.size();
    out.write((char*)&omega_count,sizeof(uint));
    for(std::map<uint,Omega*>::const_iterator iter = m_omegas.begin(); iter != m_omegas.end(); iter++){
        ItemManager::SerializeItem(out,iter->second);
    }
}

void Party::Deserialize ( std::istream& in )
{
    m_events.clear();
    m_items.clear();
    m_characters.clear();
    uint event_size;
    in.read((char*)&event_size,sizeof(uint));
    for(int i=0;i<event_size;i++){
        std::string event_name = ReadString(in);
        DoEvent(event_name,true);
    }
    uint item_size;
    in.read((char*)&item_size,sizeof(uint));
    for(int i=0;i<item_size;i++){
        int item_count;
        in.read((char*)&item_count,sizeof(int));
        GiveItem(ItemManager::DeserializeItem(in),item_count);
    }    
    in.read((char*)&m_nGold,sizeof(uint));
    
    uint char_count;
    in.read((char*)&char_count,sizeof(uint));
    for(int i=0;i<char_count;i++){
        std::string character = ReadString(in);
        Character * pChar = IApplication::GetInstance()->GetCharacterManager()->GetCharacter(character);
        pChar->Deserialize(in);
        m_characters.push_back(pChar);
    }
    
    in.read((char*)&m_nMinutes,sizeof(uint));
    
    uint omega_count;
    in.read((char*)&omega_count,sizeof(uint));
    for(int i=0;i<omega_count;i++){
        Omega* pOmega = dynamic_cast<Omega*>(ItemManager::DeserializeItem(in));
        if(pOmega){
            m_omegas[i] = pOmega;
        }
    }
}

uint Party::GetMinutesPlayed() const
{
    return m_nMinutes + IApplication::GetInstance()->GetMinutesPlayed();
}





