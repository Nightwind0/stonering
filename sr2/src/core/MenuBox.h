/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

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


#ifndef MENUBOX_H
#define MENUBOX_H

#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "sr_defines.h"

namespace StoneRing {
class MenuBox
{

public:
    MenuBox();
    virtual ~MenuBox();
    
    static void Draw(CL_GraphicContext& gc, const  CL_Rectf& rect, bool inset_shadow=true, CL_Pointf shadow = CL_Pointf(8.0f,8.0f));
private:
    
};
}
#endif // MENUBOX_H
