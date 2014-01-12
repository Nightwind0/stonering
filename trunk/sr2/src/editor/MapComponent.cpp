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

MapComponent::MapComponent(clan::GUIComponent* parent):clan::GUIComponent(parent),
m_gradient(clan::Colorf(0.5f,0.5f,0.5f),clan::Colorf(1.0f,1.0f,1.0f),clan::Colorf(1.0f,1.0f,1.0f),clan::Colorf(1.0f,1.0f,1.0f))
{
    //set_type_name("SR2Map");
    func_render().set(this, &MapComponent::on_render);
    //func_process_message().set(this, &MapComponent::on_process_message);
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
    m_origin = clan::Point(0,0);
    request_repaint();
}

void MapComponent::close_level()
{
    m_pLevel = shared_ptr<Level>();
    m_origin = clan::Point(0,0);
    request_repaint();
}


void MapComponent::load_level ( const std::string& name )
{
	m_pLevel = shared_ptr<Level>(new Level());
	m_pLevel->LoadFromFile ( name, false );
    m_origin = clan::Point(0,0);
    request_repaint();
   //m_pLevel->Invoke();
}

void MapComponent::draw_grid(clan::Canvas &gc, const clan::Rect& rect)
{
    clan::Point top_left = rect.get_top_left();
    for(int x = 0; x < m_pLevel->GetWidth(); x++){
        clan::Pointf start(top_left.x + x * 32,top_left.y);
        clan::Pointf end(top_left.x+x*32, top_left.y + rect.get_height());
        gc.draw_line(start,end,clan::Colorf(0.5f,0.5f,0.5f));
    }
    for(int y=0;y<m_pLevel->GetHeight();y++){
        clan::Pointf start(top_left.x,top_left.y+y*32);
        clan::Pointf end(top_left.x+ rect.get_width(),top_left.y+y*32);
        gc.draw_line(start,end,clan::Colorf(0.5f,0.5f,0.5f));
    }
}

void MapComponent::draw_level(clan::Canvas &gc, const clan::Rect& screen_rect)
{
    gc.push_scale(m_scale,m_scale);
    clan::Rect screen(clan::Point(0,0),clan::Size(screen_rect.get_width()/m_scale,screen_rect.get_height()/m_scale));
    clan::Rect area = clan::Rectf(to_float(m_origin),clan::Sizef(m_pLevel->GetWidth()*32,m_pLevel->GetHeight()*32));
    gc.fill_rect(area,m_gradient);   
    gc.draw_box(area,clan::Colorf(0,0,0));
    draw_grid(gc,area);
    //clan::Rectf source(to_float(-m_origin)/m_scale,clan::Sizef(screen_rect.get_width()/m_scale,screen_rect.get_height()/m_scale));
	clan::Rectf source(to_float(-m_origin),clan::Sizef(screen_rect.get_width()/m_scale,screen_rect.get_height()/m_scale));
    if(m_pLevel){
        m_pLevel->Draw(source,screen,gc,m_show_mos,m_show_mos);      
    }
   gc.pop_modelview();
}


void MapComponent::draw_rubber_band(clan::Canvas& gc)
{
    gc.fill_rect(m_rubber_band,clan::Colorf(0.0f,1.0f,0.0f,0.6f));
}

void MapComponent::on_render(clan::Canvas &gc, const clan::Rect &clip_rect)
{
#if 0 
    clan::FrameBuffer framebuffer(gc);
    //clan::Texture texture(gc,clip_rect.get_width(),clip_rect.get_height(),cl_rgba8);    
    clan::Texture texture(gc,"Media/Images/Portrait/PortraitShadow.png");
    framebuffer.attach_color_buffer(0,texture);
    gc.set_frame_buffer(framebuffer);
    clan::Draw::fill(gc,clan::Rectf(clan::Pointf(0.0f,0.0f),clan::Sizef(clip_rect.get_width(),clip_rect.get_height())),clan::Colorf(0.5f,0.0f,0.0f));
    draw_level(gc,clip_rect);
    
    gc.reset_frame_buffer();
    
    //gc.set_texture(0, texture);
    //draw_texture(mGC, clan::Rect(0, 0, mGC.get_width(), mGC.get_height()), clan::Colorf::white, clan::Rectf(0, 0, 1, 1));
    //clan::Draw::texture(gc,clan::Rectf(clan::Pointf(0,0),  clan::Sizef(clip_rect.get_width(),clip_rect.get_height() )));    
 
    //gc.reset_texture(0);
    //gc.set_texture(0,texture);
    //clan::Draw::texture(gc,clan::Rectf(clan::Pointf(0,0),  clan::Sizef(clip_rect.get_width(),clip_rect.get_height() )));
   //clan::Draw::fill(gc,clip_rect,clan::Colorf(0.0f,0.0f,0.0f));
    //clan::Image image(gc,texture,clip_rect);
    //image.draw(gc,0,0);
    clan::SpriteDescription desc;
    desc.add_frame(texture);
    clan::Sprite sprite(gc,desc);
    sprite.draw(gc,0,0);
#else
    //gc.push_cliprect(clip_rect);
    // Draw a backdrop
   // clan::Draw::fill(gc,get_geometry(),clan::Colorf(0.5f,0.5f,0.5f));
    gc.push_cliprect(component_to_window_coords(clip_rect));
    if(m_pLevel)
        draw_level(gc,clip_rect);
    if(m_show_band)
        draw_rubber_band(gc);
    gc.pop_cliprect();
#endif
}

bool MapComponent::on_process_message ( clan::GUIMessage& message )
{
    //request_repaint();
	return true;
}

clan::Point MapComponent::get_center() const 
{
    return clan::Rect(clan::Point(0,0),clan::Size(m_pLevel->GetWidth()*32,m_pLevel->GetHeight()*32)).get_center();
}

clan::Pointf MapComponent::to_float(const clan::Point& pt) const 
{
    return clan::Pointf(pt.x,pt.y);
}

clan::Size MapComponent::get_draw_size() const
{
    return clan::Size(m_pLevel->GetWidth() * 32 * m_scale,m_pLevel->GetHeight() * 32 * m_scale);
}


void MapComponent::writeXML ( const std::string& filename)
{
    if(m_pLevel){
        m_pLevel->WriteXML(filename,true);
    }
}


clan::Point MapComponent::level_to_screen ( const clan::Point& level, const clan::Point& screen_center ) const
{
    clan::Pointf v(level.x,level.y);
    clan::Pointf center = to_float(get_center());
    clan::Pointf origin(center.x,center.y);
    
    
    clan::Pointf v2 = clan::Pointf(v - center);
    v2 *= m_scale;
    v2 += center;
    
    return clan::Point(v2.x,v2.y);
}

clan::Point MapComponent::screen_to_level ( const clan::Point& screen, const clan::Point& screen_center ) const
{
    
    //clan::Pointf v = to_float(screen) - to_float(m_origin);
    //return clan::Point(v.x / m_scale,v.y / m_scale);
    return clan::Point(screen.x/m_scale,screen.y/m_scale) - m_origin;
    /*clan::Pointf s = to_float((screen) - (screen_center));
    s -= to_float(m_origin);
    s *= m_scale;
    s += to_float(get_center());
    return clan::Point(s.x,s.y);*/
}

bool MapComponent::valid_location ( const clan::Point& screen, const clan::Point& screen_center ) const
{
    if(m_pLevel == NULL) return false;
    
    clan::Point level = screen_to_level(screen,screen_center);
    clan::Point tile = level / 32;
    if(tile.x < 0 || tile.y < 0 ||
        tile.x >= m_pLevel->GetWidth() || tile.y >= m_pLevel->GetHeight()){
        return false;
    }
    
    return true;
}


void MapComponent::set_rubber_band ( const clan::Rect& rect )
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


void MapComponent::show_floaters ( bool on )
{
    if(m_pLevel){
        if(on)
            m_pLevel->AddTileVisitor(&m_floater_drawer);
        else
            m_pLevel->RemoveTileVisitor(&m_floater_drawer);
    }
}

void MapComponent::show_zorder ( bool on )
{
    if(m_pLevel){
        if(on)
            m_pLevel->AddTileVisitor(&m_zorder_drawer);
        else
            m_pLevel->RemoveTileVisitor(&m_zorder_drawer);
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

std::list<MappableObject*> MapComponent::get_mos_at( const clan::Point& point )
{
    return m_pLevel->GetMappableObjectsAt(point);
}




}

#endif