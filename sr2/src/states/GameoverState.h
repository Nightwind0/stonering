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


#ifndef GAMEOVERSTATE_H
#define GAMEOVERSTATE_H

#include "State.h"

namespace StoneRing {

class GameoverState : public State{
public:
	GameoverState();
	virtual ~GameoverState();
	void Init();
	virtual bool IsDone() const ;	
	virtual void HandleButtonUp( const IApplication::Button& button );
	virtual void HandleButtonDown( const IApplication::Button& button ) ;

	virtual bool Threaded() const {
		return false;
	}
	
	virtual void Draw( const CL_Rect &screenRect, CL_GraphicContext& GC );
	virtual bool LastToDraw() const ; // Should we continue drawing more states?
	virtual bool DisableMappableObjects() const; // Should the app move the MOs?
	virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
	virtual void Start();
	virtual void Finish(); // Hook to clean up or whatever after being popped	
private:
	CL_Image m_background;
	bool m_done;
	uint64 m_start_time;
};

}
#endif // GAMEOVERSTATE_H
