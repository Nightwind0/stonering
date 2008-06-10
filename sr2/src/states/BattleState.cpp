#include "BattleState.h"
#include "MonsterRef.h"
#include "CharacterManager.h"
#include "GraphicsManager.h"
#include "IApplication.h"
#include "MonsterGroup.h"
#include "Monster.h"
#include "IParty.h"

using namespace StoneRing;


void BattleState::init(const MonsterGroup &group, const std::string &backdrop)
{
    CharacterManager * pCharManager = IApplication::GetInstance()->GetCharacterManager();
    GraphicsManager * pGraphicsManager = GraphicsManager::GetInstance();

    m_nRows = group.GetCellRows();
    m_nColumns = group.GetCellColumns();

    uint bottomrow = m_nRows - 1;

    const std::vector<MonsterRef*> & monsters = group.GetMonsters();

    for(std::vector<MonsterRef*>::const_iterator it = monsters.begin();
        it != monsters.end(); it++)
    {
        MonsterRef *pRef= *it;
        uint count = pRef->GetCount();
   
        for(int x=0;x<pRef->GetColumns();x++)
        {
            for(int y=0;y<pRef->GetRows();y++)
            {
                if(count>0)
                {
                    Monster * pMonster = pCharManager->CreateMonster(pRef->GetName());
                    pMonster->SetCellX( pRef->GetCellX() + x );
                    pMonster->SetCellY( bottomrow - pRef->GetCellY() - y);
                  
                    pMonster->SetCurrentSprite(pGraphicsManager->CreateMonsterSprite(pMonster->GetName(),
                        "idle")); // fuck, dude

                    m_monsters.push_back(pMonster);
                }

                if(--count == 0) break;
            }
        }

        if(count > 0) throw CL_Error("Couldn't fit all monsters in their rows and columns");
    }
    m_pBackdrop = pGraphicsManager->GetBackdrop(backdrop);
    m_bDone = false;
}

bool BattleState::IsDone() const
{
    return m_bDone;
}

void BattleState::HandleKeyDown(const CL_InputEvent &key)
{
}

void BattleState::HandleKeyUp(const CL_InputEvent &key)
{
    switch(key.id)
    {
    case CL_KEY_ESCAPE:
        m_bDone = true;
        break;
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

void BattleState::Start()
{
    m_draw_method = &BattleState::draw_battle;
    m_eState = BATTLE;
    init_or_release_players(false);
}

void BattleState::init_or_release_players(bool bRelease)
{
	GraphicsManager * pGraphicsManager = GraphicsManager::GetInstance();
    IParty * pParty = IApplication::GetInstance()->GetParty();

    uint count = pParty->GetCharacterCount();
    for(uint nPlayer = 0;nPlayer < count; nPlayer++)
    {
        Character * pCharacter = dynamic_cast<Character*>(pParty->GetCharacter(nPlayer));
        assert(pCharacter);
        std::string name = pCharacter->GetName();
        if(!bRelease)
            pCharacter->SetCurrentSprite(pGraphicsManager->CreateCharacterSprite(name,"idle")); // bullshit 
        else delete pCharacter->GetCurrentSprite();
    }
}


void BattleState::Finish()
{
    for(std::vector<Monster*>::iterator it = m_monsters.begin();
        it != m_monsters.end(); it++)
        delete *it;

    m_monsters.clear();

    init_or_release_players(true);
}


void BattleState::draw_transition_in(const CL_Rect &screenRect, CL_GraphicContext *pGC)
{
}

void BattleState::draw_start(const CL_Rect &screenRect, CL_GraphicContext *pGC)
{
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
}

void BattleState::draw_monsters(const CL_Rect &monsterRect, CL_GraphicContext *pGC)
{
    uint cellWidth = monsterRect.get_width() / m_nColumns;
    uint cellHeight = monsterRect.get_height() / m_nRows;

    for(std::vector<Monster*>::iterator it = m_monsters.begin();
        it != m_monsters.end(); it++)
    {
        Monster *pMonster = *it;
        int drawX = pMonster->GetCellX() * cellWidth;
        int drawY = pMonster->GetCellY() * cellHeight;

        CL_Sprite * pSprite = pMonster->GetCurrentSprite();

        pSprite->draw(drawX,drawY,pGC);
        pSprite->update();
    }
}

void BattleState::draw_players(const CL_Rect &playerRect, CL_GraphicContext *pGC)
{
	IParty * pParty = IApplication::GetInstance()->GetParty();

    uint playercount = pParty->GetCharacterCount();
    for(uint nPlayer = 0; nPlayer < playercount; nPlayer++)
    {
        Character * pCharacter = dynamic_cast<Character*>(pParty->GetCharacter(nPlayer));
        CL_Sprite * pSprite = pCharacter->GetCurrentSprite();

         // Need to get the spacing from game config
        pSprite->draw(playerRect.left + nPlayer * 64, playerRect.top + (nPlayer * 64), pGC);
        pSprite->update();
    }
}