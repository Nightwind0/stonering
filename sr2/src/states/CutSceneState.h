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


#ifndef CUTSCENESTATE_H
#define CUTSCENESTATE_H

#include "State.h"
#include "SteelRunner.h"

#define SEPARATE_INTERPRETER 0
#define COPY_INTERPRETER 0

namespace StoneRing {
    
class MappableObject;
class Level;
class ScriptedNavigator;

class CutSceneState : public State
{
public:
    CutSceneState();
    virtual ~CutSceneState();
    void Init(const SteelType::Functor& pFunctor);
    virtual bool IsDone() const;
    // Handle raw key events
    // Handle joystick / key events that are processed according to mappings
    virtual void HandleButtonUp(const IApplication::Button& button);
    virtual void HandleButtonDown(const IApplication::Button& button);
    virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);
    
    virtual bool Threaded() const { return true; }
    virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
    virtual bool LastToDraw() const; // Should we continue drawing more states?
    virtual bool DisableMappableObjects() const; // Should the app move the MOs?
    virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
    virtual void Start();
    virtual void SteelInit      (SteelInterpreter *);
    virtual void SteelCleanup   (SteelInterpreter *);
    virtual void Finish(); // Hook to clean up or whatever after being popped
    
    bool HasTasks() const { return !m_tasks.empty(); }
    void SetFadeLevel(float level);
    void PanTo(int x, int y);
    CL_Point GetOrigin()const;
    void Completed();
    void WaitTaskEvent();
private:
    void verifyLevel();
    Level * grabMapStateLevel();
	CL_Point grabMapStateCenter();
    SteelType gotoLevel(const std::string&,int x, int y);
    SteelType hideObject(SteelType::Handle hHandle);
    SteelType unhideObject(SteelType::Handle hHandle);
    SteelType freezeObjects(); // Stop automatic NPC movement
    SteelType unfreezeObjects();
    SteelType fadeIn(double seconds);
    SteelType fadeOut(double seconds);
    SteelType panTo(int x, int y,double seconds);
	SteelType panToObject(SteelType::Handle hObject, double seconds);
    SteelType colorize(double r, double g, double b);
    SteelType uncolorize();
    SteelType getObject(const std::string& str);
    SteelType getPlayer();
	// TODO: Right now an object that is offscreen won't move... so we probably need to get around that or do it differently
    SteelType moveObject(SteelType::Handle hHandle, int x, int y, int speed);
    SteelType changeFaceDirection(SteelType::Handle hHandle, int dir);
    SteelType addSprite(const std::string& spriteRef, uint sprite_size, uint move_type, int x, int y, int face_dir);
    SteelType waitFor(const SteelType::Handle& waitOn);
    SteelType pause(double seconds);
    SteelType dialog(const std::string& who, const std::string& what, double seconds);
    SteelType fadeObject(SteelType::Handle hHandle, bool in, double seconds);
	
    
    class Task : public SteelType::IHandle {
    public:
        Task(CutSceneState& state):m_state(state){
        }
        virtual ~Task(){}
        virtual void start()=0;
        virtual void update()=0;
        virtual void draw(const CL_Rect& screenRect, CL_GraphicContext &gc){}
        virtual bool finished()=0;
        virtual void cleanup(){}
    protected:   
        CutSceneState& m_state;
    };
    
    class FadeTask : public Task {
    public:
        FadeTask(CutSceneState& state, bool out, uint duration):Task(state),m_fade_out(out),m_duration(duration){
            m_updateCount = 0;
        }
        virtual ~FadeTask(){}
        virtual void start();
        virtual void update();
        virtual void draw(const CL_Rect& screenRect, CL_GraphicContext &gc){}
        virtual bool finished();
        virtual void cleanup();
    private:
        int m_updateCount;
        bool m_fade_out;
        uint m_start_time;
        uint m_duration;        
    };
    
    class MoveTask : public Task {
    public:
        MoveTask(CutSceneState& state,Level& level,MappableObject* pMo,int x, int y, int speed):Task(state),m_level(level),m_pMO(pMo),m_pNav(NULL),m_movementSpeed(speed){
            m_target = CL_Point(x,y);
        }
        virtual ~MoveTask(){}
        virtual void start();
        virtual void update();
        virtual bool finished();
        virtual void cleanup();
    private:
        MappableObject* m_pMO;
        Level& m_level;
        ScriptedNavigator* m_pNav;
        CL_Point m_target;
        int m_movementSpeed;
    };
    
    class PanTask : public Task {
    public:
        PanTask(CutSceneState& state,int x, int y,uint ms_duration):Task(state),m_target(x,y),m_duration(ms_duration){
        }
        virtual ~PanTask(){}
        virtual void start();
        virtual void update();
        virtual bool finished();
    private:
        CL_Point m_target;
        CL_Point m_origin;
        uint m_start_time;
        uint m_duration;
    };
    
    class FadeMOTask : public Task { 
    public:
        FadeMOTask(CutSceneState& state, MappableObject* pObject, bool in, uint ms_duration):Task(state),m_pObject(pObject),m_fade_in(in),m_duration(ms_duration){
        }
        virtual ~FadeMOTask() {}
        virtual void start();
        virtual void update();
        virtual bool finished();
    private:
        MappableObject * m_pObject;
        bool m_fade_in;
        uint m_duration;
        uint m_start_time;
    };
    
    Level * m_pLevel;
    SteelRunner<CutSceneState>* m_pRunner;
    SteelType::Functor m_functor;
    std::list<Task*> m_tasks;
    bool m_bDone;
    bool m_bDrawMOs;
    bool m_bFrozen;
    CL_Point m_center;
    CL_Colorf m_color;
    float m_fade_level;
    CL_Event m_wait_event;
    CL_Thread m_steel_thread;
    CL_Mutex m_task_mutex;
    std::list<MappableObject*> m_temp_mos;
    std::list<MappableObject*> m_faded_mos;
#if SEPARATE_INTERPRETER
    SteelInterpreter m_interpreter;
#endif
    bool m_showDebug;
};


}
#endif // CUTSCENESTATE_H
