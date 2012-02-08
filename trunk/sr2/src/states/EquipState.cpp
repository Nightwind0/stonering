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


#include "EquipState.h"
#include "SoundManager.h"
#include "GraphicsManager.h"
#include "MenuBox.h"
#include "Weapon.h"
#include "Armor.h"
#include <iomanip>



using namespace StoneRing;



void EquipState::EquipmentCollector::operator()(Item* pItem, int nCount)
{
    if(pItem->GetItemType() == Item::ARMOR ||
        pItem->GetItemType() == Item::WEAPON){
        Equipment * pEquipment = dynamic_cast<Equipment*>(pItem);
        ParameterList params;
        params.push_back ( ParameterListItem("$_Character",m_char) );
        if(pEquipment->GetSlot() & m_slot && 
            m_char->GetClass()->CanEquip(pEquipment)
            && pEquipment->EquipCondition(params)
          )
        {
            for(int i=0;i<nCount;i++)
                m_menu.AddOption(pEquipment);
        }
    }
}



EquipState::EquipState()
{

}

EquipState::~EquipState()
{

}

void EquipState::Init ( Character* pCharacter )
{
    m_pChar = pCharacter;

    
    m_desc_rect = GraphicsManager::GetRect(GraphicsManager::EQUIP, "desc");
    m_equipment_rect = GraphicsManager::GetRect(GraphicsManager::EQUIP, "equipment");
    m_slots_rect = GraphicsManager::GetRect(GraphicsManager::EQUIP, "slots");
    m_stats_rect = GraphicsManager::GetRect(GraphicsManager::EQUIP, "stats");
    CL_Pointf slotSize  = GraphicsManager::GetPoint(GraphicsManager::EQUIP, "slot_size");
    m_slot_size = CL_Sizef(slotSize.x,slotSize.y);
    
    m_slot_name_font = GraphicsManager::GetFont(GraphicsManager::EQUIP,"slot_name");
    m_slot_name_selected_font = GraphicsManager::GetFont(GraphicsManager::EQUIP,"slot_name_selected");
    m_slot_font = GraphicsManager::GetFont(GraphicsManager::EQUIP,"slots");
    
    m_equipment_font = GraphicsManager::GetFont(GraphicsManager::EQUIP,"equipment");
    m_equipment_selected_font = GraphicsManager::GetFont(GraphicsManager::EQUIP,"equipment_selected");
    m_stat_font = GraphicsManager::GetFont(GraphicsManager::EQUIP,"stat");
    m_stat_up_font = GraphicsManager::GetFont(GraphicsManager::EQUIP,"stat_up");
    m_stat_down_font = GraphicsManager::GetFont(GraphicsManager::EQUIP,"stat_down");
    m_stat_name_font = GraphicsManager::GetFont(GraphicsManager::EQUIP,"stat_name");
    m_desc_font = GraphicsManager::GetFont(GraphicsManager::EQUIP,"desc");
    
    m_no_equipment_icon = GraphicsManager::GetImage(GraphicsManager::EQUIP,"no_equipment");
    
    CL_Rectf eq_menuRect = m_equipment_rect;
    eq_menuRect.shrink ( GraphicsManager::GetMenuInset().x * 2, GraphicsManager::GetMenuInset().y * 2);
    
    m_equipment_menu.SetRect( eq_menuRect );
    m_equipment_menu.SetFont( m_equipment_font );
    m_equipment_menu.SetSelectedFont( m_equipment_selected_font );
    m_equipment_menu.SetRemoveIcon( m_no_equipment_icon );
    m_equipment_menu.SetHeightPerOption( m_no_equipment_icon.get_height() + 4);
    
    CL_Rectf stats_rect = m_stats_rect;
    stats_rect.shrink(GraphicsManager::GetMenuInset().x*2,GraphicsManager::GetMenuInset().y*2);
    
    m_pStatusBox = new StatusBox(stats_rect,m_stat_font,m_stat_up_font,m_stat_down_font,m_stat_name_font);
}


bool EquipState::DisableMappableObjects() const
{
    return true;
}

void EquipState::Draw ( const CL_Rect& screenRect, CL_GraphicContext& GC )
{
    MenuBox::Draw( GC, screenRect );
    MenuBox::Draw( GC, m_desc_rect, false );
    MenuBox::Draw( GC, m_slots_rect, false, CL_Pointf(0.0,0.0) );
    MenuBox::Draw( GC, m_equipment_rect, true, CL_Pointf(0.0,0.0) );
    MenuBox::Draw( GC, m_stats_rect, true, CL_Pointf(0.0,0.0) );
    
    draw_slots(GC);
    m_equipment_menu.Draw(GC);
    draw_stats(GC);
    draw_description(GC);
}

int EquipState::options_per_column() const
{
    CL_Rectf rect = m_slots_rect;
    rect.shrink(GraphicsManager::GetMenuInset().x*2,GraphicsManager::GetMenuInset().y*2);
   // CL_Draw::fill(gc,rect,CL_Colorf(0.5f,0.5f,0.5f,0.1f));
    
    return 1+rect.get_height() / m_slot_size.height;
}


void EquipState::fill_equipment_menu()
{
    m_equipment_menu.ClearOptions();
    
    IParty * party = IApplication::GetInstance()->GetParty();    
    EquipmentCollector collector(m_equipment_menu,m_pChar, m_slots[m_nSlot]);
    party->IterateItems(collector);
}

bool EquipState::offhand_available() const
{
    if(m_pChar->HasEquipment(Equipment::EHAND)){
        Equipment * pEq = m_pChar->GetEquipment(Equipment::EHAND);
        Weapon *pWeapon = dynamic_cast<Weapon*>(pEq);
        if(pWeapon){
            if(pWeapon->IsTwoHanded()) 
                return false;
        }
    }
    if(m_pChar->HasEquipment(Equipment::EOFFHAND)){
        Equipment * pEq = m_pChar->GetEquipment(Equipment::EOFFHAND);
        Weapon *pWeapon = dynamic_cast<Weapon*>(pEq);
        if(pWeapon){
            if(pWeapon->IsTwoHanded()) 
                return false;
        }
    }    
    
    return true;
}

void EquipState::draw_description ( CL_GraphicContext& gc )
{
    CL_Rectf rect = m_desc_rect;
    rect.shrink(GraphicsManager::GetMenuInset().x,GraphicsManager::GetMenuInset().y);
    if(m_eState == SELECT_EQUIPMENT){
        m_equipment_menu.Choose();
        Equipment * pEquipment = m_equipment_menu.GetSelection();
        if(pEquipment)
            draw_text(gc,m_desc_font,rect,pEquipment->GetDescription());
        else
            draw_text(gc,m_desc_font,rect,"Remove equipment");
    }
} 


void EquipState::draw_stats ( CL_GraphicContext& gc )
{
    if(m_eState == SELECT_EQUIPMENT){
        m_equipment_menu.Choose();
        m_pStatusBox->Draw(gc, true, m_pChar,m_pChar->HasEquipment(m_slots[m_nSlot])?m_pChar->GetEquipment(m_slots[m_nSlot]):NULL,
                           m_equipment_menu.GetSelection()
                          );
    }else{
        m_pStatusBox->Draw(gc,false, m_pChar,NULL,NULL);
    }
#if 0
    CL_Rectf rect = m_stats_rect;
    rect.shrink(GraphicsManager::GetMenuInset().x*2,GraphicsManager::GetMenuInset().y*2);
    CL_FontMetrics metrics = m_stat_name_font.get_font_metrics(gc);
    CL_Pointf offset(0,metrics.get_height());
    
       
    CL_Sizef statSize = m_stat_font.get_text_size(gc,"000000");

    m_equipment_menu.Choose(); // Doesn't equip it or anything
 
    Equipment * pOldEquipment = NULL;
    if(m_pChar->HasEquipment(m_slots[m_nSlot]))
        pOldEquipment = m_pChar->GetEquipment(m_slots[m_nSlot]);
    Equipment * pSelectedEquipment = m_equipment_menu.GetSelection();
    CL_Pointf point = rect.get_top_left();
    CL_Pointf statPoint = point;

    
    for(int i=Weapon::_FIRST_ATTR+1;i<Weapon::_LAST_ATTR;i++){
        Weapon::eAttribute attr = static_cast<Weapon::eAttribute>(i);
        statPoint = point;
        statPoint.x = rect.get_top_right().x - statSize.width * 3;       
        double old_value = m_pChar->GetEquippedWeaponAttribute(attr);        
        m_stat_name_font.draw_text(gc,point,Weapon::StringForAttribute(attr),Font::TOP_LEFT);
        m_stat_font.draw_text(gc,statPoint,FloatToString(old_value,6,2),Font::TOP_LEFT);
        if(m_eState == SELECT_EQUIPMENT){
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
        statPoint.x = rect.get_top_right().x - statSize.width * 3;       
        double old_value = m_pChar->GetEquippedArmorAttribute(attr);        
        m_stat_name_font.draw_text(gc,point,Armor::StringForAttribute(attr),Font::TOP_LEFT);
        m_stat_font.draw_text(gc,statPoint,FloatToString(old_value,6,2),Font::TOP_LEFT);
        if(m_eState == SELECT_EQUIPMENT){
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

    
    for(std::vector<ICharacter::eCharacterAttribute>::const_iterator iter = m_stats.begin();
        iter != m_stats.end(); iter++,point += offset)
        {
            m_stat_name_font.draw_text(gc,point,ICharacter::CAToLabel(*iter), Font::TOP_LEFT);
            CL_Pointf statPoint = point;
            statPoint.x = rect.get_top_right().x - statSize.width*3;
            std::ostringstream os;
            if(ICharacter::IsInteger(*iter))
                os << std::setw(6) << m_pChar->GetAttribute(*iter);
            else
                os << std::setw(6) << std::setprecision(2) << m_pChar->GetAttribute(*iter);
            m_stat_font.draw_text(gc,statPoint,os.str(),Font::TOP_LEFT);
            
            if(m_eState == SELECT_EQUIPMENT){
                std::ostringstream os;
                statPoint.x += statSize.width*2;
                Equipment * pEquipment = m_equipment_menu.GetSelection();
                double old_value = m_pChar->GetAttribute(*iter);
                double new_value = m_pChar->GetBaseAttribute(*iter);
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
                    os << std::setw(6) << std::setprecision(2) << new_value;

                newStatFont.draw_text(gc,statPoint,os.str(), Font::TOP_LEFT);                
            }
        }
#endif
        
}


void EquipState::draw_slots(CL_GraphicContext& gc)
{
    CL_Rectf rect = m_slots_rect;
    rect.shrink(GraphicsManager::GetMenuInset().x*2,GraphicsManager::GetMenuInset().y*2);
    
   // CL_Draw::fill(gc,rect,CL_Colorf(0.5f,0.5f,0.5f,0.1f));
    
    int opt_per_col = options_per_column();
    
    for(int i=0;i<m_slots.size(); i++){
        uint column = i / opt_per_col;
        CL_Pointf point(m_slot_size.width * column, (i % opt_per_col) *m_slot_size.height);
        point += rect.get_top_left();
        CL_Pointf middlePoint = point;
        middlePoint.y += m_slot_size.height / 2;
        
        // Draw icons
        CL_Image icon;
        if(m_pChar->HasEquipment(m_slots[i])){
            icon = m_pChar->GetEquipment(m_slots[i])->GetIcon();
        }else {
            icon = m_no_equipment_icon;
            if(m_slots[i] == Equipment::EOFFHAND && offhand_available())
                icon.set_alpha(1.0f);
            else if(m_slots[i] == Equipment::EOFFHAND)
                icon.set_alpha(0.1f);
        }
        
        icon.draw(gc,point.x, point.y + (m_slot_size.height - icon.get_height()) / 2);
        CL_Pointf namePoint = point;
        namePoint.x += icon.get_width() + 2;
        
        Font nameFont;
        if(i!=m_nSlot)
            nameFont = m_slot_name_font;
        else
            nameFont = m_slot_name_selected_font;
            
        nameFont.draw_text(gc,namePoint,Equipment::GetSlotName(m_slots[i]), Font::TOP_LEFT);
        
        if(m_pChar->HasEquipment(m_slots[i]))
            m_slot_font.draw_text(gc,point.x + icon.get_width() + 2,point.y + m_slot_size.height, m_pChar->GetEquipment(m_slots[i])->GetName(), Font::ABOVE);
        
    }
}


void EquipState::Finish()
{
    delete m_pStatusBox;
}

void EquipState::equipment_selected()
{

}

void EquipState::slot_selected()
{
    
}


void EquipState::HandleAxisMove ( const StoneRing::IApplication::Axis& axis, const StoneRing::IApplication::AxisDirection dir, float pos )
{
 
    if(m_eState == SELECT_SLOT){
         int skip_offset = 1;
        if(dir == IApplication::AXIS_DOWN){
            m_nSlot++;
            if(m_nSlot == m_slots.size())
                m_nSlot = m_slots.size()-1;
            SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);            
        }else if(dir == IApplication::AXIS_UP){
            if(m_nSlot >0)
                m_nSlot--;           
            skip_offset = -1;
            SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);    
        }else if(dir == IApplication::AXIS_LEFT){
            if(m_nSlot >= options_per_column())
                m_nSlot -= options_per_column();
            skip_offset = -1;
            SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);
        }else if(dir == IApplication::AXIS_RIGHT){
            if(m_nSlot + options_per_column() < m_slots.size())
                m_nSlot += options_per_column();
            SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);
        }
        // You can't select OFFHAND to equip if you're wielding a two-handed weapon
        if(m_slots[m_nSlot] == Equipment::EOFFHAND && !offhand_available()){
            if(m_nSlot + skip_offset < 0)
                skip_offset = 1;
            m_nSlot+=skip_offset;
            if(m_nSlot > m_slots.size())
                m_nSlot-=2;
          
        }
        fill_equipment_menu();
    }else if(m_eState == SELECT_EQUIPMENT)
    {
        if(dir == IApplication::AXIS_DOWN){
            m_equipment_menu.SelectDown();
        }else if(dir == IApplication::AXIS_UP){
            m_equipment_menu.SelectUp();
        }
    }
}

void EquipState::HandleButtonDown ( const StoneRing::IApplication::Button& button )
{
    StoneRing::State::HandleButtonDown ( button );
}

void EquipState::HandleButtonUp ( const StoneRing::IApplication::Button& button )
{
    StoneRing::State::HandleButtonUp ( button );
    if(m_eState == SELECT_SLOT){
        if(button == IApplication::BUTTON_CANCEL){
            m_bDone = true;
            SoundManager::PlayEffect(SoundManager::EFFECT_CANCEL);
        }else if(button == IApplication::BUTTON_CONFIRM){
            m_eState = SELECT_EQUIPMENT;
            m_equipment_menu.EnableSelection();
        }
        
    }else if(m_eState == SELECT_EQUIPMENT){
        if(button == IApplication::BUTTON_CANCEL){
            SoundManager::PlayEffect(SoundManager::EFFECT_CANCEL);
            m_eState = SELECT_SLOT;
            m_equipment_menu.DisableSelection();
        }else if(button == IApplication::BUTTON_CONFIRM){
            SoundManager::PlayEffect(SoundManager::EFFECT_SELECT_OPTION);
            m_equipment_menu.Choose();
            Equipment * pEquipment = m_equipment_menu.GetSelection();
            if(pEquipment == NULL){
                if(m_pChar->HasEquipment(m_slots[m_nSlot])){
                    Equipment * pEquip = m_pChar->GetEquipment(m_slots[m_nSlot]);
                    IApplication::GetInstance()->GetParty()->GiveItem(pEquip,1);
                    m_pChar->Unequip(m_slots[m_nSlot]);
                }
            }else{
                // First, take the item being equipped out of party inventory
                IApplication::GetInstance()->GetParty()->TakeItem(pEquipment,1);
                // Then, if there is already something in this slot, add it to party inventory
                if(m_pChar->HasEquipment(m_slots[m_nSlot])){
                    IApplication::GetInstance()->GetParty()->GiveItem(m_pChar->GetEquipment(m_slots[m_nSlot]),1);
                }
                // Then, put the item into the slot
                m_pChar->Equip(m_slots[m_nSlot],pEquipment);
            }
            m_eState = SELECT_SLOT;
        }        
        
    }
    
    if(button == IApplication::BUTTON_SELECT){
        SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);            
        m_pStatusBox->switchPage();
    }

    
}

bool EquipState::IsDone() const
{
    return m_bDone;
}

bool EquipState::LastToDraw() const
{
    return false;
}

void EquipState::MappableObjectMoveHook()
{

}


void EquipState::Start()
{
    m_bDone = false;
    m_eState = SELECT_SLOT;
    m_slots.clear();
    m_slots.push_back(Equipment::EHAND);
    m_slots.push_back(Equipment::EOFFHAND);
    m_slots.push_back(Equipment::EHEAD);
    m_slots.push_back(Equipment::EBODY);
    m_slots.push_back(Equipment::EHANDS);
    m_slots.push_back(Equipment::EFEET);
    m_slots.push_back(Equipment::EFINGER1);
    m_slots.push_back(Equipment::EFINGER2);
    m_nSlot = 0;
    fill_equipment_menu();
    m_equipment_menu.DisableSelection();

}

void EquipState::SteelCleanup ( SteelInterpreter* )
{
}

void EquipState::SteelInit ( SteelInterpreter* )
{
}





EquipState::EquipmentMenu::EquipmentMenu()
{

}


EquipState::EquipmentMenu::~EquipmentMenu()
{

}

void EquipState::EquipmentMenu::SetRect ( const CL_Rectf& rect )
{
    m_rect = rect;
}


void EquipState::EquipmentMenu::AddOption ( Equipment* pOption )
{
    m_options.push_back( pOption );
}

void EquipState::EquipmentMenu::ClearOptions()
{
    m_options.clear();
    m_options.push_back ( NULL ); // This is the "Remove" option
    m_selection = 0;
    Menu::reset_menu();
}

void EquipState::EquipmentMenu::draw_option ( int option, bool selected, float x, float y, CL_GraphicContext& gc )
{
    Equipment * pEquipment = m_options[option];
    Font font = (selected&&m_enable_selection)?m_selected_font:m_option_font;
    CL_FontMetrics metrics = font.get_font_metrics(gc);
    CL_Image icon;
    if(pEquipment == NULL){
        icon = m_remove_icon;
        icon.draw(gc,x,y);
        font.draw_text(gc,x + icon.get_width() + 2,y + (height_for_option(gc) - metrics.get_height())/2 ,"-- Remove --", Font::TOP_LEFT);
    }else{
        icon = pEquipment->GetIcon();
        icon.draw(gc,x,y);
        font.draw_text(gc,x + icon.get_width() + 2,y + (height_for_option(gc) - metrics.get_height())/2,pEquipment->GetName(), Font::TOP_LEFT);
    }
}


int EquipState::EquipmentMenu::get_option_count()
{
    return m_options.size();
}

CL_Rectf EquipState::EquipmentMenu::get_rect()
{
    return m_rect;
}

Equipment* EquipState::EquipmentMenu::GetSelection() const
{
    return m_options[m_selection];
}

int EquipState::EquipmentMenu::height_for_option ( CL_GraphicContext& gc )
{
    return m_height_per_option;
}
void EquipState::EquipmentMenu::SetHeightPerOption ( uint height )
{
    m_height_per_option = height;
}


void EquipState::EquipmentMenu::process_choice ( int selection )
{
    m_selection = selection;
}

void EquipState::EquipmentMenu::SetFont ( Font font )
{
    m_option_font = font;
}

void EquipState::EquipmentMenu::SetRemoveIcon ( const CL_Image& icon )
{
    m_remove_icon = icon;
}

void EquipState::EquipmentMenu::SetSelectedFont ( Font font )
{
    m_selected_font = font;
}


void EquipState::EquipmentMenu::DisableSelection()
{
    m_enable_selection = false;
}

void EquipState::EquipmentMenu::EnableSelection()
{
    m_enable_selection = true;
}









