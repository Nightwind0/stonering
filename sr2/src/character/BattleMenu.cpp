#include <list>
#include "BattleMenu.h"
#include "BattleMenuOption.h"
#include "GraphicsManager.h"
#include "Character.h"

using StoneRing::BattleMenu;
using StoneRing::BattleMenuOption;
using StoneRing::Character;

BattleMenu::BattleMenu()
{
    m_font_height = -1;
    m_pCharacter = NULL;
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
    if(m_onFont.is_null()){
		m_onFont = GraphicsManager::GetFont(GraphicsManager::BATTLE_POPUP_MENU,"on");
		m_offFont = GraphicsManager::GetFont(GraphicsManager::BATTLE_POPUP_MENU,"off");
		m_selectedFont = GraphicsManager::GetFont(GraphicsManager::BATTLE_POPUP_MENU,"Selection");
        m_mpFont = GraphicsManager::GetFont(GraphicsManager::BATTLE_POPUP_MENU,"mp");
        m_bpFont = GraphicsManager::GetFont(GraphicsManager::BATTLE_POPUP_MENU,"bp");
        m_cost_spacing = GraphicsManager::GetPoint(GraphicsManager::BATTLE_POPUP_MENU,"cost_spacing");
    }
    Menu::Init();    
}


void BattleMenu::SetEnableConditionParams(const ParameterList& params, Character * pChar)
{
    m_params = params;
    m_pCharacter = pChar;
}

bool BattleMenu::HasEnabledOptions() const {
	assert( m_pCharacter );
	for(std::vector<BattleMenuOption*>::const_iterator it = m_options.begin();
		it != m_options.end(); it++){
		if((*it)->Enabled(m_params,m_pCharacter) && (*it)->Visible(m_pCharacter))
			return true;
	}
	
	return false;
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



clan::Rectf BattleMenu::get_rect()
{
    return m_rect;
}

BattleMenuOption* BattleMenu::GetSelectedOption() const
{
    if(m_options.size() == 0) return NULL;
    else return m_options[get_current_choice()];
}

void BattleMenu::draw_option(int option, bool selected, const clan::Rectf& rect, clan::Canvas& gc)
{
    BattleMenuOption * pOption = m_options[option];
    
    const float x = rect.get_top_left().x;
    const float y = rect.get_top_left().y;
    
    Font font;
    clan::Image icon = pOption->GetIcon();

    //icon.set_alignment(origin_bottom_left,0,0);
    

    if(pOption->Enabled(m_params, m_pCharacter))
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
    
    
    bool cost = false;
    const float spacing = 12.0f;
    if(pOption->GetBPCost() != 0 ||
        pOption->GetMPCost() != 0)
        cost = true;
    clan::Rectf text_rect(x + spacing + icon.get_width() ,  y ,
                       clan::Sizef(m_rect.get_width() - spacing - icon.get_width() - (cost?m_cost_spacing.x:0), height_for_option(gc)));
   // font.draw_text(gc,x  + 12.0f + icon.get_width() , font.get_font_metrics(gc).get_height() + y + font_height_offset,pOption->GetName());
    draw_text(gc,font,text_rect,pOption->GetName(),0);
    icon.draw(gc,static_cast<int>(x ), static_cast<int>(y));
    
    if(cost)
    {
        Font font;
        int cost = 0;
        std::string type= "";
        if(pOption->GetBPCost()){
            font = m_bpFont;
            cost = pOption->GetBPCost() * m_pCharacter->GetAttribute(ICharacter::CA_BP_COST);
            type = "BP";
        }else{
            font = m_mpFont;
            cost = pOption->GetMPCost() * m_pCharacter->GetAttribute(ICharacter::CA_MP_COST);
            type = "MP";
        }
        float font_height = font.get_font_metrics(gc).get_height();
        float height = height_for_option(gc);

        
        clan::Rectf cost_rect(x + m_rect.get_width()- m_cost_spacing.x, y  + (height - font_height)/2.0f,
                           clan::Sizef(m_cost_spacing.x,height));
        std::ostringstream stream;
        stream << cost  << type;
        draw_text(gc,font,cost_rect,stream.str(),0);
    }
}

int BattleMenu::height_for_option(clan::Canvas& gc)
{
    if(m_font_height != -1) return m_font_height; 
    
    m_font_height = m_onFont.get_font_metrics(gc).get_height();
    m_font_height = std::max(m_offFont.get_font_metrics(gc).get_height(),static_cast<float>(m_font_height));
    m_font_height = std::max(m_selectedFont.get_font_metrics(gc).get_height(),static_cast<float>(m_font_height));
    
    return m_font_height;
}

int BattleMenu::get_option_count()
{
    return m_options.size();
}

bool BattleMenu::hide_option ( int selection ) const
{
    return !m_options[selection]->Visible(m_pCharacter);        
}


void BattleMenu::SetRect(clan::Rectf& rect)
{
    m_rect = rect;
}

void BattleMenu::load_attributes(clan::DomNamedNodeMap attr)
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
    else throw XMLException("Bad BattleMenu type of " + type);
}

