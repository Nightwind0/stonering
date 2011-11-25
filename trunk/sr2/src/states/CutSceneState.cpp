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


#include "CutSceneState.h"

namespace StoneRing {

CutSceneState::CutSceneState()
{

}

CutSceneState::~CutSceneState()
{

}

bool CutSceneState::DisableMappableObjects() const
{
    return false;
}

void CutSceneState::Draw ( const CL_Rect& screenRect, CL_GraphicContext& GC )
{

}

void CutSceneState::HandleAxisMove ( const StoneRing::IApplication::Axis& axis, const StoneRing::IApplication::AxisDirection dir, float pos )
{
}

void CutSceneState::HandleButtonDown ( const StoneRing::IApplication::Button& button )
{
}

void CutSceneState::HandleButtonUp ( const StoneRing::IApplication::Button& button )
{
}

void CutSceneState::Start()
{
    m_bDone = false;
}

void CutSceneState::Finish()
{

}

void CutSceneState::SteelInit ( SteelInterpreter* )
{
}

void CutSceneState::SteelCleanup ( SteelInterpreter* )
{
}

bool CutSceneState::IsDone() const
{
    return m_bDone;
}

bool CutSceneState::LastToDraw() const
{
    return false;
}

void CutSceneState::MappableObjectMoveHook()
{

}



}