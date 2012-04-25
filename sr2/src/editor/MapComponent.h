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
#include "Level.h"
#include <ClanLib-2.3/ClanLib/Core/System/cl_platform.h>

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
    CL_Point screen_to_level(const CL_Point& screen)const;
    CL_Point level_to_screen(const CL_Point& level)const;
private:
    void on_render(CL_GraphicContext &gc, const CL_Rect &clip_rect);
    void on_process_message(CL_GUIMessage &message);
    void draw_level(CL_GraphicContext &gc, const CL_Rect &rect);
    
    CL_Point m_origin;
    float m_scale;
    
    shared_ptr<Level>  m_pLevel;
};


}
#endif // MAPCOMPONENT_H
