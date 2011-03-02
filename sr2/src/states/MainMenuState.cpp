#include "MainMenuState.h"
#include "IApplication.h"
#include "GraphicsManager.h"
#include "Level.h"

using std::min;
using std::max;

StoneRing::MainMenuState::MainMenuState():m_bDone(false)
{

}

StoneRing::MainMenuState::~MainMenuState()
{
}

bool StoneRing::MainMenuState::IsDone() const
{
    return m_bDone;
}

 void StoneRing::MainMenuState::HandleButtonUp(const IApplication::Button& button)
 {
    switch(button)
    {
	case IApplication::BUTTON_CONFIRM:
	    // Select current option.

	    break;
	case IApplication::BUTTON_CANCEL:
	    m_bDone = true;
	    break;
    }
 }
 void StoneRing::MainMenuState::HandleButtonDown(const IApplication::Button& button)
 {
 }
 
 void StoneRing::MainMenuState::HandleAxisMove(const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos)
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

void StoneRing::MainMenuState::HandleKeyDown(const CL_InputEvent &key)
{
}

void StoneRing::MainMenuState::HandleKeyUp(const CL_InputEvent &key)
{
    switch(key.id)
    {
    case CL_KEY_ENTER:
    case CL_KEY_SPACE:

        break;
    case CL_KEY_DOWN:

        break;
    case CL_KEY_UP:

        break;

    case CL_KEY_ESCAPE:
        break;
    }
}

void StoneRing::MainMenuState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC)
{
    m_overlay.draw(GC,0,0);
    Menu::Draw(GC);
}



bool StoneRing::MainMenuState::DisableMappableObjects() const
{
    return true;
}


void StoneRing::MainMenuState::MappableObjectMoveHook()
{
}

void StoneRing::MainMenuState::Start()
{

    CL_GraphicContext& GC = GET_MAIN_GC();
    m_bDone = false;
    std::string resource = "Overlays/MainMenu/";
    IApplication *pApp = IApplication::GetInstance();
    CL_ResourceManager& resources = pApp->GetResources();



    m_optionFont = GraphicsManager::GetFont(GraphicsManager::MAIN_MENU,"Option");
    m_selectionFont = GraphicsManager::GetFont(GraphicsManager::MAIN_MENU,"Selection");
    m_optionColor = GraphicsManager::GetFontColor(GraphicsManager::MAIN_MENU,"Option");
    m_selectionColor = GraphicsManager::GetFontColor(GraphicsManager::MAIN_MENU,"Selection");

    m_overlay = GraphicsManager::GetOverlay(GraphicsManager::MAIN_MENU);

    m_menu_rect.top = (float)resources.get_integer_resource(resource + "menu/top",0);
    m_menu_rect.left = (float)resources.get_integer_resource(resource + "menu/left",0);
    m_menu_rect.right = (float)resources.get_integer_resource(resource + "menu/right",0);
    m_menu_rect.bottom = (float)resources.get_integer_resource(resource + "menu/bottom",0);

    m_party_rect.top = (float)resources.get_integer_resource(resource + "party/top",0);
    m_party_rect.left = (float)resources.get_integer_resource(resource + "party/left",0);
    m_party_rect.right = (float)resources.get_integer_resource(resource + "party/right",0);
    m_party_rect.bottom = (float)resources.get_integer_resource(resource + "party/bottom",0);
    
    m_character_rect.top = (float)resources.get_integer_resource(resource + "character/top",0);
    m_character_rect.left = (float)resources.get_integer_resource(resource + "character/left",0);
    m_character_rect.right = (float)resources.get_integer_resource(resource + "character/right",0);
    m_character_rect.bottom = (float)resources.get_integer_resource(resource + "character/bottom",0);


}

void StoneRing::MainMenuState::Finish()
{
}


void StoneRing::MainMenuState::Init()
{
    Menu::Init();
}

CL_Rectf StoneRing::MainMenuState::get_rect()
{
    return m_menu_rect;
}

void StoneRing::MainMenuState::draw_option(int option, bool selected, float x, float y, CL_GraphicContext& gc)
{
        CL_Font  lineFont;
	CL_Colorf drawColor = m_optionColor;
	
	lineFont = m_optionFont;
	
	if(selected){
	    lineFont = m_selectionFont;  
	    drawColor = m_selectionColor;
	}
	
	float font_height_offset = 0 - ((lineFont.get_font_metrics(gc).get_height() - 
				    lineFont.get_font_metrics(gc).get_descent() -  
				    lineFont.get_font_metrics(gc).get_internal_leading())/ 2);
	
	m_choices[option]->GetIcon().draw(gc,x,y);
        lineFont.draw_text(gc, x + m_choices[option]->GetIcon().get_width() + 10,  y + lineFont.get_font_metrics(gc).get_height() + font_height_offset,
                         m_choices[option]->GetName(),drawColor);
}

int StoneRing::MainMenuState::height_for_option(CL_GraphicContext& gc)
{
    return cl_max(m_optionFont.get_font_metrics(gc).get_height(),m_selectionFont.get_font_metrics(gc).get_height());
}

void StoneRing::MainMenuState::process_choice(int selection)
{

}

int StoneRing::MainMenuState::get_option_count()
{
    return m_choices.size();
}

void StoneRing::MainMenuState::AddOption(MenuOption* pOption)
{
    m_choices.push_back(pOption);
}


