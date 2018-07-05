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

#include "MOView.h"
#include <ClanLib/core.h>
#include <ClanLib/display.h>

#ifdef SR2_EDITOR

namespace StoneRing  { 

MOView::MOView(clan::GUIComponent* parent):clan::GUIComponent(parent)
{
    //set_type_name("MOView");
    func_render().set(this, &MOView::on_render);
    //func_process_message().set(this, &MOView::on_process_message);
}

MOView::~MOView()
{

}

void MOView::SetSprite ( clan::Sprite sprite )
{
    m_sprite = sprite;
    request_repaint();
}

clan::Sprite MOView::GetSprite() const
{
    return m_sprite;
}


void MOView::SetSize ( const clan::Size& size )
{
    m_sprite_size = size;
}



void MOView::render_background(clan::Canvas &gc, const clan::Rect& clip)
{
    clan::Colorf dark(0.5f,0.5f,0.5f);
    clan::Colorf light(0.9f,0.9f,0.9f);
    clan::Point top_left = clip.get_top_left();
    int tilesx,tilesy;
    tilesx = clip.get_width() / 8;
    tilesy = clip.get_height() / 8;
    
    for(int x=0;x<tilesx;x++){
        for(int y=0;y<tilesy;y++){
            clan::Pointf point(top_left.x + x * 8, top_left.y + y * 8);
            clan::Sizef size(8.0,8.0);
            clan::Rectf rect(point,size);
            clan::Colorf color;
            if((y * tilesx + x + y) %2)
                color = dark;
            else
                color = light;
            gc.fill_rect(rect,color);
        }
    }
}

void MOView::on_render(clan::Canvas &gc, const clan::Rect &clip_rect)
{
    render_background(gc,clip_rect);
    if(!m_sprite.is_null()){
        clan::Pointf point((clip_rect.get_width()-m_sprite.get_width())/2.0,
                        (clip_rect.get_height()-m_sprite.get_height())/2.0);
        point += clip_rect.get_top_left();
        m_sprite.draw(gc,point.x,point.y);
        
        clan::Rectf square(clan::Pointf(clip_rect.get_top_left().x + (clip_rect.get_width()-m_sprite_size.width*32)/2.0,
                                   clip_rect.get_top_left().y +   (clip_rect.get_height()-m_sprite_size.height*32)/2.0 + m_sprite.get_height() - (m_sprite_size.height*32)),
                       clan::Sizef(m_sprite_size.width*32,m_sprite_size.height*32));
        gc.draw_box(square,clan::Colorf::yellow);
    }
}

void MOView::on_process_message(clan::GUIMessage &message)
{
}

void MOView::Clear()
{
    m_sprite = clan::Sprite();
    request_repaint();
}


}

#endif