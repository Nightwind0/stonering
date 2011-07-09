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


#ifndef STATUSBOX_H
#define STATUSBOX_H

#include "sr_defines.h"
#include "ICharacter.h"
#include "Character.h"

namespace StoneRing {

class StatusBox
{

public:
    StatusBox(const CL_Rectf& rect, const Font& stat_font, 
              const Font& stat_up_font, const Font& stat_down_font,
              const Font& stat_name_font
             );
    virtual ~StatusBox();
    
    void Draw(CL_GraphicContext& gc, bool draw_comparison,  Character * pChar, Equipment* pOldEquipment, Equipment * pEquipment);
private:
    Equipment::eSlot slot_for_equipment(Equipment* pEquipment);
    CL_Rectf m_rect;
    Font  m_stat_font;
    Font  m_stat_up_font;
    Font  m_stat_down_font;
    Font  m_stat_name_font;
    std::vector<ICharacter::eCharacterAttribute> m_stats;    
};

}

#endif // STATUSBOX_H
