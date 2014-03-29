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


#ifndef ITEMGETSTATE_H
#define ITEMGETSTATE_H

#include "State.h"
#include "Menu.h"



namespace StoneRing { 

class ItemGetState : public State, public Menu 
{
public:
    ItemGetState();
    virtual ~ItemGetState();
	void SetItems(const std::vector<Item*>& items, const std::vector<uint>& counts);
	virtual bool IsDone() const;	
	virtual void Draw( const clan::Rect &screenRect, clan::Canvas& GC );
	virtual bool LastToDraw() const; // Should we continue drawing more states?
	virtual bool DisableMappableObjects() const; // Should the app move the MOs?
	virtual void Update(); // Do stuff right after the mappable object movement
	virtual void Start();
	virtual void SteelInit( SteelInterpreter * );
	virtual void SteelCleanup( SteelInterpreter * );
	virtual void Finish(); // Hook to clean up or whatever after being popped	
	
	// Menu 
	virtual clan::Rectf get_rect();
    virtual void draw_option(int option, bool selected, float x, float y, clan::Canvas& gc);
    virtual int height_for_option(clan::Canvas& gc);
    virtual void process_choice(int selection);
    virtual int get_option_count();
protected:
	virtual void HandleButtonUp( const IApplication::Button& button );
	virtual void HandleButtonDown( const IApplication::Button& button ) ;
	virtual void HandleAxisMove( const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos );		
private:
	void on_sound_timer();
	std::vector<Item*> m_items;
	std::vector<uint> m_counts;
	uint m_sound_count;
	uint m_item_cursor;
	clan::Timer m_sound_timer;
	bool m_done;
	bool m_done_display;
	clan::Pointf m_offset;
	uint m_start_time;
	clan::Rect m_rect;
	clan::Rect m_header_rect;
	Font m_item_font;
	Font m_header_font;
};


}
#endif // ITEMGETSTATE_H
