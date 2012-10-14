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


#ifndef MAPCOMPONENT_H
#define MAPCOMPONENT_H

#include <ClanLib/gui.h>
#include "DebugTileVisitors.h"
#include "Level.h"

namespace StoneRing { 

class MapComponent : public CL_GUIComponent
{

public:
    MapComponent(CL_GUIComponent* parent);
    virtual ~MapComponent();
    void set_scale(float f){m_scale = f;}
    void set_origin(const CL_Point& point) { m_origin = point;}
    CL_Point get_origin() const { return m_origin; }
    float get_scale()const{return m_scale;}
    void load_level(const std::string& name);    
    void create_level(uint width, uint height);
    void close_level();
    void writeXML(const std::string&);
    CL_Size get_draw_size()const;
    CL_Point screen_to_level(const CL_Point& screen, const CL_Point& screen_center)const;
    CL_Point level_to_screen(const CL_Point& level, const CL_Point& screen_center)const;
    bool valid_location(const CL_Point& screen, const CL_Point& screen_center)const;
    void set_rubber_band(const CL_Rect& rect);
    void cancel_rubber_band();
    shared_ptr<Level> get_level() { return m_pLevel; }
    void show_direction_blocks(bool on);
    void show_hot(bool on);
    void show_mos(bool on);
    void show_pop(bool on);
    void show_floaters(bool on);
	void show_monster_region(bool on);
    MappableObject* get_mo_named(const std::string& name);
    std::list<MappableObject*> get_mos_at(const CL_Point& level_pt);
private:
    void on_render(CL_GraphicContext &gc, const CL_Rect &clip_rect);
    void on_process_message(CL_GUIMessage &message);
    void draw_grid(CL_GraphicContext&, const CL_Rect&);
    void draw_level(CL_GraphicContext &gc, const CL_Rect &rect);
    void draw_rubber_band(CL_GraphicContext &gc);
    CL_Point get_center()const;
    CL_Pointf to_float(const CL_Point&)const;
    CL_Point m_origin;
    TileDirectionBlockDrawer m_block_drawer;
    TileHotDrawer m_hot_drawer;
    TilePopsDrawer m_pops_drawer;
    TileFloaterDrawer m_floater_drawer;
	TileMonsterRegionDrawer m_region_drawer;
    bool m_show_mos;
    float m_scale;
    CL_Gradient m_gradient;
    bool m_show_band;
    CL_Rect m_rubber_band;
    shared_ptr<Level>  m_pLevel;
};


}
#endif // MAPCOMPONENT_H
