#include "BattleState.h"
#include "MonsterRef.h"
#include "CharacterManager.h"
#include "GraphicsManager.h"
#include "IApplication.h"
#include "MonsterGroup.h"
#include "Monster.h"
#include "IParty.h"

using StoneRing::BattleState;
using StoneRing::MonsterRef;
using StoneRing::MonsterGroup;


void BattleState::init(const MonsterGroup &group, const std::string &backdrop)
{
    CharacterManager * pCharManager = IApplication::getInstance()->getCharacterManager();
    GraphicsManager * pGraphicsManager = GraphicsManager::getInstance();

    mnRows = group.getCellRows();
    mnColumns = group.getCellColumns();

    uint bottomrow = mnRows - 1;

    const std::vector<MonsterRef*> & monsters = group.getMonsters();

    for(std::vector<MonsterRef*>::const_iterator it = monsters.begin();
        it != monsters.end(); it++)
    {
        MonsterRef *pRef= *it;
        uint count = pRef->getCount();
   
        for(int x=0;x<pRef->getColumns();x++)
        {
            for(int y=0;y<pRef->getRows();y++)
            {
                if(count>0)
                {
                    Monster * pMonster = pCharManager->createMonster(pRef->getName());
                    pMonster->setCellX( pRef->getCellX() + x );
                    pMonster->setCellY( bottomrow - pRef->getCellY() - y);
                  
                    pMonster->setCurrentSprite(pGraphicsManager->createMonsterSprite(pMonster->getName(),
                        "idle")); // fuck, dude

                    mMonsters.push_back(pMonster);
                }

                if(--count == 0) break;
            }
        }

        if(count > 0) throw CL_Error("Couldn't fit all monsters in their rows and columns");
    }
    mpBackdrop = pGraphicsManager->getBackdrop(backdrop);
    mbDone = false;
}

bool BattleState::isDone() const
{
    return mbDone;
}

void BattleState::handleKeyDown(const CL_InputEvent &key)
{
}

void BattleState::handleKeyUp(const CL_InputEvent &key)
{
    switch(key.id)
    {
    case CL_KEY_ESCAPE:
        mbDone = true;
        break;
    }
}

void BattleState::draw(const CL_Rect &screenRect,CL_GraphicContext * pGC)
{
    assert(mDrawMethod != NULL);

    (this->*mDrawMethod)(screenRect,pGC);
}

bool BattleState::lastToDraw() const
{
    return false;
}

bool BattleState::disableMappableObjects() const
{
    return true;
}

void BattleState::mappableObjectMoveHook()
{
}

void BattleState::start()
{
    mDrawMethod = &BattleState::drawBattle;
    meState = BATTLE;
    _initOrReleasePlayers(false);
}

void BattleState::_initOrReleasePlayers(bool bRelease)
{
    GraphicsManager * pGraphicsManager = GraphicsManager::getInstance();
    IParty * pParty = IApplication::getInstance()->getParty();

    uint count = pParty->getCharacterCount();
    for(uint nPlayer = 0;nPlayer < count; nPlayer++)
    {
        Character * pCharacter = dynamic_cast<Character*>(pParty->getCharacter(nPlayer));
        assert(pCharacter);
        std::string name = pCharacter->getName();
        if(!bRelease)
            pCharacter->setCurrentSprite(pGraphicsManager->createCharacterSprite(name,"idle")); // bullshit 
        else delete pCharacter->getCurrentSprite();
    }
}


void BattleState::finish()
{
    for(std::vector<Monster*>::iterator it = mMonsters.begin();
        it != mMonsters.end(); it++)
        delete *it;

    mMonsters.clear();

    _initOrReleasePlayers(true);
}


void BattleState::drawTransitionIn(const CL_Rect &screenRect, CL_GraphicContext *pGC)
{
}

void BattleState::drawStart(const CL_Rect &screenRect, CL_GraphicContext *pGC)
{
}

void BattleState::drawBattle(const CL_Rect &screenRect, CL_GraphicContext *pGC)
{
    mpBackdrop->draw(screenRect,pGC);

    CL_Rect monsterRect = screenRect;

    // Hack to get a monster size
    monsterRect.right = screenRect.get_width() * 0.66; // TODO: System variable here (not multiplier, actual size of rect)

    // Hack. Player Rect needs to come from game config
    CL_Rect playerRect = screenRect;
    playerRect.top = screenRect.bottom * 0.5;
    playerRect.left = monsterRect.right;
    _drawMonsters(monsterRect,pGC);
    _drawPlayers(playerRect,pGC);
}

void BattleState::_drawMonsters(const CL_Rect &monsterRect, CL_GraphicContext *pGC)
{
    uint cellWidth = monsterRect.get_width() / mnColumns;
    uint cellHeight = monsterRect.get_height() / mnRows;

    for(std::vector<Monster*>::iterator it = mMonsters.begin();
        it != mMonsters.end(); it++)
    {
        Monster *pMonster = *it;
        int drawX = pMonster->getCellX() * cellWidth;
        int drawY = pMonster->getCellY() * cellHeight;

        CL_Sprite * pSprite = pMonster->getCurrentSprite();

        pSprite->draw(drawX,drawY,pGC);
        pSprite->update();
    }
}

void BattleState::_drawPlayers(const CL_Rect &playerRect, CL_GraphicContext *pGC)
{
    IParty * pParty = IApplication::getInstance()->getParty();

    uint playercount = pParty->getCharacterCount();
    for(uint nPlayer = 0; nPlayer < playercount; nPlayer++)
    {
        Character * pCharacter = dynamic_cast<Character*>(pParty->getCharacter(nPlayer));
        CL_Sprite * pSprite = pCharacter->getCurrentSprite();

         // Need to get the spacing from game config
        pSprite->draw(playerRect.left + nPlayer * 64, playerRect.top + (nPlayer * 64), pGC);
        pSprite->update();
    }
}