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

class MOView: public CL_GUIComponent
{
public:
    MOView(CL_GUIComponent* parent);
    virtual ~MOView();
    void SetSprite(CL_Sprite sprite);
    void SetSize(const CL_Size& size);
    CL_Sprite GetSprite() const;
private:
    void on_render(CL_GraphicContext &gc, const CL_Rect &clip_rect);
    void on_process_message(CL_GUIMessage &message);    
    void render_background(CL_GraphicContext &gc, const CL_Rect& clip);
    
    CL_Sprite m_sprite;
    CL_Size m_sprite_size;
};

}
#endif // MOVIEW_H
