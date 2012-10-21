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


#include "Operation.h"

#ifdef SR2_EDITOR

namespace StoneRing  {

Operation::Operation()
{

}

Operation::~Operation()
{

}

OperationGroup::OperationGroup()
{

}

OperationGroup::~OperationGroup()
{

}


bool OperationGroup::Execute ( shared_ptr< Level > level )
{
	bool any_true = false;
    for(std::list<Operation*>::iterator it = m_ops.begin();
        it != m_ops.end(); it++){
        if((*it)->Execute(level))
            any_true = true;
    }
    
    return any_true;
}

void OperationGroup::Undo ( std::tr1::shared_ptr< Level > level )
{
    for(std::list<Operation*>::iterator it = m_ops.begin();
        it != m_ops.end(); it++){
        (*it)->Undo(level);
    }
}

void OperationGroup::AddOperation ( const StoneRing::Operation::Data& data )
{
    Operation * pOp = create_suboperation();
    pOp->SetData(data);
    m_ops.push_back(pOp);
}


void OperationGroup::SetData ( const StoneRing::Operation::Data& data )
{
    StoneRing::Operation::SetData ( data );
    CL_Point tile_start = data.m_level_pt;
    CL_Point tile_end = data.m_level_end_pt;
    for(int x = tile_start.x; x <= tile_end.x; x++){
        for(int y = tile_start.y; y <= tile_end.y; y++){
            Operation::Data innerdata;
            innerdata.m_level_pt = innerdata.m_level_end_pt = CL_Point(x,y);
            innerdata.m_mod_state = data.m_mod_state;
            AddOperation(innerdata);
        }
    }
}





}


#endif
