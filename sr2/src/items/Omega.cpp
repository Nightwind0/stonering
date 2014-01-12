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


#include "Omega.h"
#include "AttributeModifier.h"
#include "StatusEffectModifier.h"
#include "Description.h"

namespace StoneRing  {
    
Omega::Omega()
{

}

Omega::~Omega()
{

}

// TODO: Create a base class for equipment and omegas that shares this functionality
double Omega::GetAttributeMultiplier(uint attr) const
{
    double multiplier = 1.0;
    for(AttributeModifierSet::const_iterator iter = m_attr_mods.lower_bound(attr);
        iter != m_attr_mods.upper_bound(attr);iter++)
    {
        if(iter->second->GetType() == AttributeModifier::EMULTIPLY)
            multiplier *= iter->second->GetMultiplier();
    }

    return multiplier;
}

double Omega::GetAttributeAdd(uint attr)const
{
    double add = 0.0;
    for(AttributeModifierSet::const_iterator iter = m_attr_mods.lower_bound(attr);
        iter != m_attr_mods.upper_bound(attr);iter++)
    {
        if(iter->second->GetType() == AttributeModifier::EADD)
            add += iter->second->GetAdd();
    }

    return add;
}

bool Omega::GetAttributeToggle ( uint i_attr, bool current ) const
{
    ICharacter::eCharacterAttribute attr = static_cast<ICharacter::eCharacterAttribute>(i_attr);
    double base = current;
    for(AttributeModifierSet::const_iterator iter = m_attr_mods.lower_bound(attr);
        iter != m_attr_mods.upper_bound(attr); iter++)
        {
            if(iter->second->GetType() == AttributeModifier::ETOGGLE)
                if(ICharacter::ToggleDefaultTrue(attr))
                    base = base && iter->second->GetToggle(); 
                else 
                    base = base || iter->second->GetToggle();
        }
        
    return base;
}

double Omega::GetStatusEffectModifier ( const std::string& statuseffect ) const
{
    std::pair<StatusEffectModifierSet::const_iterator, 
                StatusEffectModifierSet::const_iterator> bounds = m_status_effect_mods.equal_range(statuseffect);
    double modifier = 0.0;
    for(StatusEffectModifierSet::const_iterator iter = bounds.first; 
        iter != bounds.second; iter++)
    {
        modifier += iter->second->GetModifier();
    }

    return modifier;
}


uint Omega::GetValue() const
{
    return m_value;
}

bool Omega::handle_element ( Element::eElement element, Element* pElement )
{
    if(NamedItemElement::handle_element ( element, pElement ))
        return true;
    
    switch(element){
        case EDESCRIPTION:
            m_desc = dynamic_cast<Description*>(pElement)->GetText();
            delete pElement;
            return true;
        case EATTRIBUTEMODIFIER:{
            AttributeModifier* am = dynamic_cast<AttributeModifier*>(pElement);
            m_attr_mods.insert(std::pair<uint,AttributeModifier*>(am->GetAttribute(),am));
            return true;
        }
        case ESTATUSEFFECTMODIFIER:{
            StatusEffectModifier* em = dynamic_cast<StatusEffectModifier*>(pElement);
            m_status_effect_mods.insert(std::pair<std::string,StatusEffectModifier*>(em->GetStatusEffect()->GetName(),em));
            return true;
        }
        
        
    }
    
    return false;
}

void Omega::load_attributes ( clan::DomNamedNodeMap attributes )
{
    NamedItemElement::load_attributes ( attributes );
    m_value = get_required_int("value",attributes);
}

void Omega::load_finished()
{
    StoneRing::NamedItemElement::load_finished();
}

std::string Omega::GetName() const
{
    return NamedItemElement::GetName();
}
uint Omega::GetMaxInventory() const
{
    return NamedItemElement::GetMaxInventory();
}

clan::Image Omega::GetIcon() const
{
    return NamedItemElement::GetIcon();
}

bool Omega::operator== ( const StoneRing::ItemRef& ref )
{
    if(ref.GetType() == ItemRef::NAMED_ITEM) return false;
    
    if(ref.GetItemName() != GetName()) return false;
    
    return true;
}



}