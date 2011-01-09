#include <list>
#include "BattleMenu.h"
#include "BattleMenuOption.h"
#include "GraphicsManager.h"

using StoneRing::BattleMenu;
using StoneRing::BattleMenuOption;

BattleMenu::BattleMenu()
{
    m_font_height = -1;
}

BattleMenu::~BattleMenu()
{
}


BattleMenu::eType BattleMenu::GetType ( void ) const
{
    return m_eType;
}

void BattleMenu::Init() 
{
    Menu::Init();
    if(m_onFont.is_null()){
	m_onFont = GraphicsManager::GetFont(GraphicsManager::GetFontName(GraphicsManager::BATTLE_POPUP_MENU,"on"));
	m_offFont = GraphicsManager::GetFont(GraphicsManager::GetFontName(GraphicsManager::BATTLE_POPUP_MENU,"off"));
	m_selectedFont = GraphicsManager::GetFont(GraphicsManager::GetFontName(GraphicsManager::BATTLE_POPUP_MENU,"Selection"));
	m_onColor = GraphicsManager::GetFontColor(GraphicsManager::GetFontName(GraphicsManager::BATTLE_POPUP_MENU,"on"));
	m_offColor = GraphicsManager::GetFontColor(GraphicsManager::GetFontName(GraphicsManager::BATTLE_POPUP_MENU,"off"));
	m_selectedColor = GraphicsManager::GetFontColor(GraphicsManager::GetFontName(GraphicsManager::BATTLE_POPUP_MENU,"Selection"));
    }
}

void BattleMenu::SetEnableConditionParams(const ParameterList& params)
{
    m_params = params;
}

std::vector<BattleMenuOption*>::iterator
BattleMenu::GetOptionsBegin()
{
    return m_options.begin();
}


std::vector<BattleMenuOption*>::iterator
BattleMenu::GetOptionsEnd()
{
    return m_options.end();
}


bool BattleMenu::handle_element(Element::eElement element, Element *pElement)
{
    if(element == EBATTLEMENUOPTION)
        m_options.push_back ( dynamic_cast<BattleMenuOption*>(pElement) );
    else return false;

    return true;
}

void BattleMenu::load_finished()
{

}



CL_Rectf BattleMenu::get_rect()
{
    return m_rect;
}

BattleMenuOption* BattleMenu::GetSelectedOption() const
{
    return m_options[get_current_choice()];
}

void BattleMenu::draw_option(int option, bool selected, float x, float y, CL_GraphicContext& gc)
{
    BattleMenuOption * pOption = m_options[option];
    
    CL_Font font;
    CL_Image icon = pOption->GetIcon();
    //icon.set_alignment(origin_bottom_left,0,0);
    

    if(pOption->Enabled(m_params))
    {
	font = m_onFont;
    }
    else
    {
	font = m_offFont;
    }
    
    if(selected)
    {
	font = m_selectedFont;
    }
    
    font.draw_text(gc,x  + 12.0f + icon.get_width() , font.get_font_metrics(gc).get_height() + y,pOption->GetName());

    icon.draw(gc,static_cast<int>(x ), static_cast<int>(y));
    
}

int BattleMenu::height_for_option(CL_GraphicContext& gc)
{
    if(m_font_height != -1) return m_font_height; 
    
    m_font_height = m_onFont.get_font_metrics(gc).get_height();
    m_font_height = cl_max(m_offFont.get_font_metrics(gc).get_height(),static_cast<float>(m_font_height));
    m_font_height = cl_max(m_selectedFont.get_font_metrics(gc).get_height(),static_cast<float>(m_font_height));
    
    return m_font_height;
}

int BattleMenu::get_option_count()
{
    return m_options.size();
}

void BattleMenu::SetRect(CL_Rectf& rect)
{
    m_rect = rect;
}

void BattleMenu::load_attributes(CL_DomNamedNodeMap attr)
{
    std::string type = get_required_string("type",attr);

    if(type == "popup")
        m_eType = POPUP;
    else if(type == "skills")
        m_eType = SKILLS;
    else if(type == "spells")
        m_eType = SPELLS;
    else if(type == "items")
        m_eType = ITEMS;
    else if(type == "custom")
        m_eType = CUSTOM;
    else throw CL_Exception("Bad BattleMenu type of " + type);
}

