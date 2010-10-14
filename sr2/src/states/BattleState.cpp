#include "BattleState.h"
#include "MonsterRef.h"
#include "CharacterManager.h"
#include "GraphicsManager.h"
#include "IApplication.h"
#include "MonsterGroup.h"
#include "Monster.h"
#include "Party.h"
#include "BattleMenu.h"
#include "AnimationState.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

using namespace StoneRing;


void BattleState::init(const std::vector<MonsterRef*>& monsters, int cellRows, int cellColumns, bool isBoss, const std::string & backdrop)
{
    CharacterManager * pCharManager = IApplication::GetInstance()->GetCharacterManager();
    GraphicsManager * pGraphicsManager = GraphicsManager::GetInstance();

    m_monsters = new MonsterParty();

    m_nRows = cellRows;
    m_nColumns = cellColumns;

    m_cur_char = 0;
    m_nRound = 0;
    
    m_bDoneAfterRound = false;

    m_bBossBattle = isBoss;
    // uint bottomrow = m_nRows - 1;
    
    for (std::vector<MonsterRef*>::const_iterator it = monsters.begin();
            it != monsters.end(); it++)
    {
        MonsterRef *pRef= *it;
        uint count = pRef->GetCount();

        for (int y=0;y<pRef->GetRows();y++)
        {
            for (int x=0;x<pRef->GetColumns();x++)
            {
                if (count>0)
                {
                    Monster * pMonster = pCharManager->CreateMonster(pRef->GetName());
                    pMonster->SetCellX( pRef->GetCellX() + x );
                    pMonster->SetCellY( pRef->GetCellY() + y );

                    pMonster->SetCurrentSprite(pGraphicsManager->CreateMonsterSprite(pMonster->GetName(),
                                               "idle")); // fuck, dude

                    m_monsters->AddMonster(pMonster);
                    pMonster->Invoke();
                }

                if (--count == 0) break;
            }
            if(count == 0) break;
        }

        if (count > 0) throw CL_Exception("Couldn't fit all monsters in their rows and columns");
    }
    m_backdrop = pGraphicsManager->GetBackdrop(backdrop);
}

void BattleState::init(const MonsterGroup &group, const std::string &backdrop)
{
    m_bBossBattle = false;
    const std::vector<MonsterRef*> & monsters = group.GetMonsters();

    init(monsters,group.GetCellRows(),group.GetCellColumns(),false,backdrop);
}

void BattleState::set_positions_to_loci()
{
    for(std::deque<ICharacter*>::iterator iter = m_initiative.begin();
    iter != m_initiative.end(); iter++)
    {
        CL_Point pos = get_character_locus_rect(*iter).get_top_left();
        (*iter)->SetBattlePos(pos);
    }
}

void BattleState::next_turn()
{
    if(m_bDoneAfterRound) 
    {
	m_bDone = true;
	return;
    }
    ICharacter * iCharacter = m_initiative[m_cur_char];
    Character * pCharacter = dynamic_cast<Character*>(iCharacter);
    set_positions_to_loci();
    iCharacter->StatusEffectRound();

    if (pCharacter != NULL)
    {
        m_combat_state = BATTLE_MENU;
        m_menu_stack.push ( pCharacter->GetBattleMenu() );
    }
    else
    {
        Monster * pMonster = dynamic_cast<Monster*>(m_initiative[m_cur_char]);
        assert(pMonster != NULL); // has to be a monster...
        // Figure out what the monster will do here.
        m_combat_state = DISPLAY_ACTION;
        ParameterList params;
        // Supply a handle to the character in question;
        params.push_back ( ParameterListItem("$Character", pMonster ) );
        params.push_back ( ParameterListItem("$Round", static_cast<int>(m_nRound) ) );
        pMonster->Round(params);
    }

}

void BattleState::pick_next_character()
{
    if (++m_cur_char == m_initiative.size())
    {
        m_cur_char = 0;
        m_nRound++;
    }
}

bool characterInitiativeSort(const ICharacter* pChar1, const ICharacter* pChar2)
{
    return pChar1->GetInitiative() > pChar2->GetInitiative();
}

void BattleState::roll_initiative()
{
    IParty * party = IApplication::GetInstance()->GetParty();

    for (uint i=0;i<party->GetCharacterCount();i++)
    {
        ICharacter * pChar = party->GetCharacter(i);
        pChar->RollInitiative();
        m_initiative.push_back(pChar);
    }

    for (uint i=0;i<m_monsters->GetCharacterCount();i++)
    {
        m_monsters->GetCharacter(i)->RollInitiative();
        m_initiative.push_back(m_monsters->GetCharacter(i));
    }

    // Okay they're all in the deque, now we just need to sort
    // it's ass
    std::sort(m_initiative.begin(),m_initiative.end(),characterInitiativeSort);

#ifndef NDEBUG
    std::cerr << "Initiative Order:" << std::endl;
    for (std::deque<ICharacter*>::const_iterator iter = m_initiative.begin(); iter != m_initiative.end();
            iter++)
    {
        std::cerr << '\t' <<(*iter)->GetName() << " @ " << (*iter)->GetInitiative() << std::endl;
    }
#endif
}

bool BattleState::IsDone() const
{
    return m_bDone;
}

void BattleState::HandleButtonUp(const IApplication::Button& button)
{
    switch(button)
    {
	case IApplication::BUTTON_CONFIRM:
	 if (m_combat_state == BATTLE_MENU)
        {
            std::list<BattleMenuOption*>::iterator it = m_menu_stack.top()->GetSelectedOption ();
            if (it != m_menu_stack.top()->GetOptionsEnd ())

            {
                BattleMenuOption * pOption = *it;

                ParameterList params;
                // Supply a handle to the character in question
                Character *pChar = dynamic_cast<Character*>(m_initiative[m_cur_char]);
                params.push_back ( ParameterListItem("$Character", pChar ) );
                params.push_back ( ParameterListItem("$Round", static_cast<int>(m_nRound) ) );

                if (pOption->Enabled(params))
                {
                    pOption->Select(m_menu_stack,params);
                }
            }
	    
	}
    }
}

void BattleState::HandleButtonDown(const IApplication::Button& button)
{
    
}

void BattleState::HandleAxisMove(const IApplication::Axis& axis, float pos)
{
    if(axis == IApplication::AXIS_VERTICAL)
    {
	if(pos == 1.0)
	{
	    if (m_combat_state == BATTLE_MENU)
		m_menu_stack.top()->SelectNext();
	}
	else if(pos == -1.0)
	{
	    if (m_combat_state == BATTLE_MENU)
		m_menu_stack.top()->SelectPrevious();
	}
    }
}

void BattleState::HandleKeyDown(const CL_InputEvent &key)
{

}

void BattleState::HandleKeyUp(const CL_InputEvent &key)
{

}

void BattleState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC)
{
    assert(m_draw_method != NULL);

    (this->*m_draw_method)(screenRect,GC);
}

bool BattleState::LastToDraw() const
{
    return false;
}

bool BattleState::DisableMappableObjects() const
{
    return true;
}

void BattleState::MappableObjectMoveHook()
{
}

void BattleState::StartTargeting()
{
    m_targets.selected.m_pTarget = NULL;
    m_targets.m_bSelectedGroup = false;
    m_combat_state = TARGETING;
}

void BattleState::FinishTargeting()
{
    m_combat_state = DISPLAY_ACTION;
}

void BattleState::Start()
{
    GraphicsManager * pGraphicsManager = GraphicsManager::GetInstance();
    m_draw_method = &BattleState::draw_battle;
    m_eState = COMBAT;
    const std::string status_resource = "Overlays/BattleStatus/";
    const std::string popup_resource = "Overlays/BattlePopup/";
    const std::string monster_rect_resource = "States/Battle/MonsterRect/";
    const std::string player_rect_resource= "States/Battle/PlayerRect/";
    IApplication * pApp = IApplication::GetInstance();
    CL_ResourceManager& resources = pApp->GetResources();

    m_bDone = false;

#if 0
    m_pStatusHPFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::HP);
    m_pStatusMPFont =  pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::MP);
    m_pStatusBPFont =  pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::BP);
    m_pStatusGeneralFont =  pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::GENERAL);
    m_pStatusBadFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::BAD);
#endif

    m_nStatusBarX = resources.get_integer_resource(status_resource + "x",0);
    m_nStatusBarY = resources.get_integer_resource(status_resource + "y",0);
    m_nPopupX = resources.get_integer_resource(popup_resource + "x",0);
    m_nPopupY = resources.get_integer_resource(popup_resource + "y",0);

    /**
     * TODO:
     * Refactor this resource shit so that you can just pull everything by state enum
     * from the graphics manager, and plan for future versions which allow the user to
     * select from themes like in FF games.
     * These states shouldn't have to know about the stupid resource manager and especially
     * the resource paths. They just go "Hey. I'm this state, and I want the font for this thing"
     * and the same with these rectangles
     */


    m_status_rect.top = resources.get_integer_resource(status_resource + "text/top",0);
    m_status_rect.left = resources.get_integer_resource(status_resource + "text/left",0);
    m_status_rect.right = resources.get_integer_resource(status_resource + "text/right",0);
    m_status_rect.bottom = resources.get_integer_resource(status_resource + "text/bottom",0);

    m_status_rect.top += m_nStatusBarY;
    m_status_rect.left += m_nStatusBarX;


    m_popup_rect.top = resources.get_integer_resource(popup_resource + "text/top",0);
    m_popup_rect.left = resources.get_integer_resource(popup_resource + "text/left",0);
    m_popup_rect.right = resources.get_integer_resource(popup_resource + "text/right",0);
    m_popup_rect.bottom = resources.get_integer_resource(popup_resource + "text/bottom",0);

    m_popup_rect.top += m_nPopupY;
    m_popup_rect.left += m_nPopupX;

    m_player_rect.top = resources.get_integer_resource(player_rect_resource + "top",0);
    m_player_rect.left = resources.get_integer_resource(player_rect_resource + "left",0);
    m_player_rect.right = resources.get_integer_resource(player_rect_resource + "right",0);
    m_player_rect.bottom = resources.get_integer_resource(player_rect_resource + "bottom",0);


    m_monster_rect.top = resources.get_integer_resource(monster_rect_resource + "top",0);
    m_monster_rect.left = resources.get_integer_resource(monster_rect_resource + "left",0);
    m_monster_rect.right = resources.get_integer_resource(monster_rect_resource + "right",0);
    m_monster_rect.bottom = resources.get_integer_resource(monster_rect_resource + "bottom",0);


    m_statusBar = pGraphicsManager->GetOverlay(GraphicsManager::BATTLE_STATUS);
    m_battleMenu = pGraphicsManager->GetOverlay(GraphicsManager::BATTLE_MENU);
    m_battlePopup = pGraphicsManager->GetOverlay(GraphicsManager::BATTLE_POPUP_MENU);
    init_or_release_players(false);

	m_bDone = false;
    roll_initiative();
    set_positions_to_loci();
    next_turn();
}

void BattleState::init_or_release_players(bool bRelease)
{
    GraphicsManager * pGraphicsManager = GraphicsManager::GetInstance();
    IParty * pParty = IApplication::GetInstance()->GetParty();

    uint count = pParty->GetCharacterCount();
    for (uint nPlayer = 0;nPlayer < count; nPlayer++)
    {
        Character * pCharacter = dynamic_cast<Character*>(pParty->GetCharacter(nPlayer));
        assert(pCharacter);
        std::string name = pCharacter->GetName();
        if (!bRelease)
            pCharacter->SetCurrentSprite(pGraphicsManager->CreateCharacterSprite(name,"idle")); // bullshit
    }
}


void BattleState::Finish()
{

    for (uint i =0; i < m_monsters->GetCharacterCount();i++)
        delete m_monsters->GetCharacter(i);

    delete m_monsters;

    m_initiative.clear();

    init_or_release_players(true);
}


void BattleState::draw_transition_in(const CL_Rect &screenRect, CL_GraphicContext& GC)
{
}

void BattleState::draw_start(const CL_Rect &screenRect, CL_GraphicContext& GC)
{
}

void BattleState::draw_status(const CL_Rect &screenRect, CL_GraphicContext& GC)
{
    IParty * pParty = IApplication::GetInstance()->GetParty();
    GraphicsManager * pGraphicsManager = GraphicsManager::GetInstance();
    m_statusBar.draw(GC,(int)m_nStatusBarX,(int)m_nStatusBarY);

    for (uint p = 0; p < pParty->GetCharacterCount(); p++)
    {
        ICharacter * pChar = pParty->GetCharacter(p);
        std::ostringstream name;
        name << std::setw(16) << pChar->GetName();
        std::ostringstream hp;
        hp << std::setw(6) << pChar->GetAttribute(ICharacter::CA_HP) << '/'
        << pChar->GetAttribute(ICharacter::CA_MAXHP);
        CL_Font generalFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, "general");
        CL_Font  hpFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, "hp");
        CL_Font  mpFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, "mp");

        generalFont.draw_text(GC,m_status_rect.left,
                              static_cast<int>(m_status_rect.top + generalFont.get_font_metrics(GC).get_height() +(p *
                                               generalFont.get_font_metrics(GC).get_height())
                                              ),
                              name.str());
        hpFont.draw_text(GC,m_status_rect.get_width() / 3 + m_status_rect.left,
                         static_cast<int>(m_status_rect.top + hpFont.get_font_metrics(GC).get_height()+(p* hpFont.get_font_metrics(GC).get_height()))
                         ,hp.str());
        std::ostringstream mp;
        mp << std::setw(6) << pChar->GetAttribute(ICharacter::CA_MP) << '/'
        << pChar->GetAttribute(ICharacter::CA_MAXMP);
        mpFont.draw_text(GC,(m_status_rect.get_width() / 3) * 2 + m_status_rect.left,
                         static_cast<int>(m_status_rect.top + mpFont.get_font_metrics(GC).get_height() + (p*mpFont.get_font_metrics(GC).get_height())),mp.str());

    }
}

void BattleState::draw_battle(const CL_Rect &screenRect, CL_GraphicContext& GC)
{
    m_backdrop.draw(GC,screenRect);

    draw_players(m_player_rect,GC);
    draw_monsters(m_monster_rect,GC);

    
    draw_status(screenRect,GC);
    draw_displays(GC);

    if (m_combat_state == BATTLE_MENU)
    {
        draw_menus(screenRect,GC);
    }

}

BattleState::Display::Display(BattleState& parent,BattleState::Display::eDisplayType type,int damage,SteelType::Handle pICharacter)
        :m_parent(parent),m_eDisplayType(type),m_amount(-damage)
{
    m_pTarget = static_cast<ICharacter*>(pICharacter);
}
BattleState::Display::~Display()
{
}

void BattleState::Display::start()
{
    m_start_time = CL_System::get_time();
    m_complete = 0.0f;
}

bool BattleState::Display::expired() const
{
    return m_complete >= 1.0f;
}

void BattleState::Display::update()
{
    int elapsed = CL_System::get_time()-m_start_time;

    m_complete = (double)elapsed / 2000.0;
}
void BattleState::Display::draw(CL_GraphicContext& GC)
{
    std::ostringstream stream;
    CL_Font font;
    CL_Colorf color;
    CL_Colorf shadow_color;

    shadow_color.set_red(0.0f);
    shadow_color.set_green(0.0f);
    shadow_color.set_blue(0.0f);


    GraphicsManager * pGraphics = GraphicsManager::GetInstance();

    GraphicsManager::DisplayFont displayFont;

    switch (m_eDisplayType)
    {
    case DISPLAY_DAMAGE:
        if (m_amount >= 0)
        {
            displayFont = GraphicsManager::DISPLAY_HP_POSITIVE;
            stream << m_amount;
        }
        else
        {
            displayFont = GraphicsManager::DISPLAY_HP_NEGATIVE;
            stream << -m_amount;
        }
        break;
    case DISPLAY_MP:
        if (m_amount >= 0)
        {
            displayFont = GraphicsManager::DISPLAY_MP_POSITIVE;
            stream << m_amount;
        }
        else
        {
            displayFont = GraphicsManager::DISPLAY_MP_NEGATIVE;
            stream << -m_amount;
        }
        break;
    case DISPLAY_MISS:
        displayFont = GraphicsManager::DISPLAY_MISS;
        stream << "MISS";
        break;

    }

    font = pGraphics->GetDisplayFont(displayFont);
    color = pGraphics->GetFontColor(displayFont);

    color.set_alpha(1.0f - m_complete);
    shadow_color.set_alpha(1.0f-m_complete);

    CL_Rect rect;
    Monster *pMonster = dynamic_cast<Monster*>(m_pTarget);

    if (pMonster)
    {
        rect = m_parent.get_character_rect(pMonster);
    }
    else
    {
        IParty * pParty = IApplication::GetInstance()->GetParty();
        for (uint n=0;n<pParty->GetCharacterCount();n++)
        {
            if (m_pTarget == pParty->GetCharacter(n))
            {
                rect = m_parent.get_character_rect(pParty->GetCharacter(n));
                break;
            }
        }
    }

    CL_String value(stream.str());

    CL_Point pos = rect.get_top_right();

    pos.y += font.get_font_metrics(GC).get_height();
    pos.x -= font.get_text_size(GC,value).width;

    font.draw_text(GC,pos.x-2,pos.y-2,value,shadow_color);
    font.draw_text(GC,pos.x,pos.y,value,color);


}

bool SortByBattlePos(const ICharacter* d1, const ICharacter* d2)
{

    CL_Point pos1,pos2;
    pos1 = d1->GetBattlePos();
    pos2 = d2->GetBattlePos();

    
  return pos1.y * IApplication::GetInstance()->GetScreenWidth() + pos1.x <
	pos2.y * IApplication::GetInstance()->GetScreenWidth() + pos2.x;
}


void BattleState::draw_monsters(const CL_Rect &monsterRect, CL_GraphicContext& GC)
{
    std::vector<Monster*> sortedList;
    sortedList.reserve(m_monsters->GetCharacterCount());
    for(uint i=0;i<m_monsters->GetCharacterCount();i++)
    {
	Monster *pMonster = dynamic_cast<Monster*>(m_monsters->GetCharacter(i));
	sortedList.push_back(pMonster);	
    }
    
   std::sort(sortedList.begin(), sortedList.end(), SortByBattlePos);

    

    for (uint i = 0; i < sortedList.size(); i++)
    {
        Monster *pMonster = sortedList[i];

       // ICharacter *iCharacter = m_monsters->GetCharacter(i);
        if (!pMonster->GetToggle(ICharacter::CA_VISIBLE))
            continue;

        CL_Rect rect = get_character_rect(pMonster);

        CL_Sprite  sprite = pMonster->GetCurrentSprite();
        sprite.set_alpha(1.0f);
        sprite.update();

        if (m_combat_state == TARGETING)
        {
            if ((m_targets.m_bSelectedGroup && m_targets.selected.m_pGroup == m_monsters)
                    || (!m_targets.m_bSelectedGroup && pMonster == m_targets.selected.m_pTarget))
            {
                sprite.draw(GC,rect);
            }
            else
            {
                sprite.set_alpha(0.7f);
                sprite.draw(GC,rect);
            }
        }
        else
        {
            sprite.draw(GC,rect);
        }
        

    }
}

void BattleState::draw_players(const CL_Rect &playerRect, CL_GraphicContext& GC)
{
    IParty * pParty = IApplication::GetInstance()->GetParty();

    uint playercount = pParty->GetCharacterCount();
    for (uint nPlayer = 0; nPlayer < playercount; nPlayer++)
    {
        Character * pCharacter = dynamic_cast<Character*>(pParty->GetCharacter(nPlayer));
        ICharacter * iCharacter = pParty->GetCharacter(nPlayer);
        CL_Sprite  sprite = pCharacter->GetCurrentSprite();
        CL_Rect rect = get_character_rect(iCharacter);
        sprite.set_alpha(1.0f);
        sprite.update();

        // Need to get the spacing from game config
        if (!pCharacter->GetToggle(ICharacter::CA_VISIBLE))
            continue;

        if (m_combat_state == TARGETING)
        {
            if ((m_targets.m_bSelectedGroup && m_targets.selected.m_pGroup == m_monsters)
                    || (!m_targets.m_bSelectedGroup && iCharacter == m_targets.selected.m_pTarget))
            {
                sprite.draw(GC,rect);
            }
            else
            {
                sprite.set_alpha(0.7f);
                sprite.draw(GC,rect);

            }
        }
        else
        {
            sprite.draw(GC,rect);
        }

    }
}

CL_Rect  BattleState::get_group_rect(ICharacterGroup* group) const
{
    MonsterParty * monsterParty = dynamic_cast<MonsterParty*>(group);

    if(monsterParty != NULL){
        return m_monster_rect;
    }else{
        return m_player_rect;
    }
}

CL_Point BattleState::get_monster_locus(const Monster * pMonster)const
{
    CL_Point point;
    const uint cellWidth = m_monster_rect.get_width() / m_nColumns;
    const uint cellHeight = m_monster_rect.get_height() / m_nRows;
    CL_Sprite  sprite = pMonster->GetCurrentSprite();


    int drawX = pMonster->GetCellX() * cellWidth + (cellWidth - sprite.get_width()) /2;
    int drawY = pMonster->GetCellY() * cellHeight + (cellHeight - sprite.get_height())/2;
    point.x = drawX;
    point.y = drawY;
    return point;
}

CL_Point BattleState::get_player_locus(uint nPlayer)const
{
    CL_Point point;
    point.x = static_cast<int>(m_player_rect.left + nPlayer * 64 + (m_player_rect.get_width() - 32) / 2);
    point.y = static_cast<int>(m_player_rect.top + nPlayer * 64);
    return point;
}

CL_Point BattleState::get_character_locus(const ICharacter* pCharacter) const
{
    CL_Point point;
    const Monster * pMonster = dynamic_cast<const Monster*>(pCharacter);
    if(pMonster != NULL){
        return get_monster_locus(pMonster);
    }else{
        IParty * pParty = IApplication::GetInstance()->GetParty();
        uint playercount = pParty->GetCharacterCount();
        for(uint i=0;i<playercount;i++){
            ICharacter * iCharacter = pParty->GetCharacter(i);

            if(pCharacter == iCharacter){
                return get_player_locus(i);
            }
        }
    }
    assert(0);
    return CL_Point(0.0,0.0);
}

CL_Size  BattleState::get_character_size(const ICharacter* pCharacter) const
{
    const Monster * pMonster = dynamic_cast<const Monster*>(pCharacter);
    if(pMonster != NULL){
        return pMonster->GetCurrentSprite().get_size();
    }else{
        return CL_Size(64,128);
    }
}


CL_Rect BattleState::get_character_rect(const ICharacter* pCharacter)const
{
    CL_Rect rect;
    CL_Point point = pCharacter->GetBattlePos();
    rect.set_top_left(point);
    rect.set_size(get_character_size(pCharacter));

    return rect;
}

CL_Rect BattleState::get_character_locus_rect(const ICharacter* pCharacter)const
{
    CL_Rect rect;
    CL_Point point = get_character_locus(pCharacter);
    rect.set_top_left(point);
    rect.set_size(get_character_size(pCharacter));

    return rect;
}


void BattleState::draw_displays(CL_GraphicContext& GC)
{
    for (std::list<Display>::iterator iter = m_displays.begin();iter != m_displays.end();iter++)
    {
        iter->update();
        iter->draw(GC);
    }

    // m_displays.remove_if(Display::has_expired);
    m_displays.remove_if(std::mem_fun_ref(&Display::expired));
}


void BattleState::draw_menus(const CL_Rect &screenrect, CL_GraphicContext& GC)
{
    BattleMenu * pMenu = m_menu_stack.top();
    GraphicsManager * pGraphicsManager = GraphicsManager::GetInstance();
    CL_Font  onFont;
    CL_Font  offFont;
    CL_Font  selectedFont;

    ParameterList params;
    // Supply a handle to the character in question
    Character *pChar = dynamic_cast<Character*>(m_initiative[m_cur_char]);
    params.push_back ( ParameterListItem("$Character", pChar ) );
    params.push_back ( ParameterListItem("$Round", static_cast<int>(m_nRound) ) );

    if (pMenu->GetType() == BattleMenu::POPUP)
    {
        onFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_POPUP_MENU,"on");
        offFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_POPUP_MENU,"off");
        selectedFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_POPUP_MENU,"Selection");
        m_battlePopup.draw(GC,(int)m_nPopupX,(int)m_nPopupY);
        std::list<BattleMenuOption*>::iterator iter;
        uint pos = 0;

        for (iter = pMenu->GetOptionsBegin();iter != pMenu->GetOptionsEnd();iter++)
        {
            BattleMenuOption * pOption = *iter;
            CL_Font  font;
            if (pOption->Enabled(params))
            {
                font = onFont;
            }
            else
            {
                font = offFont;
            }

            bool selected = false;

            if (m_menu_stack.top()->GetSelectedOption() == iter)
            {
                font = selectedFont;
                selected = true;
            }

            CL_Image icon = pOption->GetIcon();

            if (!selected)
            {
                icon.set_alpha(0.5f);
            }
            else
            {
                icon.set_alpha(1.0f);
            }

            const float option_height = cl_max(font.get_font_metrics(GC).get_height(), (float)icon.get_height());



            icon.draw(GC,static_cast<int>(m_popup_rect.left + 14), static_cast<int>(20 + m_popup_rect.top + option_height* pos));

            font.draw_text(GC,static_cast<int>(m_popup_rect.left + 24 + icon.get_width()) , static_cast<int>(20 + font.get_font_metrics(GC).get_height() +  m_popup_rect.top + option_height * pos),pOption->GetName());

            ++pos;
        }
    }
    else
    {
        m_battleMenu.draw(GC,screenrect.left,screenrect.top);
    }
}
void BattleState::SteelInit(SteelInterpreter* pInterpreter)
{
    pInterpreter->pushScope();

    static SteelFunctor3Arg<BattleState,bool,bool,bool> fn_selectTargets(this,&BattleState::selectTargets);
    static SteelFunctorNoArgs<BattleState> fn_finishTurn(this,&BattleState::finishTurn);
    static SteelFunctorNoArgs<BattleState> fn_cancelOption(this,&BattleState::cancelOption);
    static SteelFunctor3Arg<BattleState,SteelType::Handle,SteelType::Handle,const std::string&> fn_doTargetedAnimation(this,&BattleState::doTargetedAnimation);
	static SteelFunctor2Arg<BattleState,SteelType::Handle,const std::string&> fn_doCharacterAnimation(this,&BattleState::doCharacterAnimation);
    static SteelFunctor3Arg<BattleState,int,SteelType::Handle,int> fn_createDisplay(this,&BattleState::createDisplay);
    static SteelFunctor1Arg<BattleState,bool> fn_getCharacterGroup(this,&BattleState::getCharacterGroup);
    static SteelFunctorNoArgs<BattleState> fn_getAllCharacters(this,&BattleState::getAllCharacters);
    static SteelFunctor1Arg<BattleState,SteelType::Handle> fn_getMonsterDamageCategory(this,&BattleState::getMonsterDamageCategory);
    static SteelFunctor2Arg<BattleState,SteelType::Handle,const std::string&> fn_doSkill(this,&BattleState::doSkill);
    static SteelFunctorNoArgs<BattleState> fn_flee(this,&BattleState::flee);
    static SteelFunctorNoArgs<BattleState> fn_isBossBattle(this,&BattleState::isBossBattle);


    SteelConst(pInterpreter,"$_DISP_DAMAGE", Display::DISPLAY_DAMAGE);
    SteelConst(pInterpreter,"$_DISP_MP", Display::DISPLAY_MP);
    SteelConst(pInterpreter,"$_DISP_MISS", Display::DISPLAY_MISS);

    pInterpreter->addFunction("selectTargets","battle",&fn_selectTargets);
    pInterpreter->addFunction("finishTurn","battle",&fn_finishTurn);
    pInterpreter->addFunction("cancelOption","battle",&fn_cancelOption);
    pInterpreter->addFunction("doTargetedAnimation","battle",&fn_doTargetedAnimation);
	pInterpreter->addFunction("doCharacterAnimation","battle",&fn_doCharacterAnimation);
    pInterpreter->addFunction("createDisplay","battle",&fn_createDisplay);
    pInterpreter->addFunction("getCharacterGroup","battle",&fn_getCharacterGroup);
    pInterpreter->addFunction("getAllCharacters","battle",&fn_getAllCharacters);
    pInterpreter->addFunction("getMonsterDamageCategory","battle",&fn_getMonsterDamageCategory);
    pInterpreter->addFunction("doSkill","battle",&fn_doSkill);
    pInterpreter->addFunction("flee","battle",&fn_flee);
    pInterpreter->addFunction("isBossBattle","battle",&fn_isBossBattle);

}

void BattleState::SteelCleanup   (SteelInterpreter* pInterpreter)
{
    pInterpreter->removeFunctions("battle",false);
    pInterpreter->popScope();
}

ICharacter* BattleState::get_next_character(const ICharacterGroup* pParty, const ICharacter* pCharacter) const
{
    const int party_count = pParty->GetCharacterCount();
    for (int i = 0;i<party_count;i++)
    {
        if (pParty->GetCharacter(i) == pCharacter)
        {
            if (i +1 == party_count)
            {
                return pParty->GetCharacter(0);
            }
            else
            {
                return pParty->GetCharacter(i+1);
            }
        }
    }

    assert(0);
    return NULL;
}

ICharacter* BattleState::get_prev_character(const ICharacterGroup* pParty, const ICharacter* pCharacter) const
{
    const int party_count = pParty->GetCharacterCount();
    for (int i = 0;i<party_count;i++)
    {
        if (pParty->GetCharacter(i) == pCharacter)
        {
            if (i - 1 < 0)
            {
                return pParty->GetCharacter(party_count-1);
            }
            else
            {
                return pParty->GetCharacter(i-1);
            }
        }

    }

    assert(0);
    return NULL;
}

#if 0
void BattleState::SelectNextTarget()
{
    Monster * pMonster = dynamic_cast<Monster*>(m_targets.selected.m_pTarget);
    if (pMonster != NULL)
    {
        m_targets.selected.m_pTarget = get_next_character(m_monsters,pMonster);
    }
    else
    {
        Character * pCharacter = dynamic_cast<Character*>(m_targets.selected.m_pTarget);
        const Party * pParty = dynamic_cast<Party*>(IApplication::GetInstance()->GetParty());
        m_targets.selected.m_pTarget = get_next_character(pParty,pCharacter);
    }
}

void BattleState::SelectPreviousTarget()
{
    Monster * pMonster = dynamic_cast<Monster*>(m_targets.selected.m_pTarget);
    if (pMonster != NULL)
    {
        // We have a monster
        m_targets.selected.m_pTarget = get_prev_character(m_monsters,pMonster);
    }
    else
    {
        Character * pCharacter = dynamic_cast<Character*>(m_targets.selected.m_pTarget);
        const Party * pParty = dynamic_cast<Party*>(IApplication::GetInstance()->GetParty());
        m_targets.selected.m_pTarget = get_prev_character(pParty,pCharacter);
    }
}

void BattleState::SelectLeftTarget()
{

}

void BattleState::SelectRightTarget()
{

}


void BattleState::SelectUpTarget()
{

}
void BattleState::SelectDownTarget()
{


}

#endif		

ICharacterGroup* BattleState::group_for_character(ICharacter* pICharacter)
{
	if(pICharacter->IsMonster()) return m_monsters;
	else return IApplication::GetInstance()->GetParty();
}

bool BattleState::MonstersOnLeft()
{
    return true;
}

void BattleState::FinishTurn()
{
    // TODO: Good time to check if either side is wiped out, etc
    // And, any dead monsters need to have ->Remove called on them
    check_for_death();
    if(!end_conditions()){
	pick_next_character();
	next_turn();
    }
}


// Go back to menu, they decided not to proceed with this option
void BattleState::CancelTargeting()
{
    m_targets.m_bSelectedGroup = false;
    m_targets.selected.m_pTarget = NULL;
}

bool BattleState::end_conditions()
{
    IParty * party = IApplication::GetInstance()->GetParty();
       // Now, see if the monsters are all dead
    bool anyAlive = false;
    for(int i=0;i<m_monsters->GetCharacterCount();i++)
    {
	if(m_monsters->GetCharacter(i)->GetToggle(ICharacter::CA_ALIVE))
	{
	    anyAlive= true;
	    break;
	}
    }
    
    if(!anyAlive)
    {
	// You win!
	// TODO: Win
	win();
	// return so you don't lose after you win
	return true;
    }
    
    anyAlive = false;
    
    for(int i=0;i<party->GetCharacterCount();i++)
    {
	if(party->GetCharacter(i)->GetToggle(ICharacter::CA_ALIVE) )
	{
	    anyAlive = true;
	    break;
	}
    }
    
    if(!anyAlive)
    {
	lose();
	return true;
    }
    
    return false;
}


void BattleState::check_for_death()
{

    for (std::deque<ICharacter*>::const_iterator iter = m_initiative.begin();
            iter != m_initiative.end(); iter++)
    {
        Monster * pMonster = dynamic_cast<Monster*>(*iter);

        if (pMonster)
        {
            if (!pMonster->GetToggle(ICharacter::CA_ALIVE) && !pMonster->DeathAnimated())
            {
                pMonster->Die();
                death_animation(pMonster);
                pMonster->SetToggle(ICharacter::CA_VISIBLE,false);
            }
        }
        else
        {
            // Change PC sprite to dead one?
        }
    }
    // BRING OUT YOUR DEAD!
    m_initiative.erase(std::remove_if(m_initiative.begin(), m_initiative.end(),
				      std::not1(std::bind2nd(std::mem_fun(&ICharacter::GetToggle),ICharacter::CA_ALIVE)))
				    ,m_initiative.end());
    
}

void BattleState::death_animation(Monster* pMonster)
{
    pMonster->MarkDeathAnimated();
}

void BattleState::lose()
{
    m_bDone = true;
    // TODO: Put up lose state
    std::cout << "Lose :(" << std::endl;
}

void BattleState::win()
{
    m_bDone = true;
    // TODO: Put up win state
    std::cout << "Win!" << std::endl;
}


/***** BIFS *****/
SteelType BattleState::selectTargets(bool single, bool group, bool defaultMonsters)
{
    m_combat_state = TARGETING;
    std::vector<SteelType> targets;
    TargetingState::Targetable targetable;
    if (single && group)
        targetable = TargetingState::EITHER;
    else if (group)
        targetable = TargetingState::GROUP;
    else
        targetable = TargetingState::SINGLE;

    m_targeting_state.Init(this,targetable,defaultMonsters);
    IApplication::GetInstance()->RunState(&m_targeting_state);


    if (m_targets.m_bSelectedGroup)
    {

        for (uint i=0;i<m_targets.selected.m_pGroup->GetCharacterCount();i++)
        {
            SteelType ref;
            ref.set(m_targets.selected.m_pGroup->GetCharacter(i));
            targets.push_back(ref);
        }

    }
    else
    {
        SteelType ref;
	if(m_targets.selected.m_pTarget){
	    ref.set(m_targets.selected.m_pTarget);
	    targets.push_back(ref);
	}
    }

    SteelType val;
    val.set(targets);
    return val;
}

SteelType BattleState::finishTurn()
{
    FinishTurn();
    return SteelType();
}
// if they back out and want to go back to the battle menu
SteelType BattleState::cancelOption()
{
    m_combat_state = BATTLE_MENU;
    return SteelType();
}

SteelType BattleState::doTargetedAnimation(SteelType::Handle pICharacter, SteelType::Handle pITarget,const std::string& animation)
{
    ICharacter * character = reinterpret_cast<ICharacter*>(pICharacter);
    ICharacter * target = reinterpret_cast<ICharacter*>(pITarget);
    Animation * anim = AbilityManager::GetAnimation(animation);
    if(anim == NULL)
    {
	throw CL_Exception ("Animation was missing: " + animation);
    }
    StoneRing::AnimationState state(*this, group_for_character(character), group_for_character(target), character, target);
    state.Init(anim);

    IApplication::GetInstance()->RunState (&state);
    
    return SteelType();
}

SteelType BattleState::doCharacterAnimation(SteelType::Handle pICharacter,const std::string& animation)
{
	ICharacter * character = reinterpret_cast<ICharacter*>(pICharacter);
	Animation * anim = AbilityManager::GetAnimation(animation);
	if(anim == NULL)
	{
		throw CL_Exception ("Animation was missing: " + animation);
	}
	StoneRing::AnimationState state(*this, group_for_character(character), NULL, character, NULL);
	state.Init(anim);

	IApplication::GetInstance()->RunState (&state);
	
    return SteelType();
}

SteelType BattleState::createDisplay(int damage,SteelType::Handle hICharacter,int display_type)
{
    //            Display(BattleState& parent,eDisplayType type,int damage,SteelType::Handle pICharacter);
    Display display(*this, static_cast<Display::eDisplayType>(display_type),damage,hICharacter);
    display.start();
    m_displays.push_back(display);

    return SteelType();
}

SteelType BattleState::getCharacterGroup(bool monsters)
{
    std::vector<SteelType> vector;
    ICharacterGroup * group;

    if (monsters)
    {
        group = m_monsters;
    }
    else
    {
        group = IApplication::GetInstance()->GetParty();
    }

    for (uint i=0;i<group->GetCharacterCount();i++)
    {
        SteelType handle;
        handle.set( group->GetCharacter(i) );
        vector.push_back(handle);
    }
    SteelType array;
    array.set(vector);
    return array;
}

SteelType BattleState::getAllCharacters()
{
    std::vector<SteelType> vector;

    for (uint i=0;i<m_initiative.size();i++)
    {
        SteelType handle;
        handle.set ( m_initiative[i] );
        vector.push_back(handle);
    }

    SteelType array;
    array.set(vector);
    return array;
}

SteelType BattleState::getMonsterDamageCategory(SteelType::Handle hMonster)
{
    SteelType result;
    Monster * pMonster = static_cast<Monster*>(hMonster);

    result.set ( pMonster->GetDefaultDamageCategory() );

    return result;
}

SteelType BattleState::flee()
{
    m_bDoneAfterRound = true;
    return SteelType();
}

SteelType BattleState::isBossBattle()
{
    SteelType val;
    val.set(m_bBossBattle);
    
    return val;
}

SteelType BattleState::doSkill(SteelType::Handle pICharacter, const std::string& whichskill)
{
    AbilityManager * AbilityManager = IApplication::GetInstance()->GetAbilityManager();
    if(!AbilityManager->SkillExists(whichskill)){
        throw CL_Exception("(doSkill) Skill doesn't exist: " + whichskill);
    }

    ParameterList params;
    // Supply a handle to the character in question;
    params.push_back ( ParameterListItem("$Character", pICharacter ) );
    params.push_back ( ParameterListItem("$Round", static_cast<int>(m_nRound) ) );


    Skill * pSkill = AbilityManager->GetSkill(whichskill);

    pSkill->Select(params);
    pSkill->Invoke(params);

    return SteelType();
}
