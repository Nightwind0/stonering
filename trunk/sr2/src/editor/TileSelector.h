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


#ifndef TILESELECTOR_H
#define TILESELECTOR_H

#include <ClanLib/gui.h>
#include <ClanLib/display.h>
#include "Operation.h"
#include <list>

namespace StoneRing  {
    
    class MapEditorState;
    class Tile;
    
class TileSelector : public CL_GUIComponent
{
public:
    class AddTileOperation : public Operation {
    public:
        void operator=(const AddTileOperation& other);
        virtual Operation* clone();
        virtual bool Execute(shared_ptr<Level>);
        virtual void Undo(shared_ptr<Level>);      
        void SetPoint(const CL_Point& point) { m_tile_pos = point; }
        void SetName(const std::string& name) { m_tilemap = name; }
        CL_Point GetPoint() const { return m_tile_pos; }
    private:
        std::string m_tilemap;
        CL_Point    m_tile_pos;
        Tile *      m_tile;
        std::list<Tile*> m_removed_tiles;
    };
    class AddTilesOperation: public OperationGroup {
    public:
        AddTilesOperation(){}
        virtual ~AddTilesOperation(){}
        void SetPoint(const CL_Point& point) { m_tile_pos = point; }
        void SetName(const std::string& name) { m_tilemap = name; }
        Operation* clone() {
            AddTilesOperation * pOp = new AddTilesOperation;
            pOp->SetPoint(m_tile_pos);
            pOp->SetName(m_tilemap);
            return pOp;
        }
        CL_Point GetPoint() const { return m_tile_pos; }        
    protected:
        virtual Operation* create_suboperation() const { 
            AddTileOperation * pOp =  new AddTileOperation;
            pOp->SetPoint(m_tile_pos);
            pOp->SetName(m_tilemap);
            return pOp;
        }
    protected:
        std::string m_tilemap;
        CL_Point    m_tile_pos;        
    };
    
    
    TileSelector(CL_GUIComponent* parent);
    virtual ~TileSelector();
    CL_Size get_image_size() const { return m_image.get_size(); }
    void SetTilemap(CL_Sprite image, const std::string& name);
    CL_Point get_offset() const;
    void set_offset(const CL_Point& pt);
    void SetMapEditor(MapEditorState* state) { m_state = state; }
private:
    void on_render(CL_GraphicContext& gc, const CL_Rect& rect);
    bool on_click(const CL_InputEvent& event);
    
    CL_Sprite m_image;
    CL_Point m_offset;
    std::string m_name;
    AddTileOperation m_op;
    AddTilesOperation m_group_op;
    MapEditorState* m_state;
    bool m_selection;
};


}
#endif // TILESELECTOR_H
