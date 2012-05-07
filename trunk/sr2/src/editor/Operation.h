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


#ifndef OPERATION_H
#define OPERATION_H

#include <ClanLib/display.h>
#include "sr_defines.h"

namespace StoneRing {
    
    class Level;

class Operation
{
public:
    Operation();
    virtual ~Operation();
    enum OperationType {
        CLICK=1,
        DRAG=2,
        RUBBER_BAND=4,
        DOUBLE_CLICK=8
    };
    struct Data {
        int      m_mod_state;
        CL_Point m_level_pt;
        CL_Point m_level_end_pt;
    };
    void SetData(const Data& data){
        m_data = data;
    }
    virtual Operation* clone()=0;
    virtual int Type()const=0;
    virtual bool Execute(shared_ptr<Level> level)=0;
    virtual void Undo(shared_ptr<Level> level)=0;
protected:
    Data m_data;
};

}

#endif // OPERATION_H
