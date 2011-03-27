#include "ItemSelectState.h"
#include "GraphicsManager.h"
#include "Item.h"
#include <iomanip>
using StoneRing::ItemSelectState;
using StoneRing::Item;
using StoneRing::Font;


class ItemCollector : public StoneRing::ItemVisitor
{
public:
    ItemCollector(ItemSelectState& state):m_state(state)
    {
    }
    ~ItemCollector(){}
    void operator()(Item* pItem, int nCount)
    {
	m_state.addItem(pItem,nCount);
    }
private:
    ItemSelectState &m_state;
};



bool ItemSelectState::IsDone() const
{
    return m_bDone;
}
	// Handle joystick / key events that are processed according to mappings
void ItemSelectState::HandleButtonUp(const IApplication::Button& button)
{
    switch(button)
    {
    case IApplication::BUTTON_CANCEL:
	m_bDone = true;
    break;
    	case IApplication::BUTTON_R:
	     switch(m_itemType)
	     {
		 case Item::REGULAR_ITEM:
		     m_itemType = Item::WEAPON;
		     break;
		 case Item::WEAPON:
		     m_itemType = Item::ARMOR;
		     break;
		 case Item::ARMOR:
		     m_itemType = Item::SPECIAL;
		     break;
		 case Item::SPECIAL:
		     m_itemType = Item::REGULAR_ITEM;
		     break;
	     }
	     reset_menu();
	    break;
	case IApplication::BUTTON_L:
	     switch(m_itemType)
	     {
		 case Item::REGULAR_ITEM:
		     m_itemType = Item::SPECIAL;
		     break;
		 case Item::WEAPON:
		     m_itemType = Item::REGULAR_ITEM;
		     break;
		 case Item::ARMOR:
		     m_itemType = Item::WEAPON;
		     break;
		 case Item::SPECIAL:
		     m_itemType = Item::ARMOR;
		     break;
	     }
	     reset_menu();
	    break;
 
    }
}

void ItemSelectState::HandleButtonDown(const IApplication::Button& button)
{
    switch(button)
    {
    }
}

void ItemSelectState::HandleAxisMove(const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos)
{
     if(axis == IApplication::AXIS_VERTICAL)
     {
	 if(dir == IApplication::AXIS_UP)
	 {
	        SelectUp();
	 }
	 else if(dir == IApplication::AXIS_DOWN)
	 {
		SelectDown();
	 }
     }
}

	
void ItemSelectState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC)
{
    m_overlay.draw(GC,0.0f,0.0f);
    Menu::Draw(GC);
    float header_width = m_header_rect.get_width();
    float each_width = header_width / m_type_icons.size();
    
    int i =0;
    for(std::map<Item::eItemType,CL_Image>::iterator iter = m_type_icons.begin();
	iter != m_type_icons.end(); iter++)
	{
	    float offset = (each_width - iter->second.get_width()) / 2.0f;
	    if(m_itemType == iter->first) 
	    {
		iter->second.set_alpha(1.0f);
	    }
	    else
	    {
		iter->second.set_alpha(0.5f);
	    }
	    iter->second.draw(GC,m_header_rect.left + i * each_width + offset,m_header_rect.top);
	    ++i;
	}
    
   
}

bool ItemSelectState::LastToDraw() const // Should we continue drawing more states?
{
}

bool ItemSelectState::DisableMappableObjects() const // Should the app move the MOs?
{
}

void ItemSelectState::MappableObjectMoveHook() // Do stuff right after the mappable object movement
{
}

void ItemSelectState::Start()
{
    m_items.clear();
    Menu::Init();
    m_bDone = false;
    std::string resource = "Overlays/ItemSelect/";
    IApplication *pApp = IApplication::GetInstance();    
    CL_ResourceManager& resources = pApp->GetResources();    
    m_header_rect.top = (float)resources.get_integer_resource(resource + "header/top",0);
    m_header_rect.left = (float)resources.get_integer_resource(resource + "header/left",0);
    m_header_rect.right = (float)resources.get_integer_resource(resource + "header/right",0);
    m_header_rect.bottom = (float)resources.get_integer_resource(resource + "header/bottom",0);
    
    m_rect.top = (float)resources.get_integer_resource(resource + "list/top",0);
    m_rect.left = (float)resources.get_integer_resource(resource + "list/left",0);
    m_rect.right = (float)resources.get_integer_resource(resource + "list/right",0);
    m_rect.bottom = (float)resources.get_integer_resource(resource + "list/bottom",0);   
    
    m_optionFont = GraphicsManager::GetFont(GraphicsManager::ITEMS,"Option");
    m_currentOptionFont = GraphicsManager::GetFont(GraphicsManager::ITEMS,"Selection");    
    
    m_overlay = GraphicsManager::GetOverlay(GraphicsManager::ITEMS);
    
    m_type_icons[ Item::REGULAR_ITEM ] = GraphicsManager::GetIcon("regular_items");
    m_type_icons[ Item::SPECIAL ] = GraphicsManager::GetIcon("special_items");    
    m_type_icons [ Item::WEAPON ] = GraphicsManager::GetIcon("weapons");
    m_type_icons [ Item::ARMOR ] = GraphicsManager::GetIcon("armor");
    
    m_itemType = Item::REGULAR_ITEM;
    
    IParty * party = IApplication::GetInstance()->GetParty();
    ItemCollector collector(*this);
    
    party->IterateItems(collector); 
    
}


void ItemSelectState::Finish() // Hook to clean up or whatever after being popped
{
}

CL_Rectf ItemSelectState::get_rect()
{
    return m_rect;
}

void ItemSelectState::draw_option(int option, bool selected, float x, float y, CL_GraphicContext& gc)
{
    const std::pair<Item*,int>& pair  = m_items[ m_itemType ][option];

    Item * pItem = pair.first;
    const int count = pair.second;
    
    CL_Image icon = pItem->GetIcon();
    //icon.set_alignment(origin_top_left);
    
    icon.draw(gc,x,y);
    const float icon_width = icon.get_width();
    std::ostringstream text;
    if(count > 1)
	text << std::setw(40) << std::left << pItem->GetName() << ' ' << std::setw(3) << std::right << count;
    else
	text << std::setw(40) << std::left << pItem->GetName();
    
    if(selected)
	m_currentOptionFont.draw_text(gc,x + icon_width + 12,y,text.str(), Font::TOP_LEFT);
    else
	m_optionFont.draw_text(gc,x + icon_width,y + 12,text.str(), Font::TOP_LEFT);
    
}

int ItemSelectState::height_for_option(CL_GraphicContext& gc)
{
     return cl_max(m_optionFont.get_font_metrics(gc).get_height(),m_currentOptionFont.get_font_metrics(gc).get_height());
}

void ItemSelectState::process_choice(int selection)
{
}

int ItemSelectState::get_option_count()
{
    return m_items [ m_itemType ].size();
}


void ItemSelectState::draw_categories()
{

}


void ItemSelectState::addItem(Item* pItem,int count)
{
   m_items [ pItem->GetItemType() ].push_back( std::pair<Item*,int>(pItem,count) ); 
}
