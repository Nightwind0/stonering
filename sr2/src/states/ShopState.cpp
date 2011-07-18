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


#include "ShopState.h"
#include "GraphicsManager.h"
#include "MenuBox.h"
#include <iomanip>


using namespace StoneRing;



void ShopState::ItemMenu::AddOption ( Item* pOption )
{
    m_options.push_back ( pOption );
}

void ShopState::ItemMenu::ClearOptions()
{
    m_options.clear();
}

void ShopState::ItemMenu::DisableSelection()
{
    m_enable_selection = false;
}

void ShopState::ItemMenu::draw_option ( int option, bool selected, float x, float y, CL_GraphicContext& gc )
{
    Item * pItem = m_options[option];
    Font font = m_option_font;
    if(IApplication::GetInstance()->GetParty()->GetGold() < pItem->GetValue()){
        font = m_unavailable_font;
    }
    
    if(selected)
        font = m_selected_font;
    
    CL_FontMetrics metrics = font.get_font_metrics(gc);
    CL_Image icon;
    icon = pItem->GetIcon();
    icon.draw(gc,x,y);
    font.draw_text(gc,x + icon.get_width() + 2,y + (height_for_option(gc) - metrics.get_height())/2,pItem->GetName(), Font::TOP_LEFT);
    std::ostringstream stream;
    stream << std::setw(7) << pItem->GetValue();
    float point_x = m_rect.get_top_right().x - font.get_text_size(gc,"0000000").width;
    m_price_font.draw_text(gc,point_x,y + (height_for_option(gc) - metrics.get_height())/2,stream.str(),Font::TOP_LEFT);
}

void ShopState::ItemMenu::EnableSelection()
{
    m_enable_selection = true;
}

int ShopState::ItemMenu::get_option_count()
{
    return m_options.size();
}

CL_Rectf ShopState::ItemMenu::get_rect()
{
    return m_rect;
}

Item* ShopState::ItemMenu::GetSelection() const
{
    return m_options[m_selection];
}

int ShopState::ItemMenu::height_for_option ( CL_GraphicContext& gc )
{
    return m_height_per_option;
}

void ShopState::ItemMenu::process_choice ( int selection )
{
    m_selection = selection;
}

void ShopState::ItemMenu::SetFont ( Font font )
{
    m_option_font = font;
}

void ShopState::ItemMenu::SetHeightPerOption ( uint height )
{
    m_height_per_option = height;
}

void ShopState::ItemMenu::SetRect ( const CL_Rectf& rect )
{
    m_rect = rect;
}

void ShopState::ItemMenu::SetSelectedFont ( Font font )
{
    m_selected_font = font;
}

void ShopState::ItemMenu::SetUnavailableFont ( Font font )
{
    m_unavailable_font = font;
}


void ShopState::ItemMenu::SetPriceFont ( Font font )
{
    m_price_font = font;
}


void ShopState::Init ( const SteelArray& items )
{
   // Put items into array 
   m_item_menu.ClearOptions();
   for(SteelArray::const_iterator iter = items.begin(); iter != items.end(); iter++)
   {
        SteelType type = *iter;
        if(type.isHandle() && type.isValidHandle()){
            SteelType::Handle handle = type;
            Item * pItem = GrabHandle<Item*>(handle);
            //m_items.push_back(pItem);
            m_item_menu.AddOption(pItem);
        }
   }
}


bool ShopState::LastToDraw() const
{

}

void ShopState::MappableObjectMoveHook()
{

}

ShopState::ItemMenu::ItemMenu()
{
}

ShopState::ItemMenu::~ItemMenu()
{

}



ShopState::ShopState()
{

}

ShopState::~ShopState()
{

}

void ShopState::Start()
{    
    m_bDone = false;
    m_current_character = 0;
    m_character_rect = GraphicsManager::GetRect(GraphicsManager::SHOP,"character");
    m_items_rect = GraphicsManager::GetRect(GraphicsManager::SHOP,"items");
    m_stats_rect = GraphicsManager::GetRect(GraphicsManager::SHOP,"stats");
    m_desc_rect = GraphicsManager::GetRect(GraphicsManager::SHOP,"desc");
    m_gold_rect = GraphicsManager::GetRect(GraphicsManager::SHOP,"gold");
    m_item_font = GraphicsManager::GetFont(GraphicsManager::SHOP,"item");
    m_item_selected_font = GraphicsManager::GetFont(GraphicsManager::SHOP,"item_selected");
    m_price_font = GraphicsManager::GetFont(GraphicsManager::SHOP,"price");
    m_desc_font = GraphicsManager::GetFont(GraphicsManager::SHOP,"desc");
    
    m_stat_font = GraphicsManager::GetFont(GraphicsManager::SHOP,"stat");
    m_stat_up_font = GraphicsManager::GetFont(GraphicsManager::SHOP,"stat_up");
    m_stat_down_font = GraphicsManager::GetFont(GraphicsManager::SHOP,"stat_down");
    m_stat_name_font = GraphicsManager::GetFont(GraphicsManager::SHOP,"stat_name");    
    m_gold_font = GraphicsManager::GetFont(GraphicsManager::SHOP, "gold");
    m_item_unavailable_font = GraphicsManager::GetFont(GraphicsManager::SHOP,"item_unavailable");
    
    CL_Pointf item_point = GraphicsManager::GetPoint(GraphicsManager::SHOP,"item_box");
    
    CL_Rectf items_rect = m_items_rect;
    items_rect.shrink(GraphicsManager::GetMenuInset().x,GraphicsManager::GetMenuInset().y);
    m_item_menu.SetRect(items_rect);
    m_item_menu.SetFont(m_item_font);
    m_item_menu.SetSelectedFont(m_item_selected_font);
    m_item_menu.SetUnavailableFont(m_item_unavailable_font);
    m_item_menu.SetPriceFont(m_price_font);
    m_item_menu.SetHeightPerOption(item_point.y);
    
   
    CL_Rectf stats_rect = m_stats_rect;
    stats_rect.shrink ( GraphicsManager::GetMenuInset().x, GraphicsManager::GetMenuInset().y ) ;
    m_pStatusBox = new StatusBox(stats_rect,m_stat_font,m_stat_up_font,m_stat_down_font,m_stat_name_font);

}

void ShopState::SteelInit ( SteelInterpreter* )
{

}


void ShopState::SteelCleanup ( SteelInterpreter* )
{

}


bool ShopState::DisableMappableObjects() const
{
    return true;
}

void ShopState::Draw ( const CL_Rect& screenRect, CL_GraphicContext& GC )
{
    MenuBox::Draw(GC,m_character_rect,false);
    MenuBox::Draw(GC,m_items_rect,false);
    MenuBox::Draw(GC,m_stats_rect,false);
    MenuBox::Draw(GC,m_desc_rect,false);
    MenuBox::Draw(GC,m_gold_rect,false);    
    m_item_menu.Draw(GC);
    draw_characters(GC);
    draw_description(GC);
    draw_status(GC);
    draw_gold(GC);
}

void ShopState::draw_status ( CL_GraphicContext& gc )
{
    m_item_menu.Choose();
    Character * pChar = dynamic_cast<Character*>(IApplication::GetInstance()->GetParty()->GetCharacter(m_current_character));
    Item * pSelectedItem = m_item_menu.GetSelection();
    if(pSelectedItem->GetItemType() == Item::ARMOR || 
        pSelectedItem->GetItemType() == Item::WEAPON){
        Equipment * pEquip = dynamic_cast<Equipment*>(pSelectedItem);
    
        ParameterList params;
        params.push_back ( ParameterListItem("$_Character",pChar) );

    
        Equipment::eSlot slot;
        switch(pEquip->GetSlot()){
            case Equipment::EANYHAND:
                if(pChar->HasEquipment(Equipment::EHAND)){
                    slot = Equipment::EOFFHAND;
                }else{
                    slot = Equipment::EHAND;
                }
                break;
            case Equipment::EANYFINGER:
                if(pChar->HasEquipment(Equipment::EFINGER1)){
                    slot = Equipment::EFINGER2;
                }else{
                    slot =  Equipment::EFINGER1;
                }
                break;
            
            default:
                slot = pEquip->GetSlot();
                break;
            
        }
        if(pEquip->GetSlot() & slot && 
            pChar->GetClass()->CanEquip(pEquip)
            && pEquip->EquipCondition(params)){
            m_pStatusBox->Draw(gc, true, pChar,pChar->HasEquipment(slot)?pChar->GetEquipment(slot):NULL,pEquip);        
        }else{
            m_pStatusBox->Draw(gc, false, pChar, NULL,NULL );
        }
    }else{
        m_pStatusBox->Draw(gc, false, pChar,NULL,NULL);
    }
}


void ShopState::draw_characters ( CL_GraphicContext& gc )
{
    CL_Rectf rect = m_character_rect;
    rect.shrink(GraphicsManager::GetMenuInset().x, GraphicsManager::GetMenuInset().y);
    //rect.translate(GraphicsManager::GetMenuInset());
    //CL_Draw::fill(gc,rect,CL_Colorf(1.0f,1.0f,1.0f,0.5f));
    IParty * party = IApplication::GetInstance()->GetParty();
    for(uint i =0;i<party->GetCharacterCount(); i++)
    {
        CL_Sprite pSprite = dynamic_cast<Character*>(party->GetCharacter(i))
                        ->GetPortrait(Character::PORTRAIT_DEFAULT);        
        CL_Pointf point;
        point.y = rect.get_top_left().y +  (rect.get_height() - pSprite.get_height()) / 2.0f;
        float x_space =(rect.get_width() / party->GetCharacterCount());
        float center =  (x_space*i) + (x_space - pSprite.get_width()) / 2.0f; 
        point.x =  center;
        if(i != m_current_character)
            pSprite.set_alpha(0.5f);
        else
            pSprite.set_alpha(1.0f);
        pSprite.draw(gc,point.x,point.y);
    }
}

void ShopState::draw_description ( CL_GraphicContext& gc )
{
    CL_Rectf rect = m_desc_rect;
    rect.shrink(GraphicsManager::GetMenuInset().x,GraphicsManager::GetMenuInset().y);
    m_item_menu.Choose();
    Item * pItem = m_item_menu.GetSelection();
    draw_text(gc,m_desc_font,rect,pItem->GetDescription());
}

void ShopState::draw_gold ( CL_GraphicContext& gc )
{
    CL_Rectf rect = m_gold_rect;
    rect.shrink ( GraphicsManager::GetMenuInset().x, GraphicsManager::GetMenuInset().y );
    std::ostringstream stream;
    int gold = IApplication::GetInstance()->GetParty()->GetGold();
    stream << gold << ' '  
            << IApplication::GetInstance()->GetCurrencyName();
    
    draw_text(gc,m_gold_font, rect, stream.str() );
}


void ShopState::HandleAxisMove ( const StoneRing::IApplication::Axis& axis, const StoneRing::IApplication::AxisDirection dir, float pos )
{
    switch(dir){
        case IApplication::AXIS_DOWN:
            m_item_menu.SelectDown();
            break;
        case IApplication::AXIS_UP:
            m_item_menu.SelectUp();
            break;
        case IApplication::AXIS_LEFT:
            m_current_character++;
            if(m_current_character >= IApplication::GetInstance()->GetParty()->GetCharacterCount())
                m_current_character = 0;
            break;
        case IApplication::AXIS_RIGHT:
            if(m_current_character == 0){
                m_current_character = IApplication::GetInstance()->GetParty()->GetCharacterCount() - 1;
            } else {
                m_current_character--;
            }
            break;
    }
}

void ShopState::HandleButtonDown ( const StoneRing::IApplication::Button& button )
{
    StoneRing::State::HandleButtonDown ( button );
}

void ShopState::HandleButtonUp ( const StoneRing::IApplication::Button& button )
{
    StoneRing::State::HandleButtonUp ( button );
    switch(button){
        case IApplication::BUTTON_CANCEL:
            m_bDone = true;
            break;
        case IApplication::BUTTON_CONFIRM:{
            m_item_menu.Choose();
            Item * pItem = m_item_menu.GetSelection();
            int gold = IApplication::GetInstance()->GetParty()->GetGold();
            if(gold >= pItem->GetValue()){
                //TODO: Play ka-ching
                IApplication::GetInstance()->GetParty()->GiveGold( - pItem->GetValue() );
                IApplication::GetInstance()->GetParty()->GiveItem(pItem,1);
            }else{
                //TODO: play bbzzt
            }
            break;
    }
    }
}

bool ShopState::IsDone() const
{
    return m_bDone;
}


void ShopState::Finish()
{
    delete m_pStatusBox;
}

