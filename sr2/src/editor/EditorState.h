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

#if SR2_EDITOR

class CL_DisplayWindow;

namespace StoneRing { 

    class EditorState : public State
    {
    public:
        EditorState();
        virtual ~EditorState();
        virtual void Init(CL_DisplayWindow& window);
        virtual void HandleButtonUp(const IApplication::Button& button);
        virtual void HandleButtonDown(const IApplication::Button& button);
        virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);
        virtual void HandleMouseUp(const IApplication::MouseButton& button, const CL_Point& pos, uint key_state );
        virtual void HandleMouseDown(const IApplication::MouseButton& button, const CL_Point& pos, uint key_state );
        virtual void HandleDoubleClick(const IApplication::MouseButton& button, const CL_Point& pos, uint key_state );
        virtual void HandleMouseMove(const CL_Point& pos, uint key_state );     
        virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
        virtual bool LastToDraw() const; // Should we continue drawing more states?
        virtual bool DisableMappableObjects() const ; // Should the app move the MOs?
        virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void Start();
        virtual void Finish(); // Hook to clean up or whatever after being popped     
        virtual bool IsDone()const;
        
        virtual bool on_close(CL_Window*);        
		virtual void on_display_resize(CL_Rect& rect){}
    protected:
        //virtual CL_Window* create_main_window()=0;
        void finish();
        virtual CL_Size get_window_size()const=0;
        CL_GUIManager* get_gui() { return &m_gui_manager; }
    private:
        bool m_bDone;
        // GUI stuff
        CL_AcceleratorTable m_accelerator_table;
        CL_ResourceManager m_resources;
        CL_GUIThemeDefault m_theme;
        CL_CSSDocument m_css_document;
        CL_GUIWindowManagerTexture m_window_manager;
        CL_GUIManager m_gui_manager;  
        CL_Window* m_pMainWindow;
        CL_DisplayWindow m_display_window;
        float m_latch_vol;
 
        //CL_DisplayWindow* m_subwindow;
    };

}
#endif

#endif // EDITORTESTSTATE_H
