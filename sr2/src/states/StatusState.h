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


#ifndef STATUSSTATE_H
#define STATUSSTATE_H

#include "State.h"


namespace StoneRing {

class StatusState : public State
{

public:
    StatusState();
    virtual ~StatusState();
    virtual bool IsDone() const;
	
	void SetParty(ICharacterGroup* party) {
		m_party = party;
	}
 
    virtual bool Threaded() const { return false; }
    virtual void Draw(const clan::Rect &screenRect,clan::Canvas& GC);
    virtual bool LastToDraw() const; // Should we continue drawing more states?
    virtual bool DisableMappableObjects() const; // Should the app move the MOs?
    virtual void Update(); // Do stuff right after the mappable object movement
    virtual void Start();
    virtual void Finish(); // Hook to clean up or whatever after being popped    
protected:
    // Handle joystick / key events that are processed according to mappings
    virtual void HandleButtonUp(const IApplication::Button& button);
    virtual void HandleButtonDown(const IApplication::Button& button);
    virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);
   	
private:
    void        draw_party(clan::Canvas& gc);
    void        draw_stats(clan::Canvas& gc, Character * pChar);
    void        draw_header(clan::Canvas& gc);
	ICharacterGroup * m_party;
    std::vector<ICharacter::eCharacterAttribute> m_stats;
    clan::Rectf    m_party_rect;
    clan::Rectf    m_status_rect;
    clan::Rectf    m_header_rect;
    Font        m_stat_name_font;
    Font        m_stat_font;
    Font        m_plus_font;
    Font        m_minus_font;
    Font        m_header_font;
    clan::Pointf   m_name_x;
    clan::Pointf   m_stat_x;
    clan::Pointf   m_mod_x;
    clan::Pointf   m_base_x;
    int         m_current_character;
    bool        m_bDone;
};

}

#endif // STATUSSTATE_H
