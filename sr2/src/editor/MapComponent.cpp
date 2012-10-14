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


#include "MapComponent.h"

#ifdef SR2_EDITOR
namespace StoneRing { 

MapComponent::MapComponent(CL_GUIComponent* parent):CL_GUIComponent(parent),
m_gradient(CL_Colorf(0.5f,0.5f,0.5f),CL_Colorf(1.0f,1.0f,1.0f),CL_Colorf(1.0f,1.0f,1.0f),CL_Colorf(1.0f,1.0f,1.0f))
{
    set_type_name("SR2Map");
    func_render().set(this, &MapComponent::on_render);
    func_process_message().set(this, &MapComponent::on_process_message);
    m_scale = 1.0f;
    m_show_band = false;
    m_show_mos = false;
}

MapComponent::~MapComponent()
{

}

void MapComponent::create_level ( uint width, uint height )
{
    m_pLevel.reset(new Level(width,height));
    m_origin = CL_Point(0,0);
    request_repaint();
}

void MapComponent::close_level()
{
    m_pLevel = shared_ptr<Level>();
    m_origin = CL_Point(0,0);
    request_repaint();
}


void MapComponent::load_level ( const std::string& name )
{
	m_pLevel = shared_ptr<Level>(new Level());
	m_pLevel->LoadFromFile ( name );
    m_origin = CL_Point(0,0);
    request_repaint();
   //m_pLevel->Invoke();
}

void MapComponent::draw_grid(CL_GraphicContext &gc, const CL_Rect& rect)
{
    CL_Point top_left = rect.get_top_left();
    for(int x = 0; x < m_pLevel->GetWidth(); x++){
        CL_Pointf start(top_left.x + x * 32,top_left.y);
        CL_Pointf end(top_left.x+x*32, top_left.y + rect.get_height());
        CL_Draw::line(gc,start,end,CL_Colorf(0.5f,0.5f,0.5f));
    }
    for(int y=0;y<m_pLevel->GetHeight();y++){
        CL_Pointf start(top_left.x,top_left.y+y*32);
        CL_Pointf end(top_left.x+ rect.get_width(),top_left.y+y*32);
        CL_Draw::line(gc,start,end,CL_Colorf(0.5f,0.5f,0.5f));
    }
}

void MapComponent::draw_level(CL_GraphicContext &gc, const CL_Rect& screen_rect)
{
    gc.push_scale(m_scale,m_scale);
    CL_Rect screen(CL_Point(0,0),CL_Size(screen_rect.get_width()/m_scale,screen_rect.get_height()/m_scale));
    CL_Rect area = CL_Rectf(to_float(m_origin),CL_Sizef(m_pLevel->GetWidth()*32,m_pLevel->GetHeight()*32));
    CL_Draw::gradient_fill(gc,area,m_gradient);   
    CL_Draw::box(gc,area,CL_Colorf(0,0,0));
    draw_grid(gc,area);
    //CL_Rectf source(to_float(-m_origin)/m_scale,CL_Sizef(screen_rect.get_width()/m_scale,screen_rect.get_height()/m_scale));
	CL_Rectf source(to_float(-m_origin),CL_Sizef(screen_rect.get_width()/m_scale,screen_rect.get_height()/m_scale));
    if(m_pLevel){
        m_pLevel->Draw(source,screen,gc,false);
        if(m_show_mos){
            m_pLevel->DrawMappableObjects(source,screen,gc,false,true);          
        }
        m_pLevel->Draw(source,screen,gc,true);          
    }
   gc.pop_modelview();
}


void MapComponent::draw_rubber_band(CL_GraphicContext& gc)
{
    CL_Draw::fill(gc,m_rubber_band,CL_Colorf(0.0f,1.0f,0.0f,0.6f));
}

void MapComponent::on_render(CL_GraphicContext &gc, const CL_Rect &clip_rect)
{
#if 0 
    CL_FrameBuffer framebuffer(gc);
    //CL_Texture texture(gc,clip_rect.get_width(),clip_rect.get_height(),cl_rgba8);    
    CL_Texture texture(gc,"Media/Images/Portrait/PortraitShadow.png");
    framebuffer.attach_color_buffer(0,texture);
    gc.set_frame_buffer(framebuffer);
    CL_Draw::fill(gc,CL_Rectf(CL_Pointf(0.0f,0.0f),CL_Sizef(clip_rect.get_width(),clip_rect.get_height())),CL_Colorf(0.5f,0.0f,0.0f));
    draw_level(gc,clip_rect);
    
    gc.reset_frame_buffer();
    
    //gc.set_texture(0, texture);
    //draw_texture(mGC, CL_Rect(0, 0, mGC.get_width(), mGC.get_height()), CL_Colorf::white, CL_Rectf(0, 0, 1, 1));
    //CL_Draw::texture(gc,CL_Rectf(CL_Pointf(0,0),  CL_Sizef(clip_rect.get_width(),clip_rect.get_height() )));    
 
    //gc.reset_texture(0);
    //gc.set_texture(0,texture);
    //CL_Draw::texture(gc,CL_Rectf(CL_Pointf(0,0),  CL_Sizef(clip_rect.get_width(),clip_rect.get_height() )));
   //CL_Draw::fill(gc,clip_rect,CL_Colorf(0.0f,0.0f,0.0f));
    //CL_Image image(gc,texture,clip_rect);
    //image.draw(gc,0,0);
    CL_SpriteDescription desc;
    desc.add_frame(texture);
    CL_Sprite sprite(gc,desc);
    sprite.draw(gc,0,0);
#else
    //gc.push_cliprect(clip_rect);
    // Draw a backdrop
   // CL_Draw::fill(gc,get_geometry(),CL_Colorf(0.5f,0.5f,0.5f));
    gc.push_cliprect(component_to_window_coords(clip_rect));
    if(m_pLevel)
        draw_level(gc,clip_rect);
    if(m_show_band)
        draw_rubber_band(gc);
    gc.pop_cliprect();
#endif
}

void MapComponent::on_process_message ( CL_GUIMessage& message )
{
    //request_repaint();
}

CL_Point MapComponent::get_center() const 
{
    return CL_Rect(CL_Point(0,0),CL_Size(m_pLevel->GetWidth()*32,m_pLevel->GetHeight()*32)).get_center();
}

CL_Pointf MapComponent::to_float(const CL_Point& pt) const 
{
    return CL_Pointf(pt.x,pt.y);
}

CL_Size MapComponent::get_draw_size() const
{
    return CL_Size(m_pLevel->GetWidth() * 32 * m_scale,m_pLevel->GetHeight() * 32 * m_scale);
}


void MapComponent::writeXML ( const std::string& filename)
{
    if(m_pLevel){
        m_pLevel->WriteXML(filename,true);
    }
}


CL_Point MapComponent::level_to_screen ( const CL_Point& level, const CL_Point& screen_center ) const
{
    CL_Pointf v(level.x,level.y);
    CL_Pointf center = to_float(get_center());
    CL_Pointf origin(center.x,center.y);
    
    
    CL_Pointf v2 = CL_Pointf(v - center);
    v2 *= m_scale;
    v2 += center;
    
    return CL_Point(v2.x,v2.y);
}

CL_Point MapComponent::screen_to_level ( const CL_Point& screen, const CL_Point& screen_center ) const
{
    
    //CL_Pointf v = to_float(screen) - to_float(m_origin);
    //return CL_Point(v.x / m_scale,v.y / m_scale);
    return CL_Point(screen.x/m_scale,screen.y/m_scale) - m_origin;
    /*CL_Pointf s = to_float((screen) - (screen_center));
    s -= to_float(m_origin);
    s *= m_scale;
    s += to_float(get_center());
    return CL_Point(s.x,s.y);*/
}

bool MapComponent::valid_location ( const CL_Point& screen, const CL_Point& screen_center ) const
{
    if(m_pLevel == NULL) return false;
    
    CL_Point level = screen_to_level(screen,screen_center);
    CL_Point tile = level / 32;
    if(tile.x < 0 || tile.y < 0 ||
        tile.x >= m_pLevel->GetWidth() || tile.y >= m_pLevel->GetHeight()){
        return false;
    }
    
    return true;
}


void MapComponent::set_rubber_band ( const CL_Rect& rect )
{
    m_rubber_band = rect;
    m_rubber_band.translate(-get_geometry().get_top_left());
    m_show_band = true;
}


void MapComponent::cancel_rubber_band()
{
    m_show_band = false;
    request_repaint();
}

void MapComponent::show_direction_blocks ( bool on )
{
    if(m_pLevel){
        if(on)
            m_pLevel->AddTileVisitor(&m_block_drawer);
        else
            m_pLevel->RemoveTileVisitor(&m_block_drawer);
    }
}

void MapComponent::show_hot ( bool on )
{
    if(m_pLevel){
        if(on)
            m_pLevel->AddTileVisitor (&m_hot_drawer);
        else
            m_pLevel->RemoveTileVisitor (&m_hot_drawer);
    }
}

void MapComponent::show_mos ( bool on )
{
    m_show_mos = on;
}

void MapComponent::show_pop ( bool on )
{
    if(m_pLevel){
        if(on)
            m_pLevel->AddTileVisitor(&m_pops_drawer);
        else
            m_pLevel->RemoveTileVisitor(&m_pops_drawer);
    }
}

void MapComponent::show_floaters ( bool on )
{
    if(m_pLevel){
        if(on)
            m_pLevel->AddTileVisitor(&m_floater_drawer);
        else
            m_pLevel->RemoveTileVisitor(&m_floater_drawer);
    }
}

void MapComponent::show_monster_region ( bool on )
{
	if(m_pLevel){
		if(on)
			m_pLevel->AddTileVisitor(&m_region_drawer);
		else
			m_pLevel->RemoveTileVisitor(&m_region_drawer);
	}
}

MappableObject* MapComponent::get_mo_named ( const std::string& name )
{
    return m_pLevel->GetMappableObjectByName(name);
}

std::list<MappableObject*> MapComponent::get_mos_at( const CL_Point& point )
{
    return m_pLevel->GetMappableObjectsAt(point);
}




}

#endif