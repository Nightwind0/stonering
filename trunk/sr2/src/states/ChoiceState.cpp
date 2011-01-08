#include "ChoiceState.h"
#include "IApplication.h"
#include "GraphicsManager.h"
#include "Level.h"

using std::min;
using std::max;

StoneRing::ChoiceState::ChoiceState():m_bDone(false),m_nSelection(-1)
{

}

StoneRing::ChoiceState::~ChoiceState()
{
}

bool StoneRing::ChoiceState::IsDone() const
{
    return m_bDone;
}

 void StoneRing::ChoiceState::HandleButtonUp(const IApplication::Button& button)
 {
    switch(button)
    {
	case IApplication::BUTTON_CONFIRM:
	    // Select current option.
	    m_bDraw = false;
	    // mpChoice->chooseOption(mnCurrentOption);
	    m_nSelection = get_current_choice();
	    m_bDone = true;
	    break;
	case IApplication::BUTTON_CANCEL:
	    break;
    }
 }
 void StoneRing::ChoiceState::HandleButtonDown(const IApplication::Button& button)
 {
 }
 
 void StoneRing::ChoiceState::HandleAxisMove(const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos)
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

void StoneRing::ChoiceState::HandleKeyDown(const CL_InputEvent &key)
{
}

void StoneRing::ChoiceState::HandleKeyUp(const CL_InputEvent &key)
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

void StoneRing::ChoiceState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC)
{
    if(!m_bDraw) return;

    CL_Draw::fill(GC, m_question_rect, m_question_BGColor );
    CL_Draw::fill(GC, m_text_rect, m_text_BGColor ) ;

    m_choiceOverlay.draw(GC,static_cast<float>(m_X),static_cast<float>(m_Y));
    m_choiceFont.draw_text(GC,m_question_rect.left,m_question_rect.top + m_choiceFont.get_font_metrics(GC).get_height(),m_text);

    Menu::Draw(GC);
}



bool StoneRing::ChoiceState::DisableMappableObjects() const
{
    return true;
}


void StoneRing::ChoiceState::MappableObjectMoveHook()
{
}

void StoneRing::ChoiceState::Start()
{

    CL_GraphicContext& GC = GET_MAIN_GC();
    m_bDone = false;
    m_bDraw = true;
    std::string resource = "Overlays/Choice/";
    IApplication *pApp = IApplication::GetInstance();
    CL_ResourceManager& resources = pApp->GetResources();
    std::string choiceFont = CL_String_load(resource + "fonts/Choice",resources);
    std::string optionFont = CL_String_load(resource + "fonts/Option",resources);
    std::string selectionFont = CL_String_load(resource+ "fonts/Selection",resources);

    m_choiceFont = GraphicsManager::GetInstance()->GetFont(choiceFont);

    m_optionFont = GraphicsManager::GetInstance()->GetFont(optionFont);

    m_currentOptionFont = GraphicsManager::GetInstance()->GetFont(selectionFont);

    m_choiceOverlay = CL_Image(GC,"Overlays/Choice/overlay", &resources );

    m_question_rect.top = (float)resources.get_integer_resource(resource + "header/top",0);
    m_question_rect.left = (float)resources.get_integer_resource(resource + "header/left",0);
    m_question_rect.right = (float)resources.get_integer_resource(resource + "header/right",0);
    m_question_rect.bottom = (float)resources.get_integer_resource(resource + "header/bottom",0);

    m_text_rect.top = (float)resources.get_integer_resource(resource + "text/top",0);
    m_text_rect.left = (float)resources.get_integer_resource(resource + "text/left",0);
    m_text_rect.right = (float)resources.get_integer_resource(resource + "text/right",0);
    m_text_rect.bottom = (float)resources.get_integer_resource(resource + "text/bottom",0);

    m_question_BGColor.set_red (resources.get_integer_resource(resource + "header/bgcolor/r",0));
    m_question_BGColor.set_green (resources.get_integer_resource(resource + "header/bgcolor/g",0));
    m_question_BGColor.set_blue (resources.get_integer_resource(resource + "header/bgcolor/b",0));
    m_question_BGColor.set_alpha (resources.get_integer_resource(resource + "header/bgcolor/a",0));

    m_text_BGColor.set_red ((float)resources.get_integer_resource(resource + "text/bgcolor/r",0) / 255.0f);
    m_text_BGColor.set_green ((float)resources.get_integer_resource(resource + "text/bgcolor/g",0) / 255.0f);
    m_text_BGColor.set_blue ((float)resources.get_integer_resource(resource + "text/bgcolor/b",0) / 255.0f);
    m_text_BGColor.set_alpha ((float)resources.get_integer_resource(resource + "text/bgcolor/a",0) / 255.0f);

    m_X = resources.get_integer_resource(resource + "x",0);
    m_Y = resources.get_integer_resource(resource + "y",0);

/*
  if(!mpChoiceOverlay)
  mpChoiceOverlay = new CL_Surface("Overlays/choice_overlay", IApplication::getInstance()->getResources() );
*/
}

void StoneRing::ChoiceState::Finish()
{
}


void StoneRing::ChoiceState::Init(const std::string &choiceText, const std::vector<std::string> &choices)
{
    m_text = choiceText;
    m_choices = choices;
    Menu::Init();
}

CL_Rectf StoneRing::ChoiceState::get_rect()
{
    return m_text_rect;
}

void StoneRing::ChoiceState::draw_option(int option, bool selected, float x, float y, CL_GraphicContext& gc)
{
        CL_Font  lineFont;
	
	lineFont = m_optionFont;
	
	if(selected)
	    lineFont = m_currentOptionFont;  

        lineFont.draw_text(gc, x,  y + lineFont.get_font_metrics(gc).get_height(),
                         m_choices[option]);
}

int StoneRing::ChoiceState::height_for_option(CL_GraphicContext& gc)
{
    return cl_max(m_optionFont.get_font_metrics(gc).get_height(),m_currentOptionFont.get_font_metrics(gc).get_height());
}

void StoneRing::ChoiceState::process_choice(int selection)
{

}

int StoneRing::ChoiceState::get_option_count()
{
    return m_choices.size();
}



