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

namespace StoneRing {
class SaveLoadState : public State, public Menu
{

public:
    SaveLoadState();
    virtual ~SaveLoadState();
    
    void                Init(bool bSave, bool cancelable=true);
    virtual bool        IsDone() const;
    // Handle joystick / key events that are processed according to mappings
    virtual void        HandleButtonUp(const IApplication::Button& button);
    virtual void        HandleButtonDown(const IApplication::Button& button);
    virtual void        HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);
    
    virtual bool        Threaded() const { return false; }
    virtual void        Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
    virtual bool        LastToDraw() const; // Should we continue drawing more states?
    virtual bool        DisableMappableObjects() const; // Should the app move the MOs?
    virtual void        MappableObjectMoveHook(); // Do stuff right after the mappable object movement
    virtual void        Start();
    virtual void        Finish(); // Hook to clean up or whatever after being popped
protected:
    virtual CL_Rectf    get_rect();
    virtual void        draw_option(int option, bool selected, float x, float y, CL_GraphicContext& gc);
    virtual int         height_for_option(CL_GraphicContext& gc);
    virtual void        process_choice(int selection);
    virtual int         get_option_count();
private:
    struct FilePreview {
        struct CharInfo{
            std::string m_name;
            uint m_level;
        };
        uint                    m_minutes;
        uint                    m_gold;        
        std::list<CharInfo>     m_characters;
        CL_DateTime             m_datetime;
    };    
    
    std::string         filename_for_slot(uint slot);
    void                load_file_previews();
    bool                verify_file(std::istream& in);
    FilePreview         load_file_header(std::istream& in);
    void                save(uint slot);
    bool                load(uint slot);
    void                clear_previews();
    std::map<uint,FilePreview> m_previews;
    Font                m_hours_font;
    Font                m_number_font;
    Font                m_num_selected_font;
    Font                m_datetime_font;
    Font                m_empty_font;
    Font                m_empty_selected_font;
    CL_Pointf           m_number_pt;
    CL_Pointf           m_datetime_pt;
    CL_Pointf           m_empty_pt;
    bool                m_bSave;
    bool                m_bDone;
    bool                m_cancelable;
};

}
#endif // SAVELOADSTATE_H
