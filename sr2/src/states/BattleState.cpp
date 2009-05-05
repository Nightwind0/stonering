#include "BattleState.h"
#include "MonsterRef.h"
#include "CharacterManager.h"
#include "GraphicsManager.h"
#include "IApplication.h"
#include "MonsterGroup.h"
#include "Monster.h"
#include "Party.h"
#include "BattleMenu.h"
#include <sstream>
#include <iomanip>

using namespace StoneRing;


void BattleState::init(const MonsterGroup &group, const std::string &backdrop)
{
    CharacterManager * pCharManager = IApplication::GetInstance()->GetCharacterManager();
    GraphicsManager * pGraphicsManager = GraphicsManager::GetInstance();

    m_target_sprite = pGraphicsManager->CreateSprite("Battle/Target");

    m_monsters = new MonsterParty();

    m_nRows = group.GetCellRows();
    m_nColumns = group.GetCellColumns();

    m_cur_char = 0;
    m_nRound = 0;

    uint bottomrow = m_nRows - 1;

    const std::vector<MonsterRef*> & monsters = group.GetMonsters();

    for (std::vector<MonsterRef*>::const_iterator it = monsters.begin();
            it != monsters.end(); it++)
    {
        MonsterRef *pRef= *it;
        uint count = pRef->GetCount();

        for (int x=0;x<pRef->GetColumns();x++)
        {
            for (int y=0;y<pRef->GetRows();y++)
            {
                if (count>0)
                {
                    Monster * pMonster = pCharManager->CreateMonster(pRef->GetName());
                    pMonster->SetCellX( pRef->GetCellX() + x );
                    pMonster->SetCellY( bottomrow - pRef->GetCellY() - y);

                    pMonster->SetCurrentSprite(pGraphicsManager->CreateMonsterSprite(pMonster->GetName(),
                                               "idle")); // fuck, dude

                    m_monsters->AddMonster(pMonster);
                }

                if (--count == 0) break;
            }
        }

        if (count > 0) throw CL_Error("Couldn't fit all monsters in their rows and columns");
    }
    m_pBackdrop = pGraphicsManager->GetBackdrop(backdrop);
    m_bDone = false;
    roll_initiative();
    next_turn();
}

void BattleState::next_turn()
{
    Character * pCharacter = dynamic_cast<Character*>(m_initiative[m_cur_char]);

    if (pCharacter != NULL)
    {
        m_combat_state = BATTLE_MENU;
        m_menu_stack.push ( pCharacter->GetBattleMenu() );
    }
    else
    {
        Monster * pMonster = dynamic_cast<Monster*>(m_initiative[m_cur_char]);
        cl_assert(pMonster != NULL); // has to be a monster...
        // Figure out what the monster will do here.
        m_combat_state = DISPLAY_ACTION;
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

void BattleState::HandleKeyDown(const CL_InputEvent &key)
{
    switch (key.id)
    {
    case CL_KEY_DOWN:
        if (m_combat_state == BATTLE_MENU)
            m_menu_stack.top()->SelectNext();
        break;
    case CL_KEY_UP:
        if (m_combat_state == BATTLE_MENU)
            m_menu_stack.top()->SelectPrevious();
        break;
    }
}

void BattleState::HandleKeyUp(const CL_InputEvent &key)
{
    switch (key.id)
    {
    case CL_KEY_ESCAPE:
        m_bDone = true;
        break;
    case CL_KEY_ENTER:
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
                    pOption->Select(m_menu_stack,params);
            }
        }
    }
}

void BattleState::Draw(const CL_Rect &screenRect,CL_GraphicContext * pGC)
{
    assert(m_draw_method != NULL);

    (this->*m_draw_method)(screenRect,pGC);
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
    const std::string resource = "Overlays/BattleStatus/";
    IApplication * pApp = IApplication::GetInstance();
    CL_ResourceManager *pResources = pApp->GetResources();

    m_bDone = false;

#if 0
    m_pStatusHPFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::HP);
    m_pStatusMPFont =  pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::MP);
    m_pStatusBPFont =  pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::BP);
    m_pStatusGeneralFont =  pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::GENERAL);
    m_pStatusBadFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::BAD);
#endif

    m_nStatusBarX = CL_Integer::load(resource + "x",pResources);
    m_nStatusBarY = CL_Integer::load(resource + "y",pResources);

    /**
     * TODO:
     * Refactor this resource shit so that you can just pull everything by state enum
     * from the graphics manager, and plan for future versions which allow the user to
     * select from themes like in FF games.
     * These states shouldn't have to know about the stupid resource manager and especially
     * the resource paths. They just go "Hey. I'm this state, and I want the font for this thing"
     * and the same with these rectangles
     */


    m_status_rect.top = CL_Integer(resource + "text/top", pResources);
    m_status_rect.left = CL_Integer(resource + "text/left", pResources);
    m_status_rect.right = CL_Integer(resource + "text/right", pResources);
    m_status_rect.bottom = CL_Integer(resource + "text/bottom", pResources);

    m_status_rect.top += m_nStatusBarY;
    m_status_rect.left += m_nStatusBarX;

    m_pStatusBar = pGraphicsManager->GetOverlay(GraphicsManager::BATTLE_STATUS);
    m_pBattleMenu = pGraphicsManager->GetOverlay(GraphicsManager::BATTLE_MENU);
    m_pBattlePopup = pGraphicsManager->GetOverlay(GraphicsManager::BATTLE_POPUP_MENU);
    init_or_release_players(false);
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
        else delete pCharacter->GetCurrentSprite();
    }
}


void BattleState::Finish()
{

    for (int i =0; i < m_monsters->GetCharacterCount();i++)
        delete m_monsters->GetCharacter(i);

    delete m_monsters;

    m_initiative.clear();

    init_or_release_players(true);
}


void BattleState::draw_transition_in(const CL_Rect &screenRect, CL_GraphicContext *pGC)
{
}

void BattleState::draw_start(const CL_Rect &screenRect, CL_GraphicContext *pGC)
{
}

void BattleState::draw_status(const CL_Rect &screenRect, CL_GraphicContext *pGC)
{
    IParty * pParty = IApplication::GetInstance()->GetParty();
    GraphicsManager * pGraphicsManager = GraphicsManager::GetInstance();
    m_pStatusBar->draw(m_nStatusBarX,m_nStatusBarY,pGC);

    for (uint p = 0; p < pParty->GetCharacterCount(); p++)
    {
        ICharacter * pChar = pParty->GetCharacter(p);
        std::ostringstream name;
        name << std::setw(16) << pChar->GetName();
        std::ostringstream hp;
        hp << std::setw(6) << pChar->GetAttribute(ICharacter::CA_HP) << '/'
        << pChar->GetAttribute(ICharacter::CA_MAXHP);
        CL_Font * generalFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, "general");
        CL_Font * hpFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, "hp");
        CL_Font * mpFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, "mp");

        generalFont->draw(m_status_rect.left,m_status_rect.top + (p *
                          generalFont->get_height()), name.str());
        hpFont->draw(m_status_rect.get_width() / 3 + m_status_rect.left,
                     m_status_rect.top + (p* hpFont->get_height()),hp.str());
        std::ostringstream mp;
        mp << std::setw(6) << pChar->GetAttribute(ICharacter::CA_MP) << '/'
        << pChar->GetAttribute(ICharacter::CA_MAXMP);
        mpFont->draw((m_status_rect.get_width() / 3) * 2 + m_status_rect.left,
                     m_status_rect.top + (p*mpFont->get_height()),mp.str());

    }
}

void BattleState::draw_battle(const CL_Rect &screenRect, CL_GraphicContext *pGC)
{
    m_pBackdrop->draw(screenRect,pGC);

    CL_Rect monsterRect = screenRect;

    // Hack to get a monster size
    monsterRect.right = screenRect.get_width() * 0.66; // TODO: System variable here (not multiplier, actual size of rect)

    // Hack. Player Rect needs to come from game config
    CL_Rect playerRect = screenRect;
    playerRect.top = screenRect.bottom * 0.33;
    playerRect.left = monsterRect.right;
    draw_monsters(monsterRect,pGC);
    draw_players(playerRect,pGC);
    draw_status(screenRect,pGC);

    if (m_combat_state == BATTLE_MENU)
    {
        CL_Rect menu_rect = screenRect;
        // TODO: These SHOULD come from the graphics manager,
        // the values are data driven in resources.xml
        menu_rect.top = screenRect.bottom * 0.5;
        menu_rect.left = screenRect.right * 0.1;
        draw_menus(menu_rect,pGC);
    }

}

void BattleState::draw_monsters(const CL_Rect &monsterRect, CL_GraphicContext *pGC)
{
    uint cellWidth = monsterRect.get_width() / m_nColumns;
    uint cellHeight = monsterRect.get_height() / m_nRows;

    for (int i = 0; i < m_monsters->GetCharacterCount(); i++)
    {
        Monster *pMonster = dynamic_cast<Monster*>(m_monsters->GetCharacter(i));
        int drawX = pMonster->GetCellX() * cellWidth;
        int drawY = pMonster->GetCellY() * cellHeight;

        CL_Sprite * pSprite = pMonster->GetCurrentSprite();

        pSprite->draw(drawX,drawY,pGC);
        pSprite->update();

        if (m_combat_state == TARGETING)
        {
            if ((m_targets.m_bSelectedGroup && m_targets.selected.m_pGroup == m_monsters)
                    || m_targets.selected.m_pTarget == pMonster)
            {
                m_target_sprite->draw(drawX -   (m_target_sprite->get_width()/2),
                                      drawY - (m_target_sprite->get_height()/2), pGC);
            }
        }
    }
}

void BattleState::draw_players(const CL_Rect &playerRect, CL_GraphicContext *pGC)
{
    IParty * pParty = IApplication::GetInstance()->GetParty();

    uint playercount = pParty->GetCharacterCount();
    for (uint nPlayer = 0; nPlayer < playercount; nPlayer++)
    {
        Character * pCharacter = dynamic_cast<Character*>(pParty->GetCharacter(nPlayer));
        CL_Sprite * pSprite = pCharacter->GetCurrentSprite();

        // Need to get the spacing from game config
        pSprite->draw(playerRect.left + nPlayer * 64, playerRect.top + (nPlayer * 64), pGC);
        pSprite->update();

        if (m_combat_state == TARGETING)
        {
            if ((m_targets.m_bSelectedGroup && m_targets.selected.m_pGroup == pParty)
                    || m_targets.selected.m_pTarget == pCharacter)
            {
                m_target_sprite->draw(playerRect.left + nPlayer * 64 -   (m_target_sprite->get_width()/2),
                                      playerRect.top + (nPlayer*64) - (m_target_sprite->get_height()/2), pGC);
            }
        }
    }
}

void BattleState::draw_targets(const CL_Rect &screenrect, CL_GraphicContext *pGC)
{
}

void BattleState::draw_menus(const CL_Rect &screenrect, CL_GraphicContext *pGC)
{
    BattleMenu * pMenu = m_menu_stack.top();
    GraphicsManager * pGraphicsManager = GraphicsManager::GetInstance();
    CL_Font * onFont;
    CL_Font * offFont;
    CL_Font * selectedFont;

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
        m_pBattlePopup->draw(screenrect.left,screenrect.top,pGC);
        std::list<BattleMenuOption*>::iterator iter;
        uint pos = 0;

        for (iter = pMenu->GetOptionsBegin();iter != pMenu->GetOptionsEnd();iter++)
        {
            BattleMenuOption * pOption = *iter;
            CL_Font * font;
            if (pOption->Enabled(params))
            {
                font = onFont;
            }
            else
            {
                font = offFont;
            }

            if (m_menu_stack.top()->GetSelectedOption() == iter)
                font = selectedFont;

            // TODO: get font box from resources
            font->draw(screenrect.left + 20 , 20 + screenrect.top + font->get_height() * pos,pOption->GetName(),pGC);

            ++pos;
        }
    }
    else
    {
        m_pBattleMenu->draw(screenrect.left,screenrect.top,pGC);
    }
}
void BattleState::SteelInit(SteelInterpreter* pInterpreter)
{
    static SteelFunctor3Arg<BattleState,bool,bool,bool> fn_selectTargets(this,&BattleState::selectTargets);

    pInterpreter->addFunction("selectTargets",&fn_selectTargets);
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
}

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

void BattleState::SelectLeftGroup()
{
    m_targets.m_bSelectedGroup = true;
    // TODO: Don't assume monsters are left, players are right...
    m_targets.selected.m_pGroup = m_monsters;
}

void BattleState::SelectRightGroup()
{
    m_targets.m_bSelectedGroup = true;
    m_targets.selected.m_pGroup = IApplication::GetInstance()->GetParty();
}


void BattleState::SelectFromLeftGroup()
{
    m_targets.selected.m_pTarget = m_monsters->GetCharacter(0);
    m_targets.m_bSelectedGroup  = false;
}
void BattleState::SelectFromRightGroup()
{
    m_targets.selected.m_pTarget = m_initiative[m_cur_char];
    m_targets.m_bSelectedGroup  = false;
}

	bool BattleState::MonstersOnLeft()
	{
	    return true;
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

            for (int i=0;i<m_targets.selected.m_pGroup->GetCharacterCount();i++)
            {
                SteelType ref;
                ref.set(m_targets.selected.m_pGroup->GetCharacter(i));
                targets.push_back(ref);
            }

    }
    else
    {
        SteelType ref;
        ref.set(m_targets.selected.m_pTarget);
        targets.push_back(ref);
    }

    SteelType val;
    val.set(targets);
    return val;
}
