/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "CutSceneState.h"
#include "Level.h"
#include "Navigator.h"

namespace StoneRing {

CutSceneState::CutSceneState():m_pRunner(NULL),m_pLevel(NULL)
{

}

CutSceneState::~CutSceneState()
{

}

bool CutSceneState::DisableMappableObjects() const
{
    return false;
}

void CutSceneState::Draw ( const CL_Rect& screenRect, CL_GraphicContext& GC )
{
    // TODO Center around the center of the middle tile instead of the top-left of it?
    CL_Rect levelRect = CL_Rect((m_center.x*32) - screenRect.get_width()/2,
                                (m_center.y*32) - screenRect.get_height()/2,
                               screenRect.get_width(),
                               screenRect.get_height());
    if(m_pLevel){
        m_pLevel->Draw(levelRect,screenRect,GC);
        if(m_bDrawMOs)
            m_pLevel->DrawMappableObjects(levelRect,screenRect,GC);
    }
    
    CL_Draw::box(GC,screenRect,m_color);
    
    if(m_fade_level != 0.0f){
        CL_Draw::box(GC,screenRect,CL_Colorf(0.0f,0.0f,0.0f,m_fade_level));
    }
}

void CutSceneState::HandleAxisMove ( const StoneRing::IApplication::Axis& axis, const StoneRing::IApplication::AxisDirection dir, float pos )
{
}

void CutSceneState::HandleButtonDown ( const StoneRing::IApplication::Button& button )
{
}

void CutSceneState::HandleButtonUp ( const StoneRing::IApplication::Button& button )
{
}

void CutSceneState::Init ( SteelFunctor* pFunctor )
{
    m_functor = shared_ptr<SteelFunctor>(pFunctor);
}

Level* CutSceneState::grabMapStateLevel()
{
    return IApplication::GetInstance()->GetCurrentLevel();
}



void CutSceneState::Start()
{
    m_bDone = false;
    m_bDrawMOs = true;
    m_bFrozen = false;
    m_pLevel = grabMapStateLevel();
    m_center = m_pLevel->GetPlayer()->GetPosition();    
    // TODO: Need to get X,Y from MapState too..
    uncolorize();
    // Run script in Runner
}

void CutSceneState::Finish()
{

}

void CutSceneState::SteelInit ( SteelInterpreter* pInterpreter )
{
    m_pRunner = new SteelRunner(pInterpreter);
    // Add BIFs
}

void CutSceneState::SteelCleanup ( SteelInterpreter* )
{
}

bool CutSceneState::IsDone() const
{
    return m_bDone;
}

bool CutSceneState::LastToDraw() const
{
    return false;
}

void CutSceneState::MappableObjectMoveHook()
{

}

void CutSceneState::MoveCharacterTo ( MappableObject* pMO, int x, int y, int speed )
{
    // Create navigator and assign it to the MO
    ScriptedNavigator * pNavigator = new ScriptedNavigator(CL_Point(x,y),speed);
    pMO->PushNavigator(pNavigator);
    // then we just go until the navigator says it's reached its destination
    
}

void CutSceneState::PanTo ( int x, int y )
{
    m_center = CL_Point(x,y);
}

CL_Point CutSceneState::GetOrigin() const
{
    return m_center;
}

void CutSceneState::SetFadeLevel ( float level )
{
    m_fade_level = level;
}

void CutSceneState::verifyLevel()
{
    if(m_pLevel == NULL)
        throw CL_Exception("gotoLevel must be called first.");
}

SteelType CutSceneState::addCharacter ( const std::string& spriteRef, int x, int y, int face_dir )
{
    verifyLevel();
}

SteelType CutSceneState::colorize ( float r, float g, float b )
{
    m_color = CL_Colorf(r,g,b,0.75f);
}

SteelType CutSceneState::uncolorize()
{
    m_color = CL_Colorf(1.0f,1.0f,1.0f,1.0f);
}


SteelType CutSceneState::faceCharacter ( SteelType::Handle hHandle, int dir )
{
    verifyLevel();
}

SteelType CutSceneState::fadeIn ( float seconds )
{

}

SteelType CutSceneState::fadeOut ( float seconds )
{

}

SteelType CutSceneState::freezeCharacters()
{
    verifyLevel();
    m_pLevel->FreezeMappableObjects();
    m_bFrozen = true;
}

SteelType CutSceneState::unfreezeCharacters()
{
    verifyLevel();
    if(m_bFrozen)
        m_pLevel->UnfreezeMappableObjects();
    m_bFrozen = false;
}

SteelType CutSceneState::hideCharacter ( SteelType::Handle hHandle )
{
    verifyLevel();
}

SteelType CutSceneState::unhideCharacter ( SteelType::Handle hHandle )
{
    verifyLevel();
}

SteelType CutSceneState::moveCharacter ( SteelType::Handle hHandle, int x, int y, int speed )
{
    verifyLevel();
}

SteelType CutSceneState::panTo ( int x, int y, float seconds )
{

}

SteelType CutSceneState::gotoLevel ( const std::string& level, int x, int y )
{
    // TODO Maybe do a stack?
   if(m_pLevel)
       delete m_pLevel;
   m_pLevel = new Level();
   m_pLevel->Load ( level, IApplication::GetInstance()->GetResources() );
   m_pLevel->Invoke();
   
   return SteelType();
}

SteelType CutSceneState::getCharacter ( const  std::string& name )
{
    verifyLevel();
    SteelType val;
    val.set( m_pLevel->GetMappableObjectByName(name) );
    return val;
}

SteelType CutSceneState::getPlayer()
{
    verifyLevel();
}





// For the BIFs, instead of pushing a new state.... 
// can I save some data here, sort of like how the animation state works,
// with a queue of task objects.....
// or possibly ONE task object and somehow stop the script and don't start it
// again until the task is finished
// but how to stop the script? if the script is running in a thread,
// it could stop on a mutex, and the state unlocks the mutex when it's done
// processing a queue item
// TODO: If this works, I could get rid of some substates


/*  Fade */
void CutSceneState::FadeTask::start()
{

}

void CutSceneState::FadeTask::update()
{
    
}

bool CutSceneState::FadeTask::finished()
{

}

/*  Move */
void CutSceneState::MoveTask::start()
{

}

void CutSceneState::MoveTask::update()
{
}

bool CutSceneState::MoveTask::finished()
{
}


/* Pan */
void CutSceneState::PanTask::start()
{
}

void CutSceneState::PanTask::update()
{
}

bool CutSceneState::PanTask::finished()
{
}

}