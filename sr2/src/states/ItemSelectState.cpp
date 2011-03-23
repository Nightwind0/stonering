#include "ItemSelectState.h"
#include "GraphicsManager.h"

using StoneRing::ItemSelectState;


bool ItemSelectState::IsDone() const
{
    return m_bDone;
}
	// Handle joystick / key events that are processed according to mappings
void ItemSelectState::HandleButtonUp(const IApplication::Button& button)
{
    m_bDone = true;
}

void ItemSelectState::HandleButtonDown(const IApplication::Button& button)
{
}

void ItemSelectState::HandleAxisMove(const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos)
{
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
    
    m_optionFont = GraphicsManager::GetFont(GraphicsManager::CHOICE,"Option");
    m_currentOptionFont = GraphicsManager::GetFont(GraphicsManager::CHOICE,"Selection");    
    
    m_overlay = GraphicsManager::GetOverlay(GraphicsManager::ITEMS);
    
    m_type_icons[ Item::REGULAR_ITEM ] = GraphicsManager::GetIcon("regular_items");
    m_type_icons[ Item::SPECIAL ] = GraphicsManager::GetIcon("special_items");    
    m_type_icons [ Item::WEAPON ] = GraphicsManager::GetIcon("weapons");
    m_type_icons [ Item::ARMOR ] = GraphicsManager::GetIcon("armor");
    
    m_itemType = Item::REGULAR_ITEM;
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
}


void ItemSelectState::draw_categories()
{

}
