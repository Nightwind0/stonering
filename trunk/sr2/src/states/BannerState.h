/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2012  <copyright holder> <email>

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


#ifndef BANNERSTATE_H
#define BANNERSTATE_H

#include "State.h"

namespace StoneRing { 

class BannerState : public State
{

public:
    BannerState();
    virtual ~BannerState();
    void Init(const std::string& text, int time);
    virtual bool IsDone() const;    
    virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);    
    virtual void HandleButtonUp(const IApplication::Button& button);
    virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
    virtual bool LastToDraw() const; // Should we continue drawing more states?
    virtual bool DisableMappableObjects() const; // Should the app move the MOs?
    virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
    virtual void Start();
    virtual void Finish(); // Hook to clean up or whatever after being popped
	virtual bool AcceptInput() const;
	void BringDown();
private:
    CL_Rect m_banner_rect;
    CL_Pointf m_text_inset;    
    Font m_font;
    std::string m_text;
    int m_time;
    int m_start_time;
    bool m_done;
};

}
#endif // BANNERSTATE_H
