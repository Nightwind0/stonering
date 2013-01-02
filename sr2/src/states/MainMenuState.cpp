#include "MainMenuState.h"
#include "IApplication.h"
#include "GraphicsManager.h"
#include "Level.h"
#include "SoundManager.h"
#include "MenuBox.h"
#include <iomanip>
#include "Party.h"
#include "Omega.h"

using std::min;
using std::max;

using namespace Steel;

namespace StoneRing { 

MainMenuState::MainMenuState():m_targetingState(*this),m_bDone(false)
{

}

MainMenuState::~MainMenuState()
{
}

bool MainMenuState::IsDone() const
{
    return m_bDone;
}

 void MainMenuState::HandleButtonUp(const IApplication::Button& button)
 {
    switch(button)
    {
	case IApplication::BUTTON_CONFIRM:
	    // Select current option.
	    SoundManager::PlayEffect(SoundManager::EFFECT_SELECT_OPTION);
	    Menu::Choose();
	    break;
	case IApplication::BUTTON_CANCEL:
            SoundManager::PlayEffect(SoundManager::EFFECT_CANCEL);
            if(m_option_parent){
                Menu::PopMenu();
                m_option_parent = m_option_parent->GetParent();
                if(m_option_parent)
                    fill_choices(m_option_parent->GetChildrenBegin(),
                                 m_option_parent->GetChildrenEnd());
                else
                    fill_choices(m_root_choices.begin(),m_root_choices.end());
            }else{
                m_bDone = true;
            }
	    break;
    }
 }
 void MainMenuState::HandleButtonDown(const IApplication::Button& button)
 {
 }
 
 void MainMenuState::HandleAxisMove(const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos)
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

void MainMenuState::HandleKeyDown(const CL_InputEvent &key)
{
}

void MainMenuState::HandleKeyUp(const CL_InputEvent &key)
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

void MainMenuState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC)
{
    MenuBox::Draw(GC,screenRect);
    MenuBox::Draw(GC,m_menu_rect,false);
    MenuBox::Draw(GC,m_status_rect,false);
    Menu::Draw(GC);
    
    draw_party(GC);
    
    if(m_bSelectingTarget){
	Party * party = IApplication::GetInstance()->GetParty();
	const int player_count = party->GetCharacterCount();
	if(m_bSelectAll)
	{
	    for(int i=0;i<player_count;i++)
	    {
		CL_Pointf point = calc_player_position(i);
		m_target_sprite.draw(GC,point.x,point.y);
	    }
	}
	else
	{
	    CL_Pointf point = calc_player_position(m_nSelectedChar);
	    m_target_sprite.draw(GC,point.x,point.y);
	}
    }
    draw_party_stats(GC);
}

CL_Pointf MainMenuState::calc_player_position(int player)const
{
    float height = m_character_rect.get_height() / 4;

    CL_Pointf portraitPoint;
    portraitPoint.x = m_character_rect.left; // TODO Row?
    portraitPoint.y = m_character_rect.top + height * player;
    
    return portraitPoint;
}

void MainMenuState::draw_party_stats ( CL_GraphicContext& gc )
{
    const uint kTextWidth = 30;
    std::ostringstream os;
    os << "Time:";
    uint minutes = IApplication::GetInstance()->GetParty()->GetMinutesPlayed();
    if(minutes > 60){
        os << std::setw(kTextWidth) << minutes/60 << ':' << minutes%60 << 'm';
    }else{
        os << std::setw(kTextWidth) << minutes << 'm';
    }
    
    std::string min_str = os.str();
    
    os.clear();
    os.str("");
    os << "Gold:" << std::setw(kTextWidth) << IApplication::GetInstance()->GetParty()->GetGold();

 
    std::string gold_str = os.str();
       
    m_partyStatFont.draw_text(gc,m_status_rect.get_top_left().x+20,m_status_rect.get_top_left().y+20,min_str,Font::TOP_LEFT);
    m_partyStatFont.draw_text(gc,m_status_rect.get_top_left().x+20,m_status_rect.get_top_left().y+40,gold_str,Font::TOP_LEFT); 
    draw_omegas(gc);
}

void MainMenuState::draw_omegas ( CL_GraphicContext& gc )
{
    Party * pParty = IApplication::GetInstance()->GetParty();
    CL_Pointf top_left = m_omega_rect.get_top_left();
    
    const uint slots_per_row = m_omega_rect.get_width() / 40;
    for(uint i=0;i<pParty->GetCommonAttribute(ICharacter::CA_IDOL_SLOTS);i++){
        Omega * pOmega = pParty->GetOmega(i);
        CL_Pointf point( i % slots_per_row + top_left.x, top_left.y + i /  slots_per_row );
        if(pOmega){            
            pOmega->GetIcon().draw(gc,point.x,point.y);
        }
    }
    
}



void MainMenuState::draw_party(CL_GraphicContext& GC)
{
    Party * party = IApplication::GetInstance()->GetParty();
    float height = m_character_rect.get_height() / 4;
    const float spacing = 12.0f;
    for(int i=0;i<party->GetCharacterCount();i++){
	CL_Pointf portraitPoint = calc_player_position(i);

	CL_Pointf shadowPoint = portraitPoint;
	shadowPoint += CL_Pointf(-8.0f,-8.0f);
	m_portrait_shadow.draw(GC,shadowPoint.x,shadowPoint.y);
	Character * pCharacter = dynamic_cast<Character*>(party->GetCharacter(i));
	// TODO: Which portrait should change, and depend on status effects
	CL_Sprite portrait = pCharacter->GetPortrait(Character::PORTRAIT_DEFAULT);
        portrait.set_alpha(1.0f);
	portrait.draw(GC,portraitPoint.x,portraitPoint.y);
	std::ostringstream levelstream;
	levelstream <<  "Level " << pCharacter->GetLevel();
	m_CharacterFont.draw_text(GC,spacing + portraitPoint.x + portrait.get_width(), portraitPoint.y, pCharacter->GetName(), Font::TOP_LEFT);
	m_ClassFont.draw_text(GC,spacing + portraitPoint.x + portrait.get_width() + m_CharacterFont.get_text_size(GC, pCharacter->GetName()).width + spacing,
			      portraitPoint.y, pCharacter->GetClass()->GetName(),
			      Font::TOP_LEFT
 			    );
	m_LevelFont.draw_text(GC,spacing + spacing + portraitPoint.x + portrait.get_width(), portraitPoint.y + m_CharacterFont.get_font_metrics(GC).get_height(),
			      levelstream.str(),Font::TOP_LEFT);
	std::ostringstream hpstream;
	hpstream << "HP "
		 << std::setw(9)
		 << std::setfill(' ')
		 << pCharacter->GetAttribute(ICharacter::CA_HP)
		 << " / "
		 << std::setw(9)
		 << std::setfill(' ')
		 << pCharacter->GetAttribute(ICharacter::CA_MAXHP);
	std::ostringstream mpstream;
	mpstream << "MP "
		 << std::setw(9)
		 << std::setfill(' ')
		 << pCharacter->GetAttribute(ICharacter::CA_MP)
		 << " / "
		 << std::setw(9)
		 << std::setfill(' ')
		 << pCharacter->GetAttribute(ICharacter::CA_MAXMP);	
        std::ostringstream spstream;
        spstream << "SP "
                 << std::setw(9)
                 << std::setfill(' ')
                 << pCharacter->GetSP();
                 
       const float baseY = portraitPoint.y + m_CharacterFont.get_font_metrics(GC).get_height();
                 
	m_HPFont.draw_text(GC,spacing + spacing + portraitPoint.x + portrait.get_width(),
			   baseY + m_LevelFont.get_font_metrics(GC).get_height(),
			   hpstream.str(),Font::TOP_LEFT);
	m_MPFont.draw_text(GC,spacing + spacing + portraitPoint.x + portrait.get_width(),
			   baseY + m_LevelFont.get_font_metrics(GC).get_height() + m_HPFont.get_font_metrics(GC).get_height(),
			   mpstream.str(),Font::TOP_LEFT);
        m_SPFont.draw_text(GC,spacing + spacing + portraitPoint.x + portrait.get_width(),
                           baseY +  m_LevelFont.get_font_metrics(GC).get_height() + m_HPFont.get_font_metrics(GC).get_height() 
                           + m_MPFont.get_font_metrics(GC).get_height(),
                           spstream.str(),Font::TOP_LEFT);
    }
}

bool MainMenuState::hide_option ( int option ) const
{
    ParameterList params;
    return !m_choices[option]->Enabled(params);
}


bool MainMenuState::DisableMappableObjects() const
{
    return true;
}


void MainMenuState::MappableObjectMoveHook()
{
}

void MainMenuState::Start()
{
    m_bDone = false;

    reload();
	Menu::Init();
    SelectionFinish();
}

void MainMenuState::Finish()
{
}


void MainMenuState::Init()
{
}

CL_Rectf MainMenuState::get_rect()
{
    CL_Rectf menu_rect = m_menu_rect;
    menu_rect.shrink(GraphicsManager::GetMenuInset().x,GraphicsManager::GetMenuInset().y);
    menu_rect.translate(GraphicsManager::GetMenuInset());
    return menu_rect;
}

void MainMenuState::draw_option(int option, bool selected, float x, float y, CL_GraphicContext& gc)
{
        Font  lineFont;
	
	lineFont = m_optionFont;
	
	if(selected){
	    lineFont = m_selectionFont;  ;
	}

	
	m_choices[option]->GetIcon().draw(gc,x,y);
        lineFont.draw_text(gc, x + m_choices[option]->GetIcon().get_width() + 10,  y + lineFont.get_font_metrics(gc).get_height(),
                         m_choices[option]->GetName(), Font::DEFAULT);
}

int MainMenuState::height_for_option(CL_GraphicContext& gc){
#if 1 
    if(!m_choices.empty()){
		return 12 + max(m_selectionFont.get_font_metrics(gc).get_height(),max((float)m_choices[0]->GetIcon().get_height(), m_optionFont.get_font_metrics(gc).get_height()));
	}
	return 0;
#else
	return 42; // TODO: Make this a resource 
#endif
}

void MainMenuState::process_choice(int selection)
{
    m_choices[selection]->Select(ParameterList());
    if(m_choices[selection]->HasChildren()){
        m_option_parent = m_choices[selection];
        fill_choices(m_choices[selection]->GetChildrenBegin(),m_choices[selection]->GetChildrenEnd());
        Menu::PushMenu();		
    }
    
}

int MainMenuState::get_option_count()
{
    return m_choices.size();
}

void MainMenuState::fill_choices ( std::vector< MenuOption* >::const_iterator begin, std::vector< MenuOption* >::const_iterator end )
{
    m_choices.clear();
    for(std::vector<MenuOption*>::const_iterator iter = begin; iter != end; iter++)
        m_choices.push_back(*iter);
}


void MainMenuState::AddOption(MenuOption* pOption)
{
    m_root_choices.push_back(pOption);
}

SteelType MainMenuState::selectTargets(bool group)
{
    m_targetingState.Init(group);
    IApplication::GetInstance()->RunState(&m_targetingState);
    Party * party = IApplication::GetInstance()->GetParty();
    SteelType targets;
    if(m_nSelectedChar >= 0)
    {
	SteelArray array;
	if(m_bSelectAll)
	{

	    for(int i=0;i<party->GetCharacterCount();i++)
	    {
		SteelType man;
		man.set(party->GetCharacter(i));
		array.push_back(man);
	    }
	    targets.set(array);
	}
	else
	{
	    SteelType man;
	    man.set( party->GetCharacter(m_nSelectedChar) );
	    array.push_back(man);
	    targets.set(array);
	}
	
	return targets;
    }
    else
    {
	
	targets.set(SteelArray());
	return targets;
    }
}

SteelType MainMenuState::reload()
{
    m_target_sprite = GraphicsManager::CreateSprite("Menu/Target");

    m_portrait_shadow = GraphicsManager::CreateImage("Overlays/MainMenu/portrait_shadow");

    m_optionFont = GraphicsManager::GetFont(GraphicsManager::MAIN_MENU,"Option");
    m_selectionFont = GraphicsManager::GetFont(GraphicsManager::MAIN_MENU,"Selection");
    m_ClassFont = GraphicsManager::GetFont(GraphicsManager::MAIN_MENU,"Class");
    m_MPFont = GraphicsManager::GetFont(GraphicsManager::MAIN_MENU,"MP");
    m_SPFont = GraphicsManager::GetFont(GraphicsManager::MAIN_MENU,"SP");
    m_HPFont = GraphicsManager::GetFont(GraphicsManager::MAIN_MENU,"HP");
    m_LevelFont = GraphicsManager::GetFont(GraphicsManager::MAIN_MENU,"Level");
    m_CharacterFont = GraphicsManager::GetFont(GraphicsManager::MAIN_MENU,"Character");
    m_partyStatFont = GraphicsManager::GetFont(GraphicsManager::MAIN_MENU,"PartyStat");

    
    m_menu_rect = GraphicsManager::GetRect(GraphicsManager::MAIN_MENU,"menu");
    m_party_rect = GraphicsManager::GetRect(GraphicsManager::MAIN_MENU,"party");
    m_character_rect = GraphicsManager::GetRect(GraphicsManager::MAIN_MENU,"character");
    m_status_rect = GraphicsManager::GetRect(GraphicsManager::MAIN_MENU,"status");
    m_omega_rect = GraphicsManager::GetRect(GraphicsManager::MAIN_MENU,"omegas");
    m_option_parent = NULL;
    fill_choices(m_root_choices.begin(),m_root_choices.end());
    
    return SteelType();
}


void MainMenuState::SteelInit      (SteelInterpreter *pInterpreter)
{
    pInterpreter->addFunction("selectTargets","menu",new SteelFunctor1Arg<MainMenuState,bool>(this,&MainMenuState::selectTargets));
    pInterpreter->addFunction("reload","menu", new SteelFunctorNoArgs<MainMenuState>(this,&MainMenuState::reload));
}

void MainMenuState::SteelCleanup   (SteelInterpreter *pInterpreter)
{
    pInterpreter->removeFunctions("menu");
}

void MainMenuState::SelectCharacterUp()
{
    Party * party = IApplication::GetInstance()->GetParty();
    if(--m_nSelectedChar < 0){
	m_nSelectedChar = party->GetCharacterCount() - 1;
    }
}

void MainMenuState::SelectCharacterDown()
{
    Party * party = IApplication::GetInstance()->GetParty();
    if(++m_nSelectedChar >= party->GetCharacterCount()){
	m_nSelectedChar = 0;
    }    
}

void MainMenuState::SelectAllCharacters()
{
    m_nSelectedChar = 0;
    m_bSelectAll = true;
}

void MainMenuState::SelectionStart()
{
    m_bSelectAll = false;
    m_bSelectingTarget = true;
    m_nSelectedChar = 0;
}
void MainMenuState::SelectionFinish()
{
    m_bSelectingTarget = false;
}

void MainMenuState::SelectionCancel()
{
    m_nSelectedChar = -1;
    m_bSelectAll = false;
    m_bSelectingTarget = false;
}

}