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


#ifndef SAVELOADSTATE_H
#define SAVELOADSTATE_H

#include "State.h"
#include "Menu.h"
#include "AppUtils.h"

namespace StoneRing {
class SaveLoadState : public State, public Menu
{

public:
    SaveLoadState();
    virtual ~SaveLoadState();
    
    void                Init(bool bSave, bool cancelable=true);
    virtual bool        IsDone() const;

    virtual bool        Threaded() const { return false; }
    virtual void        Draw(const clan::Rect &screenRect,clan::Canvas& GC);
    virtual bool        LastToDraw() const; // Should we continue drawing more states?
    virtual bool        DisableMappableObjects() const; // Should the app move the MOs?
    virtual void        Update(); // Do stuff right after the mappable object movement
    virtual void        Start();
    virtual void        Finish(); // Hook to clean up or whatever after being popped
	bool 				Cancelled() const { return m_cancelled; }
protected:
    // Handle joystick / key events that are processed according to mappings
    virtual void        HandleButtonUp(const IApplication::Button& button);
    virtual void        HandleButtonDown(const IApplication::Button& button);
    virtual void        HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);
    	
    virtual clan::Rectf    get_rect();
    virtual void        draw_option(int option, bool selected, const clan::Rectf& rect, clan::Canvas& gc);
    virtual int         height_for_option(clan::Canvas& gc);
    virtual void        process_choice(int selection);
    virtual int         get_option_count();
private:
   
    
	void load_file_previews();
    void                clear_previews();
    std::map<uint,AppUtils::SaveSummary> m_previews;
    Font                m_hours_font;
    Font                m_number_font;
    Font                m_num_selected_font;
    Font                m_datetime_font;
    Font                m_empty_font;
    Font                m_empty_selected_font;
	clan::Image  			m_portrait_shadow;
    clan::Pointf           m_number_pt;
    clan::Pointf           m_datetime_pt;
    clan::Pointf           m_empty_pt;
    clan::Pointf           m_portrait_pt;
    clan::Pointf           m_minutes_pt;
    bool                m_bSave;
    bool                m_bDone;
    bool                m_cancelable;
	bool 				m_cancelled;
};

}
#endif // SAVELOADSTATE_H
