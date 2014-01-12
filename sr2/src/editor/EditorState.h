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


#ifndef EDITORTESTSTATE_H
#define EDITORTESTSTATE_H

#include "State.h"
#include <ClanLib/gui.h>
#include <ClanLib/csslayout.h>
#if SR2_EDITOR

namespace clan { 
  class DisplayWindow;
}
namespace StoneRing { 

    class EditorState : public State
    {
    public:
        EditorState();
        virtual ~EditorState();
        virtual void Init(clan::DisplayWindow& window);
        virtual void HandleButtonUp(const IApplication::Button& button);
        virtual void HandleButtonDown(const IApplication::Button& button);
        virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);
        virtual void HandleMouseUp(const IApplication::MouseButton& button, const clan::Point& pos, uint key_state );
        virtual void HandleMouseDown(const IApplication::MouseButton& button, const clan::Point& pos, uint key_state );
        virtual void HandleDoubleClick(const IApplication::MouseButton& button, const clan::Point& pos, uint key_state );
        virtual void HandleMouseMove(const clan::Point& pos, uint key_state );     
        virtual void Draw(const clan::Rect &screenRect,clan::Canvas& GC);
        virtual bool LastToDraw() const; // Should we continue drawing more states?
        virtual bool DisableMappableObjects() const ; // Should the app move the MOs?
        virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void Start();
        virtual void Finish(); // Hook to clean up or whatever after being popped     
        virtual bool IsDone()const;
        
        virtual bool on_close(clan::Window*);        
		virtual void on_display_resize(clan::Rect& rect){}
    protected:
        //virtual clan::Window* create_main_window()=0;
        void finish();
        virtual clan::Size get_window_size()const=0;
        clan::GUIManager* get_gui() { return &m_gui_manager; }
    private:
        bool m_bDone;
        // GUI stuff
        clan::AcceleratorTable m_accelerator_table;
        clan::ResourceManager m_resources;
        //clan::GUIThemeDefault m_theme;
        clan::CSSDocument m_css_document;
        clan::GUIWindowManagerTexture m_window_manager;
        clan::GUIManager m_gui_manager;  
        clan::Window* m_pMainWindow;
        clan::DisplayWindow m_display_window;
        float m_latch_vol;
 
        //clan::DisplayWindow* m_subwindow;
    };

}
#endif

#endif // EDITORTESTSTATE_H
