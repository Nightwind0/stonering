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


#ifndef MOVIEW_H
#define MOVIEW_H

#include <ClanLib/gui.h>

namespace StoneRing { 

class MOView: public clan::GUIComponent
{
public:
    MOView(clan::GUIComponent* parent);
    virtual ~MOView();
    void SetSprite(clan::Sprite sprite);
    void SetSize(const clan::Size& size);
    clan::Sprite GetSprite() const;
    void Clear();
private:
    void on_render(clan::Canvas &gc, const clan::Rect &clip_rect);
    void on_process_message(clan::GUIMessage &message);    
    void render_background(clan::Canvas &gc, const clan::Rect& clip);
    
    clan::Sprite m_sprite;
    clan::Size m_sprite_size;
};

}
#endif // MOVIEW_H
