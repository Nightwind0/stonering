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


#include "SoundManager.h"
#include "IApplication.h"
#include <ClanLib/sound.h>

using namespace StoneRing;

SoundManager* SoundManager::m_pInstance = NULL;

SoundManager::SoundManager()
{

}

SoundManager::~SoundManager()
{

}


void StoneRing::SoundManager::initialize()
{
   if(!m_pInstance)
       m_pInstance = new SoundManager();
}

void SoundManager::PlaySound ( const std::string& sound_name )
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_SoundBuffer sound(sound_name,&resources);
    sound.play();
}

void SoundManager::SetMusic ( const std::string& )
{

}


