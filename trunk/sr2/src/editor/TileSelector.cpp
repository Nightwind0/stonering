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


#include "TileSelector.h"
#include "Operation.h"
#include "GraphicsManager.h"
#include <ClanLib/display.h>
#include "MapEditorState.h"
#include <Level.h>

#ifdef SR2_EDITOR

namespace StoneRing { 

void TileSelector::AddTileOperation::operator= ( const StoneRing::TileSelector::AddTileOperation& other )
{
    m_tilemap = other.m_tilemap;
    m_tile_pos = other.m_tile_pos;
}
    
    
Operation* TileSelector::AddTileOperation::clone()
{
    TileSelector::AddTileOperation * pOp = new TileSelector::AddTileOperation();
    *pOp = *this;
    return pOp;
}

bool TileSelector::AddTileOperation::Execute(shared_ptr<Level>  pLevel)
{
    CL_Point tilePos = m_data.m_level_end_pt;
    if(tilePos.x >= 0 && tilePos.y >= 0 && tilePos.x < pLevel->GetWidth() 
        && tilePos.y < pLevel->GetHeight()){
        Tile * pTile = new Tile;
        Tilemap * pTilemap =  new Tilemap;
        pTilemap->SetTilemap(GraphicsManager::GetTileMap(m_tilemap),m_tilemap,m_tile_pos.x,m_tile_pos.y);
        pTile->SetTileMap(pTilemap);
        pTile->SetPos(m_data.m_level_end_pt.x,m_data.m_level_end_pt.y);

        // Shift means add, without it means to replace
        if(m_data.m_mod_state & Operation::SHIFT){
            pLevel->AddTile(pTile);        
        }else {
            m_removed_tiles = pLevel->GetTilesAt(m_data.m_level_end_pt);
            while(NULL != pLevel->PopTileAtPos(m_data.m_level_end_pt));
            pLevel->AddTile(pTile);         
        }
        m_tile = pTile;
        return true;
    }else{
        return false;
    }
}


void TileSelector::AddTileOperation::Undo(shared_ptr<Level> pLevel)
{
    if(m_data.m_mod_state & Operation::SHIFT){
        Tile * pTile = pLevel->PopTileAtPos(m_data.m_level_end_pt);
        if(pTile) delete pTile;        
    }else{
        // First, delete the one we just added
        delete pLevel->PopTileAtPos(m_data.m_level_end_pt);
        // Then, put everything back
        for(std::list<Tile*>::const_iterator it = m_removed_tiles.begin(); it!= m_removed_tiles.end();
            it++){
            pLevel->AddTile(*it);
        }
    }
}
 
    
TileSelector::TileSelector(CL_GUIComponent* parent):CL_GUIComponent(parent)
{
    func_render().set(this, &TileSelector::on_render);
    func_input_released().set(this,&TileSelector::on_click);

    m_selection = false;
}

TileSelector::~TileSelector()
{

}

void TileSelector::SetTilemap ( CL_Sprite image, const std::string& name )
{
    m_name = name;
    m_image = image;    
    m_selection = false;
}

CL_Point TileSelector::get_offset() const 
{
    return m_offset;
}

void TileSelector::set_offset ( const CL_Point& pt )
{
    m_offset = pt;
}


void TileSelector::on_render ( CL_GraphicContext& gc, const CL_Rect& rect )
{
    gc.push_cliprect(component_to_window_coords(rect));
    CL_Draw::fill(gc,rect,CL_Colorf(0.2f,0.2f,0.2f));
    
    if(!m_image.is_null()){
        m_image.draw(gc,m_offset.x,m_offset.y);
    }
    
    if(m_selection){
        CL_Rectf box(CL_Pointf(m_op.GetPoint() * 32 + m_offset),CL_Sizef(32.0f,32.0f));
        CL_Draw::box(gc,box,CL_Colorf(0.0f,1.0f,1.0f));
    }
   
    
    gc.pop_cliprect();
}

bool TileSelector::on_click(const CL_InputEvent& event)
{
    if(event.id == CL_MOUSE_LEFT){
        CL_Point pt = event.mouse_pos - m_offset;
        pt /= 32;
        if(!m_image.is_null() && !(pt.x < 0 || pt.y < 0 || pt.x > m_image.get_size().width / 32 ||
            pt.y > m_image.get_size().height / 32)){
            m_op.SetPoint(pt);
            m_op.SetName(m_name);
            m_group_op.SetPoint(pt);
            m_group_op.SetName(m_name);
        
            m_state->SetOperation(Operation::CLICK,&m_op);
            m_state->SetOperation(Operation::CLICK | Operation::SHIFT,&m_op); // For adding instead of replacing
            m_state->SetOperation(Operation::CLICK | Operation::ALT,&m_op); // For adding a floater

            m_state->SetOperation(Operation::DRAG,&m_group_op);
            m_state->SetOperation(Operation::DRAG | Operation::SHIFT,&m_group_op);
            m_state->SetOperation(Operation::DRAG | Operation::ALT,&m_group_op);
            m_selection = true;
            request_repaint();
        }
    }
    return true;
}



}
#endif