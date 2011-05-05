/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

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

#include "MainMenuTargetingState.h"
#include "MainMenuState.h"

using StoneRing::MainMenuTargetingState;
using StoneRing::MainMenuState;
using StoneRing::IApplication;

MainMenuTargetingState::MainMenuTargetingState(MainMenuState& parent):m_parent(parent),m_bDone(false)
{
}
MainMenuTargetingState::~MainMenuTargetingState()
{
}

void MainMenuTargetingState::Init(bool PartyMode)
{
    m_parent.SelectionStart();
    m_bPartyMode = PartyMode;
    if(PartyMode){
	m_parent.SelectAllCharacters();
    }
}


bool MainMenuTargetingState::IsDone() const
{
    return m_bDone;
}
    // Handle joystick / key events that are processed according to mappings
void MainMenuTargetingState::HandleButtonUp(const StoneRing::IApplication::Button& button)
{
    if(button == IApplication::BUTTON_CONFIRM)
    {
	m_parent.SelectionFinish();
	m_bDone = true;
    }
    else if(button == IApplication::BUTTON_CANCEL)
    {
	m_parent.SelectionCancel();
	m_bDone = true;
    }
}
void MainMenuTargetingState::HandleAxisMove(const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos)
{
    if(m_bPartyMode) return; 
    
    if(dir == IApplication::AXIS_UP)
    {
	m_parent.SelectCharacterUp();
    }
    else if(dir == IApplication::AXIS_DOWN)
    {
	m_parent.SelectCharacterDown();
    }
}
    
void MainMenuTargetingState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC)
{
}

bool MainMenuTargetingState::LastToDraw() const
{
	return false;
}

bool MainMenuTargetingState::DisableMappableObjects() const
{
	return true;
}

void MainMenuTargetingState::MappableObjectMoveHook()
{
}

void MainMenuTargetingState::Start()
{
    m_bDone = false;
}

void MainMenuTargetingState::SteelInit      (SteelInterpreter *)
{

}

void MainMenuTargetingState::SteelCleanup   (SteelInterpreter *)
{
}

void MainMenuTargetingState::Finish()
{
}