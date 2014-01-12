/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef OMEGA_H
#define OMEGA_H

#include "Item.h"
#include "NamedItem.h"

namespace StoneRing {

    
    class AttributeModifier;
    class StatusEffectModifier;
    
    
class Omega : public NamedItemElement, public Item
{
public:
    Omega();
    virtual ~Omega();
    
    typedef std::multimap<uint,AttributeModifier*> AttributeModifierSet;
    typedef std::multimap<std::string,StatusEffectModifier*> StatusEffectModifierSet;
    
    virtual Element::eElement WhichElement() const { return EOMEGA; }
    virtual Item::eItemType GetItemType() const { return Item::OMEGA; }
    virtual Item::eDropRarity GetDropRarity() const { return Item::NEVER; }
    virtual std::string GetDescription() const { return m_desc; }
    virtual std::string GetName() const;
    virtual uint GetMaxInventory() const;
    clan::Image GetIcon() const;


    // These next two do not apply to special or system items.
    virtual uint GetValue() const; // Price to buy, and worth when calculating drops.
    virtual uint GetSellValue() const { return m_value / 2.0; }
    
    double GetAttributeMultiplier(uint attr) const;
    double GetAttributeAdd(uint attr)const;
    bool   GetAttributeToggle(uint attr, bool current)const;
    
    double GetStatusEffectModifier(const std::string &statuseffect)const;   
    virtual bool operator == ( const ItemRef &ref );    
private:
    virtual bool handle_element(eElement element, Element * pElement );
    virtual void load_attributes(clan::DomNamedNodeMap attributes) ;
    virtual void load_finished();
    
    AttributeModifierSet m_attr_mods;
    StatusEffectModifierSet m_status_effect_mods;
    std::string m_desc;
    uint m_value;
};

}
#endif // OMEGA_H
