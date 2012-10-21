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


#ifndef TILESELECTORWINDOW_H
#define TILESELECTORWINDOW_H

#include <ClanLib/gui.h>

namespace StoneRing {

class TileSelector;
class MapEditorState;


class TileSelectorWindow : public CL_Window
{

public:
    TileSelectorWindow(CL_GUIComponent* owner, const CL_GUITopLevelDescription &desc);
    virtual ~TileSelectorWindow();
    void SetMapEditor(MapEditorState* state);
    void SetTilemap(CL_Image image, const std::string& name);
private:
    void create_menu();
    void on_tilemap_clicked(int tilemap);
    void on_tilemap_change(CL_Image sprite);
    void on_vert_scroll();
    void on_horiz_scroll();
    
    CL_MenuBar*         m_pMenuBar;
    CL_ScrollBar*       m_pVertScroll;
    CL_ScrollBar*       m_pHorizScroll;
    TileSelector*       m_pTileSelector;
    std::vector<CL_String> m_tilemaps;
    std::vector<CL_PopupMenuItem> m_tilemap_items;
};

}

#endif // TILESELECTORWINDOW_H
