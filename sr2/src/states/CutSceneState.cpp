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
#include "SayState.h"

using namespace Steel;

namespace StoneRing {
    
    class CutSceneRunner: public SteelRunner<CutSceneState> {
    public:
        CutSceneRunner(SteelInterpreter* pInterpreter,CutSceneState* pState)
        :SteelRunner<CutSceneState>(pInterpreter,pState,&CutSceneState::Completed){
        }
        virtual ~CutSceneRunner(){
        }
        virtual void run(){
            // TODO: Should I support an AstScript here too, like the normal runner does?
            try {
            if(m_pFunctor){
                
                    m_result = m_pFunctor->Call(m_pInterpreter,SteelType::Container());
            }
    #if 0
                    std::cerr << "Cut scene functor finished. Waiting for tasks to finish." << std::endl;
    #endif
                
                // wait for all tasks to finish
                while(m_callee->HasTasks()){
                    m_callee->WaitTaskEvent();
                }
            }catch(SteelException ex){
               std::cerr << "Exception in cut scene: " << ex.getMessage() << " on line " << ex.getLine() << std::endl;
            }
            
            (m_callee->*m_callback)();
        }
        
    };

CutSceneState::CutSceneState():m_pRunner(NULL),m_pLevel(NULL)
{
    m_showDebug = false;
}

CutSceneState::~CutSceneState()
{

}

bool CutSceneState::DisableMappableObjects() const
{
    
    return false;
}

void CutSceneState::Draw ( const clan::Rect& screenRect, clan::Canvas& GC )
{
    // TODO Center around the center of the middle tile instead of the top-left of it?

    uint width = std::min( (unsigned int)screenRect.get_width(), m_pLevel->GetWidth() * 32);
    uint height = std::min((unsigned int)screenRect.get_height(), m_pLevel->GetHeight() * 32);
    clan::Rect src(m_center.x - width /2,
                                m_center.y - height /2,
                                m_center.x + width /2,
                                m_center.y + height /2);	


    clan::Rect dst = screenRect;
    // Center
    if(screenRect.get_width() > src.get_width())
    {
        uint amount = (screenRect.get_width() - src.get_width()) /2;
        dst.left += amount;
        dst.right -= amount;
    }

    if(screenRect.get_height() > src.get_height())
    {
        uint amount = (screenRect.get_height() - src.get_height()) /2;
        dst.top += amount;
        dst.bottom -= amount;
    }	
	
	
	
    GC.clear();
    if(m_pLevel){
        m_pLevel->Draw(src,dst,GC,m_bDrawMOs,m_showDebug);
    }
    for(std::list<Task*>::iterator it = m_tasks.begin(); it != m_tasks.end();
        /* */){
        std::list<Task*>::iterator _it = it++;
        (*_it)->update();
        (*_it)->draw(screenRect,GC);     
        if((*_it)->finished()){
#ifndef NDEBUG
			std::cout << "CutSceneState::Draw finished task. cleaning up" << std::endl;
#endif
            (*_it)->cleanup();
            // Do something with waitFor?
#ifndef NDEBUG
			std::cout << "CutSceneState::Draw locking task mutex" << std::endl;
			std::cout.flush();
#endif
            m_task_mutex.lock();
            m_tasks.erase(_it);
            m_task_mutex.unlock();
#ifndef NDEBUG
			std::cout << "CutSceneState::Draw unlocked task mutex" << std::endl;
#endif
            m_wait_event.set();			
        }
    }
    
    GC.draw_box(screenRect,m_color);
    
    if(m_fade_level != 0.0f){
        GC.fill_rect(screenRect,clan::Colorf(0.0f,0.0f,0.0f,m_fade_level));
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
#ifndef NDEBUG
    if(button == IApplication::BUTTON_ALT)
        m_showDebug = !m_showDebug;
#endif
}

void CutSceneState::Init ( const SteelType::Functor& pFunctor )
{
    m_functor = pFunctor;
}

Level* CutSceneState::grabMapStateLevel()
{
    return IApplication::GetInstance()->GetCurrentLevel();
}

clan::Point CutSceneState::grabMapStateCenter()
{
	return IApplication::GetInstance()->GetCurrentLevelCenter();
}



void CutSceneState::Start()
{
    m_bDone = false;
    m_bDrawMOs = true;
    m_bFrozen = false;
    m_pLevel = grabMapStateLevel();
    m_center = grabMapStateCenter();  
    uncolorize();
    // Run script in Runner
    m_pRunner->setFunctor(m_functor);
    m_steel_thread.start(m_pRunner);
}

void CutSceneState::Finish()
{

   for(std::list<MappableObject*>::iterator it = m_faded_mos.begin();
            it != m_faded_mos.end(); it++){
            (*it)->SetAlpha(1.0f);
        }
        
    for(std::list<MappableObject*>::iterator it = m_temp_mos.begin();
        it != m_temp_mos.end(); it++)
        {
            IApplication::GetInstance()->GetCurrentLevel()->RemoveMappableObject(*it);
            delete *it;
        }        
    //m_functor.reset();
    m_steel_thread.join();
}

void CutSceneState::Completed()
{
    m_bDone = true;
    //m_steel_thread.join();
}


void CutSceneState::SteelInit ( 
#if SEPARATE_INTERPRETER
    SteelInterpreter* pMainInterpreter
#else
    SteelInterpreter* pInterpreter 
#endif
)
{
#if SEPARATE_INTERPRETER
#if COPY_INTERPRETER
    m_interpreter = *pMainInterpreter;
#endif
    SteelInterpreter * pInterpreter = &m_interpreter;
#endif
    
	pInterpreter->pushScope();
    m_pRunner = new CutSceneRunner(pInterpreter,this);
    // Add BIFs
    pInterpreter->addFunction("gotoLevel","scene",new SteelFunctor3Arg<CutSceneState,const std::string&,int,int>(this,&CutSceneState::gotoLevel));
    pInterpreter->addFunction("hideObject","scene",new SteelFunctor1Arg<CutSceneState,SteelType::Handle>(this,&CutSceneState::hideObject));
    pInterpreter->addFunction("unhideObject","scene", new SteelFunctor1Arg<CutSceneState,SteelType::Handle>(this,&CutSceneState::unhideObject));
    pInterpreter->addFunction("freezeObjects","scene",new SteelFunctorNoArgs<CutSceneState>(this,&CutSceneState::freezeObjects));
    pInterpreter->addFunction("unfreezeObjects","scene",new SteelFunctorNoArgs<CutSceneState>(this,&CutSceneState::unfreezeObjects));
    pInterpreter->addFunction("fadeIn","scene",new SteelFunctor1Arg<CutSceneState,double>(this,&CutSceneState::fadeIn));
    pInterpreter->addFunction("fadeOut","scene",new SteelFunctor1Arg<CutSceneState,double>(this,&CutSceneState::fadeOut));
    pInterpreter->addFunction("panTo","scene",new SteelFunctor3Arg<CutSceneState,int,int,double>(this,&CutSceneState::panTo));
    pInterpreter->addFunction("colorize","scene",new SteelFunctor3Arg<CutSceneState,double,double,double>(this,&CutSceneState::colorize));
    pInterpreter->addFunction("uncolorize","scene",new SteelFunctorNoArgs<CutSceneState>(this,&CutSceneState::uncolorize));
    pInterpreter->addFunction("getObject","scene",new SteelFunctor1Arg<CutSceneState,const std::string&>(this,&CutSceneState::getObject));
    pInterpreter->addFunction("getPlayer","scene",new SteelFunctorNoArgs<CutSceneState>(this,&CutSceneState::getPlayer));
    pInterpreter->addFunction("moveObject","scene",new SteelFunctor4Arg<CutSceneState,SteelType::Handle,int,int,int>(this,&CutSceneState::moveObject));
    pInterpreter->addFunction("changeFaceDirection","scene",new SteelFunctor2Arg<CutSceneState,SteelType::Handle,int>(this,&CutSceneState::changeFaceDirection));
    pInterpreter->addFunction("addSprite","scene",new SteelFunctor6Arg<CutSceneState,const std::string&,uint,uint,int,int,int>(this,&CutSceneState::addSprite));
    pInterpreter->addFunction("waitFor","scene",new SteelFunctor1Arg<CutSceneState,const SteelType::Handle&>(this,&CutSceneState::waitFor));
    pInterpreter->addFunction("pause","scene", new SteelFunctor1Arg<CutSceneState,double>(this,&CutSceneState::pause));
    pInterpreter->addFunction("dialog","scene",new SteelFunctor3Arg<CutSceneState,const std::string&,const std::string&, double>(this,&CutSceneState::dialog));   
    pInterpreter->addFunction("fadeObject","scene",new SteelFunctor3Arg<CutSceneState,SteelType::Handle,bool,double>(this,&CutSceneState::fadeObject));
    
    SteelConst(pInterpreter,"$_MOVE_SLOW", (int)MappableObject::SLOW);
    SteelConst(pInterpreter,"$_MOVE_MEDIUM",(int)MappableObject::MEDIUM);
    SteelConst(pInterpreter,"$_MOVE_FAST",(int)MappableObject::FAST);
    SteelConst(pInterpreter,"$_MOVE_NSEW",(int)MappableObject::MOVEMENT_WANDER);
    SteelConst(pInterpreter,"$_MOVE_NS",(int)MappableObject::MOVEMENT_PACE_NS);
    SteelConst(pInterpreter,"$_MOVE_EW",(int)MappableObject::MOVEMENT_PACE_EW);
    SteelConst(pInterpreter,"$_MOVE_NONE",(int)MappableObject::MOVEMENT_NONE);
    SteelConst(pInterpreter,"$_SIZE_SMALL",(int)MappableObject::MO_SMALL);
    SteelConst(pInterpreter,"$_SIZE_MEDIUM",(int)MappableObject::MO_MEDIUM);
    SteelConst(pInterpreter,"$_SIZE_LARGE",(int)MappableObject::MO_LARGE);
    SteelConst(pInterpreter,"$_SIZE_WIDE",(int)MappableObject::MO_WIDE);
    SteelConst(pInterpreter,"$_SIZE_TALL",(int)MappableObject::MO_TALL);
}

void CutSceneState::SteelCleanup ( SteelInterpreter* pInterpreter )
{
#if SEPARATE_INTERPRETER
    //m_interpreter.removeFunctions("scene");
#else
    //pInterpreter->removeFunctions("scene");
#endif
	pInterpreter->popScope();
    
    delete m_pRunner;
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
    static uint ticks = 0;
    clan::Rect displayRect = IApplication::GetInstance()->GetDisplayRect();
    clan::Rect rect(m_center.x - displayRect.get_width()/2,
                           m_center.y - displayRect.get_height()/2,
                           m_center.x + displayRect.get_width()/2,
                           m_center.y + displayRect.get_height()/2);
    // If we have the current level, then we don't move the MOs because
    // the map state will be doing that.
    if(m_pLevel != IApplication::GetInstance()->GetCurrentLevel())
        m_pLevel->MoveMappableObjects(rect);
    
    if(ticks++ % 4 == 0)
        m_pLevel->Update(rect);
}



void CutSceneState::PanTo ( int x, int y )
{
    m_center = clan::Point(x,y);
}

clan::Point CutSceneState::GetOrigin() const
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
        throw clan::Exception("gotoLevel must be called first.");
}

SteelType CutSceneState::addSprite ( const std::string& spriteRef, uint sprite_size, uint move_type, int x, int y, int face_dir )
{
    verifyLevel();
    
    MappableObjectDynamic * pMO = new MappableObjectDynamic(MappableObject::NPC,static_cast<MappableObject::eMovementType>(move_type),
        MappableObject::SLOW
    );
    
    class SpriteFunctor : public IApplication::Functor{
    public:
        SpriteFunctor(MappableObjectDynamic* pMO,Level * pLevel,const std::string& spriteRef, MappableObject::eSize size)
        :m_pMO(pMO),m_level(pLevel),m_spriteRef(spriteRef),m_size(size){
        }
        virtual void operator()(){
            m_sprite = GraphicsManager::CreateSprite(m_spriteRef);
            m_pMO->SetSprite(m_sprite, static_cast<MappableObject::eSize>(m_size));
            m_level->AddMappableObject(m_pMO);
        }
        MappableObjectDynamic *m_pMO;
        Level*          m_level;
        std::string     m_spriteRef;
        clan::Sprite       m_sprite;
        MappableObject::eSize m_size;        
    };
    clan::Event event;
    pMO->SetSolid(true);
    pMO->SetAlpha(1.0f);
    pMO->SetDefaultFacing(face_dir);
    pMO->SetPixelPosition(clan::Point(x*32,y*32));
    SpriteFunctor functor(pMO,m_pLevel,spriteRef,static_cast<MappableObject::eSize>(sprite_size));
	std::function<void()> func = functor;
    IApplication::GetInstance()->RunOnMainThread(func);
    
    // Anything we add in a cut scene to the actual level, we have to remove it
    // at the end of the cut scene.
    if(m_pLevel == IApplication::GetInstance()->GetCurrentLevel()){
        m_temp_mos.push_back(pMO);
    }
    SteelType ret;
    ret.set(pMO);
    
    return ret;
}

SteelType CutSceneState::colorize ( double r, double g, double b )
{
    m_color = clan::Colorf(r,g,b,0.75f);
    return SteelType();
}

SteelType CutSceneState::uncolorize()
{
    m_color = clan::Colorf(1.0f,1.0f,1.0f,1.0f);
    return SteelType();
}


SteelType CutSceneState::changeFaceDirection ( SteelType::Handle hHandle, int dir )
{
    verifyLevel();
    return SteelType();
}

SteelType CutSceneState::fadeIn ( double seconds )
{
    FadeTask * task = new FadeTask(*this,false,uint(seconds*1000));
    SteelType taskhandle;
    taskhandle.set(task);
    m_task_mutex.lock();
    m_tasks.push_back(task);
    m_task_mutex.unlock();
    task->start();
    return taskhandle;
}

SteelType CutSceneState::fadeOut ( double seconds )
{
    FadeTask * task = new FadeTask(*this,true,uint(seconds*1000));
    SteelType taskhandle;
    taskhandle.set(task);
    m_task_mutex.lock();
    m_tasks.push_back(task);
    m_task_mutex.unlock();
    task->start();
    return taskhandle;
}

SteelType CutSceneState::freezeObjects()
{
    verifyLevel();
    m_pLevel->FreezeMappableObjects();
    m_bFrozen = true;
    return SteelType();
}

SteelType CutSceneState::unfreezeObjects()
{
    verifyLevel();
    if(m_bFrozen)
        m_pLevel->UnfreezeMappableObjects();
    m_bFrozen = false;
    return SteelType();
}

SteelType CutSceneState::hideObject ( SteelType::Handle hHandle )
{
    verifyLevel();
    MappableObject * pObject = GrabHandle<MappableObject*>(hHandle);
    pObject->SetAlpha(0.0f);
    return SteelType();
}

SteelType CutSceneState::unhideObject ( SteelType::Handle hHandle )
{
    verifyLevel();
    MappableObject * pObject = GrabHandle<MappableObject*>(hHandle);
    pObject->SetAlpha(1.0f);    
    return SteelType();
}

SteelType CutSceneState::moveObject ( SteelType::Handle hHandle, int x, int y, int speed )
{
    verifyLevel();
    MappableObject * pMO = GrabHandle<MappableObject*>(hHandle);
    MoveTask * pTask = new MoveTask(*this,*m_pLevel,pMO,x,y,speed);
    m_task_mutex.lock();
    m_tasks.push_back(pTask);
    m_task_mutex.unlock();
    pTask->start();
    SteelType taskhandle;
    taskhandle.set(pTask);
    return taskhandle;
}

SteelType CutSceneState::panTo ( int x, int y, double seconds )
{
    PanTask *pTask = new PanTask(*this,x,y,uint(seconds*1000));
    m_task_mutex.lock();
    m_tasks.push_back(pTask);
    m_task_mutex.unlock();
    pTask->start();
    SteelType taskhandle;
    taskhandle.set(pTask);
    return taskhandle;
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

SteelType CutSceneState::getObject ( const  std::string& name )
{
    verifyLevel();
    SteelType val;
    val.set( m_pLevel->GetMappableObjectByName(name) );
    return val;
}

SteelType CutSceneState::getPlayer()
{
    verifyLevel();
    SteelType val;
    val.set(m_pLevel->GetPlayer());
    return val;
}

SteelType CutSceneState::waitFor ( const SteelType::Handle& waitOn )
{
    Task * pTask = GrabHandle<Task*>(waitOn);
    while(true){
        m_wait_event.wait();
        //m_task_mutex.lock();
        bool found = false;

        for ( std::list<Task*>::const_iterator it = m_tasks.begin();
            it != m_tasks.end(); it++ ) {
             if ( *it == pTask ) {
                found = true;
            }
        }
        //m_task_mutex.unlock();
        if(!found) 
            break;
    }
    return SteelType();
}

void CutSceneState::WaitTaskEvent()
{
	m_wait_event.wait();
}

SteelType CutSceneState::pause ( double seconds )
{
    clan::System::sleep(seconds * 1000.0);
	return SteelType();
}

SteelType CutSceneState::dialog ( const string& who, const string& what, double seconds )
{
    SayState state;
    state.Init(who,what, int(seconds * 1000),false);
    IApplication::GetInstance()->RunState(&state,true);
    return SteelType();
}

SteelType CutSceneState::fadeObject ( SteelType::Handle hHandle, bool in, double seconds )
{
    MappableObject * pObject = GrabHandle<MappableObject*>(hHandle);
    FadeMOTask *pTask = new FadeMOTask(*this,pObject,in,int(seconds * 1000));
    m_task_mutex.lock();
    m_tasks.push_back(pTask);
    m_task_mutex.unlock();
    pTask->start();
    SteelType taskhandle;
    taskhandle.set(pTask);
    
    if(m_pLevel == IApplication::GetInstance()->GetCurrentLevel()){
        m_faded_mos.push_back(pObject);
    }
    
    return taskhandle;
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
    m_start_time = clan::System::get_time();
}

void CutSceneState::FadeTask::update()
{
    const float p = float(clan::System::get_time() - m_start_time)/float(m_duration);
    m_state.SetFadeLevel(m_fade_out?p:1-p);
    ++m_updateCount;
}

bool CutSceneState::FadeTask::finished()
{
    return (clan::System::get_time() >= (m_start_time + m_duration));
}

void CutSceneState::FadeTask::cleanup()
{
    StoneRing::CutSceneState::Task::cleanup();
}


/*  Move */
void CutSceneState::MoveTask::start()
{
    m_pNav = new ScriptedNavigator(m_level,*m_pMO,m_target,m_movementSpeed);
    m_pMO->PushNavigator(m_pNav);  
}

void CutSceneState::MoveTask::cleanup()
{
    m_pNav = NULL;
    delete m_pMO->PopNavigator();
}

void CutSceneState::MoveTask::update()
{
    // nothing to do here. the movement code actually moves the guy
}

bool CutSceneState::MoveTask::finished()
{ 
    return m_pMO->GetPosition() == m_target || (m_pNav && m_pNav->Complete());
}


/* Pan */
void CutSceneState::PanTask::start()
{
    m_start_time = clan::System::get_time();
    m_origin = m_state.GetOrigin();
}

void CutSceneState::PanTask::update()
{
    const float p = float(clan::System::get_time() - m_start_time)/float(m_duration);
    clan::Vec2<float> origin = m_origin;
    clan::Vec2<float> target(m_target.x * 32, m_target.y * 32);
    clan::Vec2<float> current;
    current.x = origin.x + p*target.x;
    current.y = origin.y + p*target.y;
    m_state.PanTo(current.x,current.y);
}

bool CutSceneState::PanTask::finished()
{
    return (clan::System::get_time() - m_start_time > m_duration);
}

void CutSceneState::FadeMOTask::start()
{
    m_start_time = clan::System::get_time();
}

void CutSceneState::FadeMOTask::update()
{
    const float p = float(clan::System::get_time() - m_start_time)/float(m_duration);
    float f = m_fade_in?p:1.0f-p;
    if(f > 1.0f) f = 1.0f;
    if(f < 0.0f) f = 0.0f;
    m_pObject->SetAlpha(f);
}

bool CutSceneState::FadeMOTask::finished()
{
    return (clan::System::get_time() - m_start_time > m_duration);
}



}