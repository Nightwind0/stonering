#include "MapState.h"
#include "Level.h"
#include "MappableObject.h"
#include "Party.h"
#include "SoundManager.h"


using std::min;
using StoneRing::Party;


StoneRing::MapState::MapState():m_LevelX(0),
                                m_LevelY(0),m_player(0,0)
{
    m_bShowDebug = false;
    m_horizontal_idle = true;
    m_vertical_idle = true;
    m_pLevel = NULL;
	m_lerpLevelX.SetStaticValue(0);
	m_lerpLevelY.SetStaticValue(0);
	m_lerpLevelX.SetTime(0.16f);
	m_lerpLevelY.SetTime(0.16f);
}

StoneRing::MapState::~MapState()
{
}

void StoneRing::MapState::SetDimensions(const clan::Rect &screenRect)
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

void StoneRing::MapState::BringDown()
{
	m_bDone = true;
}


void StoneRing::MapState::HandleButtonUp(const IApplication::Button& button)
{
    if(!m_pLevel) return;
    
    MappablePlayer *pPlayer = &m_player;
    switch(button)
    {
	case IApplication::BUTTON_CANCEL:
	    pPlayer->GetNavigator().SetRunning(false);
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
     if(!m_pLevel) return;
     MappablePlayer *pPlayer = &m_player;
     
     switch(button)
     {
	 case IApplication::BUTTON_CANCEL:
             if(m_pLevel->AllowsRunning())
                 pPlayer->GetNavigator().SetRunning(true);
	     break;
     }
}

void StoneRing::MapState::HandleAxisMove(const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos)
{
    if(!m_pLevel) return;
    MappablePlayer *pPlayer = &m_player;
    IApplication* pApp = IApplication::GetInstance();

    if(axis == IApplication::AXIS_HORIZONTAL)
    {
	m_horizontal_idle = false;
	if(dir == IApplication::AXIS_RIGHT)
	{
        pPlayer->GetNavigator().SetNextDirection(Direction::EAST);
	}
	else if(dir == IApplication::AXIS_LEFT)
	{
	    pPlayer->GetNavigator().SetNextDirection(Direction::WEST);
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
	    pPlayer->GetNavigator().SetNextDirection(Direction::SOUTH);
	}
	else if(dir == IApplication::AXIS_UP)
	{
	    pPlayer->GetNavigator().SetNextDirection(Direction::NORTH);
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

void StoneRing::MapState::HandleKeyDown(const clan::InputEvent &key)
{
    if(!m_pLevel) return;
    MappablePlayer *pPlayer = &m_player;
    assert(pPlayer);
    if(key.shift && m_pLevel->AllowsRunning())
       pPlayer->GetNavigator().SetRunning(true);

    switch(key.id)
    {
#if 0
    case clan::KEY_ESCAPE:
        m_bDone = true;
        break;

    case clan::KEY_DOWN:
        pPlayer->SetNextDirection(StoneRing::MappableObject::SOUTH);
        break;
    case clan::KEY_UP:
        pPlayer->SetNextDirection(StoneRing::MappableObject::NORTH);
        break;
    case clan::KEY_LEFT:
        pPlayer->SetNextDirection(StoneRing::MappableObject::WEST);
        break;
    case clan::KEY_RIGHT:
        pPlayer->SetNextDirection(StoneRing::MappableObject::EAST);
        break;
#endif
    default:
        break;
    }

}

void StoneRing::MapState::HandleKeyUp(const clan::InputEvent &key)
{
    if(!m_pLevel) return;
    MappablePlayer *pPlayer = &m_player;
    assert(pPlayer);
    switch(key.id)
    {
	/*
    case clan::KEY_SPACE:
        do_talk();
        break;
    case clan::KEY_TAB:
        do_talk(true); // Prod!
        break;
	*/
    case clan::keycode_shift:
        pPlayer->GetNavigator().SetRunning(false);
        break;
#ifndef NDEBUG
    case clan::keycode_p:
        std::cout << "Player location:" << pPlayer->GetPosition().x << ',' << pPlayer->GetPosition().y << std::endl;
        break;
    case clan::keycode_x:
        break;
	case clan::keycode_f:{
        static bool frozen = false;
        if(frozen)
            m_pLevel->UnfreezeMappableObjects();
        else
            m_pLevel->FreezeMappableObjects();
        frozen = !frozen;
        break;
				  }
    case clan::keycode_d:
        m_bShowDebug = !m_bShowDebug;
        if(m_bShowDebug){
            m_pLevel->AddTileVisitor(&m_block_drawer);
            m_pLevel->AddTileVisitor(&m_hot_drawer);
            m_pLevel->AddTileVisitor(&m_floater_drawer);
        }else{
            m_pLevel->RemoveTileVisitor(&m_block_drawer);
            m_pLevel->RemoveTileVisitor(&m_hot_drawer);
            m_pLevel->RemoveTileVisitor(&m_floater_drawer);
        }
        break;
    case clan::keycode_m:
         m_pLevel->DumpMappableObjects();
        break;
    case clan::keycode_s:
        gbDebugStop = true;
        break;
#endif

    }

}

void StoneRing::MapState::Covered() {
	m_vertical_idle = m_horizontal_idle = true;
	m_player.StopMovement();
}


void StoneRing::MapState::Draw(const clan::Rect &screenRect,clan::Canvas& GC)
{
    if(!m_pLevel) return;
    bool clearBg = false;
    uint width = min( (unsigned int)screenRect.get_width(), m_pLevel->GetWidth() * 32);
    uint height = min((unsigned int)screenRect.get_height(), m_pLevel->GetHeight() * 32);

    clan::Rect src = clan::Rect(m_lerpLevelX.GetValue(), m_lerpLevelY.GetValue(), m_lerpLevelX.GetValue() +width,
                          m_lerpLevelY.GetValue() + height);
    clan::Rect dst = screenRect;
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

    m_pLevel->Draw(src,dst, GC);
#ifndef NDEBUG
	if(m_bShowDebug)
		m_pLevel->DrawMOQuadtree(GC,dst.get_top_left()-src.get_top_left());
#endif
}


bool StoneRing::MapState::DisableMappableObjects() const // Should the app move the MOs?
{
    return false;
}

bool StoneRing::MapState::DrawMappableObjects() const // Should the app draw the MOs, including the player
{
    return true;
}

void StoneRing::MapState::Update() // Do stuff right after the mappable object movemen
{
    recalculate_player_position();
}

void StoneRing::MapState::Start()
{
	m_bDone = false;
}

void StoneRing::MapState::Finish() // Hook to clean up or whatever after being poppe
{
}

StoneRing::Level* StoneRing::MapState::GetCurrentLevel() const 
{
    return m_pLevel;
}

clan::Point StoneRing::MapState::GetCurrentCenter() const 
{
	uint width = min( (unsigned int)m_screen_rect.get_width(), m_pLevel->GetWidth() * 32);
    uint height = min((unsigned int)m_screen_rect.get_height(), m_pLevel->GetHeight() * 32);
    clan::Rect rect = clan::Rect(m_lerpLevelX.GetValue(), m_lerpLevelY.GetValue(),
                                         (m_lerpLevelX.GetValue()+ width),
                                         (m_lerpLevelY.GetValue() + height));
	return rect.get_center();
}



void StoneRing::MapState::PushLevel(Level * pLevel, uint x, uint y)
{
	if(m_pLevel){
		m_pLevel->RemoveMappableObject(&m_player);	
		m_levels.push_back(m_pLevel);
		m_position_stack.push_back(m_player.GetPosition());			
	}
    clan::ResourceManager& resources = IApplication::GetInstance()->GetResources();
    Character *pMapCharacter = IApplication::GetInstance()->GetParty()->GetMapCharacter();

    m_pLevel = pLevel;

    if(pMapCharacter)
    {
		// TODO: Move this line
        m_player.SetSprite ( pMapCharacter->GetMapSprite() );
        m_LevelX = 0;
        m_LevelY = 0;
		m_lerpLevelX.SetStaticValue(0);
		m_lerpLevelY.SetStaticValue(0);
        m_player.ResetLevelX(x);
        m_player.ResetLevelY(y);		
		m_player.Placed();
        pLevel->AddMappableObject(&m_player);
        recalculate_player_position(false);        
    }
    
    SoundManager::SetMusic(m_pLevel->GetMusic());
}

void StoneRing::MapState::LoadLevel(Level * pLevel, uint x, uint y)
{
	if(m_pLevel){
		m_pLevel->RemoveMappableObject(&m_player);
		m_pLevel->MarkForDeath();
	}
    clan::ResourceManager& resources = IApplication::GetInstance()->GetResources();
    Character *pMapCharacter = IApplication::GetInstance()->GetParty()->GetMapCharacter();
	if(pMapCharacter){
		m_player.SetSprite(pMapCharacter->GetMapSprite());
	}
	m_pLevel = pLevel;
	m_player.ResetLevelX(x);
	m_player.ResetLevelY(y);
	m_lerpLevelX.SetStaticValue(0);
	m_lerpLevelY.SetStaticValue(0);	
	m_player.Placed();
	m_pLevel->AddMappableObject(&m_player);
	m_LevelX = m_LevelY = 0;
	recalculate_player_position(false);
    SoundManager::SetMusic(m_pLevel->GetMusic());	
}


void StoneRing::MapState::SetPlayerSprite(clan::Sprite  player)
{
    m_playerSprite = player;
}

void StoneRing::MapState::Pop(bool bAll)
{
	clan::Point point;
    if(bAll)
    {
		m_pLevel->MarkForDeath();
        while(!m_levels.empty())
        {
            m_levels.back()->MarkForDeath();
			m_pLevel = m_levels.back();
			//m_levels.back()->RemoveMappableObject(&m_player);
            m_levels.pop_back();
			point = m_position_stack.back();
			m_position_stack.pop_back();
        }
    }
    else
    {
		m_pLevel->MarkForDeath();
        if(!m_levels.empty())
        {
			m_pLevel = m_levels.back();
            //m_levels.back()->MarkForDeath();
			//m_levels.back()->RemoveMappableObject(&m_player);
            m_levels.pop_back();
			point = m_position_stack.back();
			m_position_stack.pop_back();
        }else{
			return;
		}
    }
	//m_pLevel->RemoveMappableObject(&m_player);
	m_player.ResetLevelX(point.x);
	m_player.ResetLevelY(point.y);	
	m_player.Placed();
	m_pLevel->AddMappableObject(&m_player);
	recalculate_player_position(false);
    SoundManager::SetMusic(m_pLevel->GetMusic());
}



void StoneRing::MapState::recalculate_player_position(bool lerp)
{
    if(!m_pLevel) return;
    MappablePlayer *pPlayer = &m_player;
    assert(pPlayer);
    clan::Rect spriteRect = pPlayer->GetSpriteRect();
    int X = pPlayer->GetLevelX();
    int Y = pPlayer->GetLevelY();
	const int origx = lerp?m_lerpLevelX.GetValue():m_LevelX;
	const int origy = lerp?m_lerpLevelY.GetValue():m_LevelY;

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

    if(lerp){
		m_lerpLevelX.SetRange(origx,m_LevelX);
		m_lerpLevelY.SetRange(origy,m_LevelY);
	}else{
		m_lerpLevelX.SetStaticValue(m_LevelX);
		m_lerpLevelY.SetStaticValue(m_LevelY);		
	}
}

void StoneRing::MapState::MoveMappableObjects()
{
    if(!m_pLevel) return;
    static uint ticks = 0;
    clan::Rect rect = clan::Rect(m_lerpLevelX.GetValue()/32, m_lerpLevelY.GetValue()/32,
                                         (m_lerpLevelX.GetValue() + m_screen_rect.get_width()/32),
                                         (m_lerpLevelY.GetValue() + m_screen_rect.get_height())/32);
    m_pLevel->MoveMappableObjects(rect);
    
    if(ticks++ % 4 == 0)
        m_pLevel->Update(rect);

}

void StoneRing::MapState::do_talk(bool prod)
{
    MappablePlayer *pPlayer = &m_player;
    assert(pPlayer);
    clan::Point talkPoint = pPlayer->GetPointInFront();

 
    m_pLevel->Talk ( talkPoint, prod );
}

#if 0 
void StoneRing::MapState::switch_from_player(MappablePlayer * pPlayer)
{
    // TODO: This
}
#endif

void StoneRing::MapState::SerializeState ( std::ostream& out )
{
	m_player.SerializeState(out);
	WriteString(out,m_pLevel->GetResourceName());
	m_pLevel->SerializeState(out);
    uint level_count = m_levels.size();
    out.write((char*)&level_count,sizeof(uint));
    for(int i=0;i<level_count;i++){
        WriteString(out, m_levels[i]->GetResourceName());
        m_levels[i]->SerializeState(out);
	}
	uint pos_size = m_position_stack.size();
	out.write((char*)&pos_size,sizeof(uint));
	for(int i=0;i<m_position_stack.size();i++){
		clan::Point pt = m_position_stack[i];
		out.write((char*)&pt.x,sizeof(pt.x));
		out.write((char*)&pt.y,sizeof(pt.y));
	}
}

void StoneRing::MapState::DeserializeState ( std::istream& in )
{
	m_player.DeserializeState(in);
	std::string name = ReadString(in);
	m_pLevel = new Level();
	m_pLevel->Load(name,IApplication::GetInstance()->GetResources());
	m_pLevel->Invoke();
	m_pLevel->DeserializeState(in);
    uint level_count;
    in.read((char*)&level_count,sizeof(uint));
    for(int i=0;i<level_count;i++){
        std::string name = ReadString(in);
        Level * pLevel = new Level();
        pLevel->Load(name,IApplication::GetInstance()->GetResources());
        pLevel->Invoke();
        pLevel->DeserializeState(in);
        m_levels.push_back(pLevel);
    }
    uint pos_count;
	in.read((char*)&pos_count,sizeof(uint));
	for(int i=0;i<pos_count;i++){
		clan::Point pt;
		in.read((char*)&pt.x,sizeof(pt.x));
		in.read((char*)&pt.y,sizeof(pt.y));
		m_position_stack.push_back(pt);
	}

    Character *pMapCharacter = IApplication::GetInstance()->GetParty()->GetMapCharacter();    
	
	if(pMapCharacter)
    {
		m_player.SetSprite ( pMapCharacter->GetMapSprite() );
		m_pLevel->AddMappableObject(&m_player);
		m_player.Placed();
		recalculate_player_position();
    }        
	
    SoundManager::SetMusic(m_pLevel->GetMusic());
}



