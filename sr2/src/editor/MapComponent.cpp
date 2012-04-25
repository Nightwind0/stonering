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

namespace StoneRing { 

MapComponent::MapComponent(CL_GUIComponent* parent):CL_GUIComponent(parent)
{
    set_type_name("SR2Map");
    func_render().set(this, &MapComponent::on_render);
    func_process_message().set(this, &MapComponent::on_process_message);
    m_scale = 1.0f;
}

MapComponent::~MapComponent()
{

}

void MapComponent::load_level ( const std::string& name )
{
   m_pLevel.reset(new Level());
   m_pLevel->Load ( name, IApplication::GetInstance()->GetResources() );
   m_origin = CL_Point(0,0);
   //m_pLevel->Invoke();
}

void MapComponent::draw_level(CL_GraphicContext &gc, const CL_Rect& screen_rect)
{
    CL_Draw::fill(gc,screen_rect,CL_Colorf(1.0f,1.0f,1.0f));    
    gc.push_scale(m_scale,m_scale);
    CL_Rect source(screen_to_level(m_origin),CL_Size(screen_rect.get_width()*1.0f/m_scale,screen_rect.get_height()*1.0f/m_scale));
    if(m_pLevel){
        m_pLevel->Draw(source,screen_rect,gc);
    }
   gc.pop_modelview();
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
    draw_level(gc,clip_rect);
    //gc.pop_cliprect();
#endif
}

void MapComponent::on_process_message ( CL_GUIMessage& message )
{
    //request_repaint();
}

CL_Point MapComponent::level_to_screen ( const CL_Point& level ) const
{
    CL_Pointf v(level.x,level.y);
    CL_Pointf v2 = v / m_scale;
    return CL_Point(v2.x,v2.y);
}

CL_Point MapComponent::screen_to_level ( const CL_Point& screen ) const
{
    CL_Pointf v2(screen.x,screen.y); //- cp; // get a vector to v relative to the centerpoint
    CL_Pointf v2_scaled = v2 * m_scale; // scale the cp-relative-vector
    return CL_Point(v2_scaled.x,v2_scaled.y); //+ cp; // translate the scaled vector back
}


}