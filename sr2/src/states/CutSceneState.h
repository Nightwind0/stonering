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

namespace StoneRing {
    
class MappableObject;
class Level;

class CutSceneState : public State
{
public:
    CutSceneState();
    virtual ~CutSceneState();
    void Init(SteelFunctor* pFunctor);
    virtual bool IsDone() const;
    // Handle raw key events
    // Handle joystick / key events that are processed according to mappings
    virtual void HandleButtonUp(const IApplication::Button& button);
    virtual void HandleButtonDown(const IApplication::Button& button);
    virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);
        
    virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
    virtual bool LastToDraw() const; // Should we continue drawing more states?
    virtual bool DisableMappableObjects() const; // Should the app move the MOs?
    virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
    virtual void Start();
    virtual void SteelInit      (SteelInterpreter *);
    virtual void SteelCleanup   (SteelInterpreter *);
    virtual void Finish(); // Hook to clean up or whatever after being popped
    
    void SetFadeLevel(float level);
    void MoveCharacterTo(MappableObject* pMO,int x, int y, int speed);
    void PanTo(int x, int y);
    CL_Point GetOrigin()const;
private:
    void verifyLevel();
    Level * grabMapStateLevel();
    SteelType gotoLevel(const std::string&,int x, int y);
    SteelType hideCharacter(SteelType::Handle hHandle);
    SteelType unhideCharacter(SteelType::Handle hHandle);
    SteelType freezeCharacters(); // Stop automatic NPC movement
    SteelType unfreezeCharacters();
    SteelType fadeIn(float seconds);
    SteelType fadeOut(float seconds);
    SteelType panTo(int x, int y,float seconds);
    SteelType colorize(float r, float g, float b);
    SteelType uncolorize();
    SteelType getCharacter(const std::string& str);
    SteelType getPlayer();
    SteelType moveCharacter(SteelType::Handle hHandle, int x, int y, int speed);
    SteelType faceCharacter(SteelType::Handle hHandle, int dir);
    SteelType addCharacter(const std::string& spriteRef, int x, int y, int face_dir);
    
    class Task {
    public:
        Task(CutSceneState& state):m_state(state){
        }
        virtual ~Task(){}
        virtual void start()=0;
        virtual void update()=0;
        virtual bool finished()=0;
    protected:   
        CutSceneState& m_state;
    };
    
    class FadeTask : public Task {
    public:
        FadeTask(CutSceneState& state, bool out):Task(state),m_fade_out(out){
        }
        virtual ~FadeTask(){}
        virtual void start();
        virtual void update();
        virtual bool finished();
    private:
        bool m_fade_out;
    };
    
    class MoveTask : public Task {
    public:
        MoveTask(CutSceneState& state,MappableObject*,int x, int y, int speed):Task(state){
        }
        virtual ~MoveTask(){}
        virtual void start();
        virtual void update();
        virtual bool finished();
    private:
        MappableObject* m_pMO;
        CL_Point m_target;
        int m_movementSpeed;
    };
    
    class PanTask : public Task {
    public:
        PanTask(CutSceneState& state, MappableObject*,int x, int y,float duration):Task(state){
        }
        virtual ~PanTask(){}
        virtual void start();
        virtual void update();
        virtual bool finished();
    private:
        CL_Point m_target;
        MappableObject* m_pMO;
        CL_Point m_origin;
        float m_duration;
    };
    
    Level * m_pLevel;
    SteelRunner* m_pRunner;
    shared_ptr<SteelFunctor> m_functor;
    std::list<Task*> m_tasks;
    bool m_bDone;
    bool m_bDrawMOs;
    bool m_bFrozen;
    CL_Point m_center;
    CL_Colorf m_color;
    float m_fade_level;
};


}
#endif // CUTSCENESTATE_H
