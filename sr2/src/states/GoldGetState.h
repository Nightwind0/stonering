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


#ifndef GOLDGETSTATE_H
#define GOLDGETSTATE_H

#include "State.h"
#include "Menu.h"



namespace StoneRing { 

class GoldGetState : public State
{
public:
    GoldGetState();
    virtual ~GoldGetState();
	void SetGold(int gold);
	virtual bool IsDone() const;	
	virtual void HandleButtonUp( const IApplication::Button& button );
	virtual void HandleButtonDown( const IApplication::Button& button ) ;
	virtual void HandleAxisMove( const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos );	
	virtual void Draw( const CL_Rect &screenRect, CL_GraphicContext& GC );
	virtual bool LastToDraw() const; // Should we continue drawing more states?
	virtual bool DisableMappableObjects() const; // Should the app move the MOs?
	virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
	virtual void Start();
	virtual void SteelInit( SteelInterpreter * );
	virtual void SteelCleanup( SteelInterpreter * );
	virtual void Finish(); // Hook to clean up or whatever after being popped	
private:
	void on_sound_timer();
	uint m_gold;
	CL_Timer m_sound_timer;
	bool m_done;
	bool m_done_display;
	CL_Pointf m_icon_offset;
	CL_Pointf m_text_offset;
	CL_Image m_icon;
	uint m_start_time;
	CL_Rect m_rect;
	std::string m_text;
	Font m_gold_font;
};


}
#endif // ITEMGETSTATE_H