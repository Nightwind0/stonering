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


#ifndef ITEMGETSINGLESTATE_H
#define ITEMGETSINGLESTATE_H

#include "GetState.h"
#include "Menu.h"



namespace StoneRing { 

class ItemGetSingleState : public GetState
{
public:
    ItemGetSingleState();
    virtual ~ItemGetSingleState();
	void SetItem(Item* pItem, int count);
protected:
	virtual void load();
	virtual SoundManager::Effect get_sound_effect()const;
	virtual std::string get_text()const;
	virtual GraphicsManager::Overlay get_overlay()const;
	virtual CL_Image get_icon()const;
private:
	Item* m_pItem;
	int m_count;
	std::string m_text;
};


}
#endif // ITEMGETSTATE_H
