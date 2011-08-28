#include "MapState.h"
#include "Level.h"
#include "MappableObject.h"
#include "IParty.h"

using std::min;
using StoneRing::IParty;


StoneRing::MapState::MapState():m_bDone(false),m_LevelX(0),
                                m_LevelY(0)
{
#ifndef NDEBUG
    m_bShowDebug = false;
#endif
    m_horizontal_idle = true;
    m_vertical_idle = true;
}

StoneRing::MapState::~MapState()
{
}

void StoneRing::MapState::SetDimensions(const CL_Rect &screenRect)
{
    m_screen_rect = screenRect;
}

bool StoneRing::MapState::IsDone() const
{
    return m_bDone;
}

void StoneRing::MapState::RegisterSteelFunctions(SteelInterpreter* pInterpreter)
{

}

void StoneRing::MapState::HandleButtonUp(const IApplication::Button& button)
{
    MappablePlayer *pPlayer = m_pLevel->GetPlayer();
    switch(button)
    {
	case IApplication::BUTTON_CANCEL:
	    pPlayer->SetRunning(false);
	    break;
	case IApplication::BUTTON_CONFIRM:
	    do_talk();
	    break;
	case IApplication::BUTTON_ALT:
	    do_talk(true);
	    break;
	case IApplication::BUTTON_MENU:
	    IApplication::GetInstance()->MainMenu();
	    break;
    }
}

void StoneRing::MapState::HandleButtonDown(const IApplication::Button& button)
{
     MappablePlayer *pPlayer = m_pLevel->GetPlayer();
     
     switch(button)
     {
	 case IApplication::BUTTON_CANCEL:
	     pPlayer->SetRunning(true);
	     break;
     }
}

void StoneRing::MapState::HandleAxisMove(const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos)
{
    MappablePlayer *pPlayer = m_pLevel->GetPlayer();
    IApplication* pApp = IApplication::GetInstance();

    if(axis == IApplication::AXIS_HORIZONTAL)
    {
	m_horizontal_idle = false;
	if(dir == IApplication::AXIS_RIGHT)
	{
	    pPlayer->SetNextDirection(StoneRing::MappableObject::EAST);
	}
	else if(dir == IApplication::AXIS_LEFT)
	{
	    pPlayer->SetNextDirection(StoneRing::MappableObject::WEST);
	}
	else // neutral
	{
	    m_horizontal_idle = true;
	}
    }
    else
    {
	m_vertical_idle = false;
	if(dir == IApplication::AXIS_DOWN)
	{
	    pPlayer->SetNextDirection(StoneRing::MappableObject::SOUTH);
	}
	else if(dir == IApplication::AXIS_UP)
	{
	    pPlayer->SetNextDirection(StoneRing::MappableObject::NORTH);
	}
	else // neutral
	{
	    m_vertical_idle = true;
	}
    }
    
    if(m_horizontal_idle && m_vertical_idle)
    {
	pPlayer->StopMovement();
    }
}

void StoneRing::MapState::HandleKeyDown(const CL_InputEvent &key)
{
    MappablePlayer *pPlayer = m_pLevel->GetPlayer();
    assert(pPlayer);
    if(key.shift && m_pLevel->AllowsRunning())
        pPlayer->SetRunning(true);

    switch(key.id)
    {
    case CL_KEY_ESCAPE:
        m_bDone = true;
        break;
#if 0
    case CL_KEY_DOWN:
        pPlayer->SetNextDirection(StoneRing::MappableObject::SOUTH);
        break;
    case CL_KEY_UP:
        pPlayer->SetNextDirection(StoneRing::MappableObject::NORTH);
        break;
    case CL_KEY_LEFT:
        pPlayer->SetNextDirection(StoneRing::MappableObject::WEST);
        break;
    case CL_KEY_RIGHT:
        pPlayer->SetNextDirection(StoneRing::MappableObject::EAST);
        break;
#endif
    default:
        break;
    }

}

void StoneRing::MapState::HandleKeyUp(const CL_InputEvent &key)
{
    MappablePlayer *pPlayer = m_pLevel->GetPlayer();
    assert(pPlayer);
    switch(key.id)
    {
	/*
    case CL_KEY_SPACE:
        do_talk();
        break;
    case CL_KEY_TAB:
        do_talk(true); // Prod!
        break;
	*/
    case CL_KEY_SHIFT:
        pPlayer->SetRunning(false);
        break;
#ifndef NDEBUG
    case CL_KEY_P:
        std::cout << "Player location:" << pPlayer->GetPosition().x << ',' << pPlayer->GetPosition().y << std::endl;
        break;

    case CL_KEY_D:
        m_bShowDebug = m_bShowDebug?false:true;
        break;
    case CL_KEY_M:
         m_pLevel->DumpMappableObjects();
        break;
    case CL_KEY_S:
        gbDebugStop = true;
        break;
#endif

    }

}

void StoneRing::MapState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC)
{
    bool clearBg = false;
    uint width = min( (unsigned int)screenRect.get_width(), m_pLevel->GetWidth() * 32);
    uint height = min((unsigned int)screenRect.get_height(), m_pLevel->GetHeight() * 32);

    CL_Rect src = CL_Rect(m_LevelX, m_LevelY, m_LevelX +width,
                          m_LevelY + height);
    CL_Rect dst = screenRect;
    // Center
    if(screenRect.get_width() > src.get_width())
    {
        uint amount = (screenRect.get_width() - src.get_width()) /2;
        dst.left += amount;
        dst.right -= amount;
        clearBg = true;
    }

    if(screenRect.get_height() > src.get_height())
    {
        uint amount = (screenRect.get_height() - src.get_height()) /2;
        dst.top += amount;
        dst.bottom -= amount;
        clearBg = true;
    }

    if(clearBg)
        GC.clear();

    m_pLevel->Draw(src,dst, GC, false,m_bShowDebug,m_bShowDebug);
    m_pLevel->DrawMappableObjects(src,dst,GC);
    m_pLevel->DrawFloaters(src,dst,GC);
}


bool StoneRing::MapState::DisableMappableObjects() const // Should the app move the MOs?
{
    return false;
}

bool StoneRing::MapState::DrawMappableObjects() const // Should the app draw the MOs, including the player
{
    return true;
}

void StoneRing::MapState::MappableObjectMoveHook() // Do stuff right after the mappable object movemen
{
    recalculate_player_position();
}

void StoneRing::MapState::Start()
{
}

void StoneRing::MapState::Finish() // Hook to clean up or whatever after being poppe
{
}


void StoneRing::MapState::PushLevel(Level * pLevel, uint x, uint y)
{
    MappablePlayer *pPlayer = pLevel->GetPlayer();
    assert(pPlayer);
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    Character *pMapCharacter = IApplication::GetInstance()->GetParty()->GetMapCharacter();

    m_levels.push_back( pLevel );
    m_pLevel = m_levels.back();

    if(pMapCharacter)
    {
        pPlayer->SetSprite ( pMapCharacter->GetMapSprite() );
        pPlayer->ResetLevelX(x);
        pPlayer->ResetLevelY(y);

        pLevel->SetPlayerPos(CL_Point(x,y));
        recalculate_player_position();
    }
}

void StoneRing::MapState::SetPlayerSprite(CL_Sprite  player)
{
    m_playerSprite = player;
}

void StoneRing::MapState::Pop(bool bAll)
{
    MappablePlayer *pPlayer = m_pLevel->GetPlayer();
    assert(pPlayer);

    MappableObject::eDirection oldDir = pPlayer->GetDirection();

    if(bAll)
    {
        while(m_levels.size() > 1)
        {
            m_levels.back()->MarkForDeath();
            m_levels.pop_back();
        }
    }
    else
    {

        if(m_levels.size())
        {
            m_levels.back()->MarkForDeath();
            m_levels.pop_back();
        }
    }

    m_pLevel = m_levels.back();
    MappablePlayer * pNewPlayer = m_pLevel->GetPlayer();

    pNewPlayer->SetNextDirection(oldDir);
}



void StoneRing::MapState::recalculate_player_position()
{
    MappablePlayer *pPlayer = m_pLevel->GetPlayer();
    assert(pPlayer);

    int X = pPlayer->GetLevelX();
    int Y = pPlayer->GetLevelY();

    if( X  > m_LevelX + (m_screen_rect.get_width() / 2) &&
        static_cast<int>(m_pLevel->GetWidth()) * 32 > static_cast<int>(m_screen_rect.get_width()))
    {
        // Try to scroll right
        int amount = X - (m_LevelX  + (m_screen_rect.get_width()/2));

        if(m_LevelX + amount + m_screen_rect.get_width() < m_pLevel->GetWidth() * 32)
        {
            m_LevelX += amount;
        }
        else
        {
            // Scroll as far over as possible
            m_LevelX = (m_pLevel->GetWidth() * 32) - m_screen_rect.get_width();
        }

    }
    if(X  <  m_LevelX + (m_screen_rect.get_width() / 2) && m_pLevel->GetWidth() * 32 > m_screen_rect.get_width())
    {
        int amount = (m_LevelX + (m_screen_rect.get_width()/2)) - X;

        if(m_LevelX - amount >0)
        {
            m_LevelX-= amount;
        }
        else
        {
            m_LevelX = 0;
        }
    }


    if(Y > m_LevelY + (m_screen_rect.get_height()/2) && m_screen_rect.get_height() < m_pLevel->GetHeight() * 32)
    {
        int amount = Y - (m_LevelY + (m_screen_rect.get_height()/2));

        if(m_LevelY + amount + m_screen_rect.get_height() < m_pLevel->GetHeight() * 32)
        {
            m_LevelY+= amount;
        }
        else
        {
            m_LevelY = (m_pLevel->GetHeight() * 32) - m_screen_rect.get_height();
        }
    }

    if(Y  <  m_LevelY + (m_screen_rect.get_height() / 2) && m_screen_rect.get_height() < m_pLevel->GetHeight() * 32)
    {
        int amount = (m_LevelY + (m_screen_rect.get_height()/2)) - Y;

        if(m_LevelY - amount >0)
        {
            m_LevelY-= amount;
        }
        else
        {
            m_LevelY = 0;
        }
    }


}

void StoneRing::MapState::MoveMappableObjects()
{
    m_pLevel->MoveMappableObjects(CL_Rect(m_LevelX, m_LevelY,
                                         m_LevelX + m_screen_rect.get_width(),
                                         m_LevelY + m_screen_rect.get_height()));

}

void StoneRing::MapState::do_talk(bool prod)
{
    MappablePlayer *pPlayer = m_pLevel->GetPlayer();
    assert(pPlayer);
    CL_Point talkPoint = pPlayer->GetPointInFront();

    if(talkPoint.x >=0 && talkPoint.x < m_pLevel->GetWidth() &&
       talkPoint.y >=0 && talkPoint.y < m_pLevel->GetHeight())
        m_pLevel->Talk ( talkPoint, prod );
}

void StoneRing::MapState::switch_from_player(MappablePlayer * pPlayer)
{
    pPlayer->ClearNextDirection();
}

void StoneRing::MapState::SerializeState ( std::ostream& out )
{
    uint level_count = m_levels.size();
    out.write((char*)&level_count,sizeof(uint));
    for(int i=0;i<level_count;i++){
        WriteString(out, m_levels[i]->GetName());
        m_levels[i]->SerializeState(out);
    }
}

void StoneRing::MapState::DeserializeState ( std::istream& in )
{
    uint level_count;
    in.read((char*)&level_count,sizeof(uint));
    for(int i=0;i<level_count;i++){
        std::string name = ReadString(in);
        Level * pLevel = new Level();
        pLevel->Load(name,IApplication::GetInstance()->GetResources());
        pLevel->Invoke();
        pLevel->DeserializeState(in);
        MappablePlayer* pPlayer = pLevel->GetPlayer();
        Character *pMapCharacter = IApplication::GetInstance()->GetParty()->GetMapCharacter();    
        if(pMapCharacter)
        {
            pPlayer->SetSprite ( pMapCharacter->GetMapSprite() );
            pLevel->SetPlayerPos(pPlayer->GetPosition());
            recalculate_player_position();
        }        
        m_levels.push_back(pLevel);
    }

    m_pLevel = m_levels.back();
  
}



