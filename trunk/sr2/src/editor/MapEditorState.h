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


#ifndef MAPEDITORTESTSTATE_H
#define MAPEDITORTESTSTATE_H

#include "EditorState.h"
#include <ClanLib/gui.h>
#include "MapComponent.h"

#if SR2_EDITOR

class CL_DisplayWindow;

namespace StoneRing { 

    class MapEditorState : public EditorState
    {
    public:
        MapEditorState();
        virtual ~MapEditorState();
        virtual void Init(CL_DisplayWindow & window);
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
        
        virtual void on_button_clicked(CL_PushButton*);
        virtual bool on_close(CL_Window*);
    private:
        
        enum Modifier {
            ALT=1,
            SHIFT=2,
            CTRL=4
        };
        
        enum MouseButton {
            MOUSE_LEFT,
            MOUSE_RIGHT
        }m_drag_button;
        
        enum Menu {
            FILE_MENU
        };
        
        enum MouseState {
            MOUSE_IDLE,
            MOUSE_DOWN,
            MOUSE_DRAG
        }m_mouse_state;
        
        int  mod_value(bool shift,bool ctrl,bool alt)const;
        void on_file_open();
        void on_file_new();
        void on_file_save();
        void on_file_quit();
        
        void on_edit_grow_column();
        void on_edit_grow_row();
        void on_edit_grow_column5();
        void on_edit_grow_row5();
        void on_zoom_changed();
        bool on_mouse_moved(const CL_InputEvent&);
        bool on_mouse_pressed(const CL_InputEvent&);
        bool on_mouse_released(const CL_InputEvent&);
        void on_mouse_double_click(const CL_InputEvent&);
        bool on_pointer_entered();
        bool on_pointer_exit();
        
        // mouse 
        void start_drag(const CL_Point&, MouseButton button, int mod_state);
        void update_drag(const CL_Point& start,const CL_Point& prev_point,const CL_Point& point,MouseButton button, int mod_state);
        void cancel_drag();
        void end_drag(const CL_Point& start,const CL_Point& prev_point, const CL_Point& point,MouseButton button, int mod_state);
        
        
        void construct_menu();
        bool m_bDone;
        // GUI stuff      
        CL_Window * m_pWindow;
        CL_PopupMenu m_file_menu;
        CL_PopupMenu m_edit_menu;
        CL_PopupMenu m_grow_submenu;
        CL_MenuBar * m_pMenuBar;
        CL_PushButton* m_pButton;
        CL_Slider* m_pZoomSlider;
   
        MapComponent* m_pMap;
        int m_mod_state;
        CL_Point m_drag_start;
        CL_Point m_last_drag_point;
        //CL_DisplayWindow* m_subwindow;
    };

}
#endif

#endif // EDITORTESTSTATE_H
