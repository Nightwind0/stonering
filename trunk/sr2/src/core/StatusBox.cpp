/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

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


#include "StatusBox.h"
#include "Equipment.h"
#include <iomanip>

using namespace StoneRing;


StatusBox::StatusBox ( const CL_Rectf& rect, const CL_Rectf& headerRect,
                       const StoneRing::Font& headerFont,
                       const StoneRing::Font& stat_font, 
                       const StoneRing::Font& stat_up_font, const StoneRing::Font& stat_down_font,
                        const StoneRing::Font& stat_name_font )
                        :m_rect(rect),m_header_rect(headerRect),m_header_font(headerFont),m_stat_font(stat_font),
                            m_stat_up_font(stat_up_font),m_stat_down_font(stat_down_font),m_stat_name_font(stat_name_font)
{
    m_nPage = 0;
    m_stats[0].push_back(ICharacter::CA_MAXHP);
    m_stats[0].push_back(ICharacter::CA_MAXMP);
    m_stats[0].push_back(ICharacter::CA_STR);
    m_stats[0].push_back(ICharacter::CA_DEF);
    m_stats[0].push_back(ICharacter::CA_DEX);
    m_stats[0].push_back(ICharacter::CA_EVD);
    m_stats[0].push_back(ICharacter::CA_MAG);
    m_stats[0].push_back(ICharacter::CA_RST);
    m_stats[0].push_back(ICharacter::CA_LCK);
    m_stats[0].push_back(ICharacter::CA_JOY);
    
    m_stats[1].push_back(ICharacter::CA_PIERCE_DEF);
    m_stats[1].push_back(ICharacter::CA_SLASH_DEF);
    m_stats[1].push_back(ICharacter::CA_BASH_DEF);
    m_stats[1].push_back(ICharacter::CA_FIRE_RST);
    m_stats[1].push_back(ICharacter::CA_WATER_RST);
    m_stats[1].push_back(ICharacter::CA_WIND_RST);
    m_stats[1].push_back(ICharacter::CA_EARTH_RST);
    m_stats[1].push_back(ICharacter::CA_HOLY_RST);
    m_stats[1].push_back(ICharacter::CA_DARK_RST);
}


StatusBox::~StatusBox()
{

}


Equipment::eSlot StatusBox::slot_for_equipment ( Equipment* pEquipment )
{
    return pEquipment->GetSlot();
}

void StatusBox::Draw ( CL_GraphicContext& gc, bool draw_comparison, Character* pChar, Equipment* pOldEquipment, Equipment* pEquipment )
{
    CL_FontMetrics metrics = m_stat_name_font.get_font_metrics(gc);
    CL_Pointf offset(0,metrics.get_height());   
       
    CL_Sizef statSize = m_stat_font.get_text_size(gc,"000000");

    Equipment * pSelectedEquipment = pEquipment;
    CL_Pointf point = m_rect.get_top_left();
    CL_Pointf statPoint = point;
   
    for(int i=Weapon::_FIRST_ATTR+1;i<Weapon::_LAST_ATTR;i++){
        Weapon::eAttribute attr = static_cast<Weapon::eAttribute>(i);
        statPoint = point;
        statPoint.x = m_rect.get_top_right().x - statSize.width * 3;       
        double old_value = pChar->GetEquippedWeaponAttribute(attr);        
        m_stat_name_font.draw_text(gc,point,Weapon::StringForAttribute(attr),Font::TOP_LEFT);
        m_stat_font.draw_text(gc,statPoint,FloatToString(old_value,6,2),Font::TOP_LEFT);
        if(draw_comparison){
            statPoint.x += statSize.width*2;
            std::ostringstream os;
            Weapon * pOldWeapon = dynamic_cast<Weapon*>(pOldEquipment);
            double new_value = old_value;//pOldWeapon?(old_value-pOldWeapon->GetWeaponAttribute(attr)):0.0;                
            if(pSelectedEquipment != NULL){ // Remove
                Weapon * pWeapon = dynamic_cast<Weapon*>(pSelectedEquipment);
                if(pWeapon){
                    new_value = old_value + pWeapon->GetWeaponAttribute(attr);
                }
            }
            if(pOldWeapon)
                new_value -= pOldWeapon->GetWeaponAttribute(attr);
            
            
            Font newStatFont = m_stat_font;
            if(new_value > old_value)
                newStatFont = m_stat_up_font;
            else if(new_value < old_value)
                newStatFont = m_stat_down_font;
            os << std::setw(6) << std::setprecision(2) << new_value;

            newStatFont.draw_text(gc,statPoint,os.str(), Font::TOP_LEFT);
        }
        point += offset;
    }
  
    for(int i=Armor::_FIRST_ATTR+1;i<Armor::_LAST_ATTR;i++){
        Armor::eAttribute attr = static_cast<Armor::eAttribute>(i);
        statPoint = point;
        statPoint.x = m_rect.get_top_right().x - statSize.width * 3;       
        double old_value = pChar->GetEquippedArmorAttribute(attr);        
        m_stat_name_font.draw_text(gc,point,Armor::StringForAttribute(attr),Font::TOP_LEFT);
        m_stat_font.draw_text(gc,statPoint,FloatToString(old_value,6,2),Font::TOP_LEFT);
        if(draw_comparison){
            statPoint.x += statSize.width*2;
            std::ostringstream os;
            Armor * pOldArmor = dynamic_cast<Armor*>(pOldEquipment);
            double new_value = old_value;//pOldWeapon?(old_value-pOldWeapon->GetWeaponAttribute(attr)):0.0;                
            if(pSelectedEquipment != NULL){ // Remove
                Armor * pArmor = dynamic_cast<Armor*>(pSelectedEquipment);
                if(pArmor){
                    new_value = old_value + pArmor->GetArmorAttribute(attr);
                }
            }
            if(pOldArmor)
                new_value -= pOldArmor->GetArmorAttribute(attr);
            
            
            Font newStatFont = m_stat_font;
            if(new_value > old_value)
                newStatFont = m_stat_up_font;
            else if(new_value < old_value)
                newStatFont = m_stat_down_font;
            os << std::setw(6) << std::setprecision(2) << new_value;

            newStatFont.draw_text(gc,statPoint,os.str(), Font::TOP_LEFT);
        }
        point += offset;
    }

    
    for(std::vector<ICharacter::eCharacterAttribute>::const_iterator iter = m_stats[m_nPage].begin();
        iter != m_stats[m_nPage].end(); iter++,point += offset)
        {
            m_stat_name_font.draw_text(gc,point,ICharacter::CAToLabel(*iter), Font::TOP_LEFT);
            CL_Pointf statPoint = point;
            statPoint.x = m_rect.get_top_right().x - statSize.width*3;
            std::ostringstream os;
            if(ICharacter::IsInteger(*iter))
                os << std::setw(6) << pChar->GetAttribute(*iter);
            else
                os << std::setw(6) << std::setprecision(0) << int(100.0 * pChar->GetAttribute(*iter));
            m_stat_font.draw_text(gc,statPoint,os.str(),Font::TOP_LEFT);
            
            if(draw_comparison){
                std::ostringstream os;
                statPoint.x += statSize.width*2;
                double old_value = pChar->GetAttribute(*iter);
                double new_value = pChar->GetAttributeWithoutEquipment(*iter, pOldEquipment);
                if(pEquipment){
                    new_value *= pEquipment->GetAttributeMultiplier(*iter);
                    new_value += pEquipment->GetAttributeAdd(*iter);
                }
                Font newStatFont = m_stat_font;
                if(new_value > old_value)
                    newStatFont = m_stat_up_font;
                else if(new_value < old_value)
                    newStatFont = m_stat_down_font;
                if(ICharacter::IsInteger(*iter))
                    os << std::setw(6) << (int)new_value;
                else
                    os << std::setw(6) << std::setprecision(0) <<  int(100.0 * new_value);

                newStatFont.draw_text(gc,statPoint,os.str(), Font::TOP_LEFT);                
            }
        }
        
        draw_text(gc,m_header_font,m_header_rect,"Press [Select] to switch view");
   
}

void StatusBox::switchPage()
{
    m_nPage = m_nPage==0?1:0;
}

