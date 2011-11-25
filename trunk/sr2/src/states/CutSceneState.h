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

namespace StoneRing {

class CutSceneState : public State
{
public:
    CutSceneState();
    virtual ~CutSceneState();
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
private:
    SteelType gotoLevel(const std::string&,int x, int y);
    SteelType fadeIn(float seconds);
    SteelType fadeOut(float seconds);
    SteelType panTo(int x, int y,float seconds);
    SteelType colorize(float r, float g, float b);
    SteelType getCharacter(std::string& str);
    SteelType moveCharacter(SteelType::Handle hHandle, int x, int y);
    SteelType faceCharacter(SteelType::Handle hHandle, int dir);
    SteelType addCharacter(std::string& spriteRef, int x, int y, int face_dir);
    bool m_bDone;
};


}
#endif // CUTSCENESTATE_H
