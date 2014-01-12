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
#include "Callback.h"
#include <deque>
#include <list>

#ifdef SR2_EDITOR

namespace clan { 
  class DisplayWindow;
}
namespace StoneRing { 

    class Operation;
    class TileSelectorWindow;
	class MapWindow;
    
    class MapEditorState : public EditorState
    {
    public:
        MapEditorState();
        virtual ~MapEditorState();
	
        virtual void Init(clan::DisplayWindow & window);
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
			
		// Sets an operation to be performed by click
		void SetOperation(int mods,Operation*); 
		void CloseMapWindow(MapWindow* pWindow);
		void SetCopyTiles(const std::vector<std::vector<std::list<Tile*> > >& tiles);
		std::vector<std::vector<std::list<Tile*> > > GetCopyTiles() const{
			return m_copy_tiles;
		}
		
        virtual bool on_close();
		virtual void on_display_resize(clan::Rect&);
    private:
        virtual clan::Size get_window_size()const{ return clan::Size(1024,768); }    
        void construct_menu();
        
        enum Menu {
            FILE_MENU
        };
        

        void on_file_open();
        void on_file_new();
        void on_file_quit();	
		MapWindow * new_map_window(const std::string& title);
        
        clan::PopupMenu            m_file_menu;
        clan::Window *         m_pWindow;
        TileSelectorWindow*     m_pTileWindow;
        clan::MenuBar *            m_pMenuBar;		
		std::list<MapWindow*>   m_map_windows;
		std::vector<std::vector<std::list<Tile*> > > m_copy_tiles;
		bool m_bDone;
        //clan::DisplayWindow* m_subwindow;
    };

}
#endif

#endif // EDITORTESTSTATE_H
