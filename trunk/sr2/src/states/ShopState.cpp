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
#include "SoundManager.h"
#include "Party.h"
#include <iomanip>


using namespace StoneRing;
using namespace Steel;

namespace StoneRing{ 
    class ItemCollector : public StoneRing::ItemVisitor
    {
    public:
        ItemCollector(ShopState::ItemMenu& items):m_items(items)
        {
        }
        ~ItemCollector(){}
        void operator()(Item* pItem, int nCount)
        {
            // Can only sell certain types of items
            if(pItem->GetItemType() == Item::REGULAR_ITEM ||
                pItem->GetItemType() == Item::WEAPON ||
                pItem->GetItemType() == Item::ARMOR)
                m_items.AddOption(pItem,nCount);
        }
    private:
        struct ItemEntry {
            Item* m_pItem;
            int m_count;
        };
        ShopState::ItemMenu& m_items;
    };

}


void ShopState::ItemMenu::AddOption ( Item* pOption, int nCount )
{
    ItemEntry entry;
    entry.m_pItem = pOption;
    entry.m_count = nCount;
    m_options.push_back ( entry );
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
    Item * pItem = m_options[option].m_pItem;
    Font font = m_option_font;
    if(!m_bSell && IApplication::GetInstance()->GetParty()->GetGold() < pItem->GetValue()){
        font = m_unavailable_font;
    }
    
    if(selected)
        font = m_selected_font;
    
    CL_FontMetrics metrics = font.get_font_metrics(gc);
    CL_Image icon;
    icon = pItem->GetIcon();
    icon.draw(gc,x,y);
    std::ostringstream name_stream;
    name_stream << pItem->GetName();
    if(m_options[option].m_count > 1)
        name_stream << ' ' << 'x' << m_options[option].m_count;
    font.draw_text(gc,x + icon.get_width() + 2,y + (height_for_option(gc) - metrics.get_height())/2,name_stream.str(), Font::TOP_LEFT);
    std::ostringstream stream;
    int value = m_bSell?pItem->GetSellValue():pItem->GetValue();
    stream << std::setw(7) << value;
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
    return m_options[m_selection].m_pItem;
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

void ShopState::ItemMenu::SetSellMode ( bool Sell )
{
    m_bSell = Sell;
}


void ShopState::Init ( const SteelArray& items )
{
	m_item_menu.Init();
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
   
	m_bSell = false;
}

void ShopState::Init()
{
    m_bSell = true;
	m_item_menu.Init();
    m_item_menu.ClearOptions();
    ItemCollector collector(m_item_menu);
    IApplication::GetInstance()->GetParty()->IterateItems(collector);
    
}



bool ShopState::LastToDraw() const
{
	return false;
}

void ShopState::MappableObjectMoveHook()
{

}

ShopState::ItemMenu::ItemMenu():m_bSell(false)
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
    m_stat_header_rect = GraphicsManager::GetRect(GraphicsManager::SHOP,"stat_header");
    m_stat_box = GraphicsManager::GetRect(GraphicsManager::SHOP,"stat_box");
    m_item_font = GraphicsManager::GetFont(GraphicsManager::SHOP,"item");
    m_item_selected_font = GraphicsManager::GetFont(GraphicsManager::SHOP,"item_selected");
    m_price_font = GraphicsManager::GetFont(GraphicsManager::SHOP,"price");
    m_desc_font = GraphicsManager::GetFont(GraphicsManager::SHOP,"desc");
    m_stat_header_font = GraphicsManager::GetFont(GraphicsManager::SHOP,"stat_header");
    
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
    
    m_pStatusBox = new StatusBox(m_stats_rect,m_stat_header_rect, m_stat_header_font,  m_stat_font,m_stat_up_font,m_stat_down_font,m_stat_name_font);
    
    m_item_menu.SetSellMode(m_bSell);

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
    MenuBox::Draw(GC,m_stat_box,false);
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
    Party * party = IApplication::GetInstance()->GetParty();
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
    int gold = IApplication::GetInstance()->GetParty()->GetLerpGold();
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
            SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);
            m_current_character++;
            if(m_current_character >= IApplication::GetInstance()->GetParty()->GetCharacterCount())
                m_current_character = 0;
            break;
        case IApplication::AXIS_RIGHT:
            SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);
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
        case IApplication::BUTTON_SELECT:
            SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);
            m_pStatusBox->switchPage();
            break;
        case IApplication::BUTTON_CANCEL:
            SoundManager::PlayEffect(SoundManager::EFFECT_CANCEL);
            m_bDone = true;
            break;
        case IApplication::BUTTON_CONFIRM:{
            m_item_menu.Choose();
            Item * pItem = m_item_menu.GetSelection();
            int gold = IApplication::GetInstance()->GetParty()->GetGold();
            if(!m_bSell){
                if(gold >= pItem->GetValue()){          
					Party * party = IApplication::GetInstance()->GetParty();		
					Character * pChar = dynamic_cast<Character*>(party->GetCharacter(m_current_character));
                    SoundManager::PlayEffect(SoundManager::EFFECT_REWARD);
                    IApplication::GetInstance()->GetParty()->GiveGold( - pItem->GetValue() );
				if(pItem->GetItemType() == Item::WEAPON || pItem->GetItemType() == Item::ARMOR){
						Equipment * pEquipment = dynamic_cast<Equipment*>(pItem);
						if(pChar->CanEquip(pEquipment)){
							std::vector<std::string> options;
							options.push_back("Not Now");
							options.push_back("Equip Now");
							if(1 == IApplication::GetInstance()->DynamicMenu(options)){
								// First, try to find an empty slot that is compatible with pItem
								bool equipped = false;
								for(int i=0;i<7;i++){
									// TODO: Make a method on Equipment that gives me an iterator to the slots or something
									Equipment::eSlot slot = (Equipment::eSlot)std::pow(2,i);
									if(pEquipment->GetSlot() & slot  && !pChar->HasEquipment(slot)){
										pChar->Equip(slot,pEquipment);
										equipped = true;
										break;
									}
								}
								if(!equipped){
									for(int i=0;i<7;i++){
										// TODO: Make a method on Equipment that gives me an iterator to the slots or something
										Equipment::eSlot slot = (Equipment::eSlot)std::pow(2,i);
										if(pEquipment->GetSlot() & slot){
											if(pChar->HasEquipment(slot)){
												party->GiveItem(pChar->GetEquipment(slot),1);
											}
											pChar->Equip(slot,pEquipment);
											break;
										}
									}				
							   }
							}else{
								party->GiveItem(pItem,1);							
							}
						}else{
							party->GiveItem(pItem,1);						
						}	
					}
                }else{
                    //play bbzzt
                    SoundManager::PlayEffect(SoundManager::EFFECT_BAD_OPTION);
                }
            }else{
                SoundManager::PlayEffect(SoundManager::EFFECT_GOLD);
                IApplication::GetInstance()->GetParty()->TakeItem(pItem,1);
                IApplication::GetInstance()->GetParty()->GiveGold( pItem->GetSellValue() );
                m_item_menu.ClearOptions();
                ItemCollector collector(m_item_menu);
                IApplication::GetInstance()->GetParty()->IterateItems(collector);
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

