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
#include <ClanLib/vorbis.h> 

using namespace StoneRing;

SoundManager* SoundManager::m_pInstance = NULL;

SoundManager::SoundManager()
{
    m_music_max = 0.5f;
    m_sound_vol = 0.5f;
    
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
    sound.set_volume(m_pInstance->m_sound_vol);
    sound.play();
}


void SoundManager::set_music ( CL_SoundBuffer song )
{
    if(!m_pInstance->m_session.is_null()){
        CL_FadeFilter fade(1.0f);
        fade.fade_to_volume(0.0f,500);
        m_pInstance->m_session.add_filter(fade);
    }
    m_buffer = song;
    
    m_pInstance->m_transition_timer.func_expired().set(m_pInstance,&SoundManager::onTransitionTimer);
    m_pInstance->m_transition_timer.start(500,false);
}

void SoundManager::SetMusic ( const std::string& music )
{
	if(!music.empty()){
		CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
		m_pInstance->set_music(CL_SoundBuffer(music, &resources));
	}
}

float SoundManager::GetSoundVolume()
{
    return m_pInstance->m_sound_vol;
}


void SoundManager::SetSoundVolume ( float vol )
{
    m_pInstance->m_sound_vol = vol;
}



void SoundManager::onTransitionTimer ( ) 
{
    if(!m_session.is_null())
        m_session.stop();
    m_session = m_buffer.prepare();
    CL_FadeFilter fade(0.0f);
    fade.fade_to_volume(1.0f,500);
    m_session.add_filter(fade);
    m_session.set_looping(true);
    m_session.set_volume(m_music_max);
    m_session.play();
}

void SoundManager::SetMusicVolume(float vol)
{
    m_pInstance->m_session.set_volume(vol * m_pInstance->m_music_max);
}

float SoundManager::GetMusicVolume()
{
   return m_pInstance->m_session.get_volume();
}

void SoundManager::PushMusic()
{
    m_pInstance->m_song_stack.push_front(m_pInstance->m_buffer);
}

void SoundManager::PopMusic()
{
   m_pInstance->set_music ( m_pInstance->m_song_stack.front() );
   m_pInstance->m_song_stack.pop_front();
}


void SoundManager::PlayEffect ( SoundManager::Effect effect )
{
    switch(effect){
        case EFFECT_CHANGE_OPTION:
            PlaySound("Sound/Option");
            break;
        case EFFECT_BAD_OPTION:
            PlaySound("Sound/Buzz");
            break;
        case EFFECT_SELECT_OPTION:
            PlaySound("Sound/Select");
            break;
        case EFFECT_REWARD:
            PlaySound("Sound/Reward");
            break;
        case EFFECT_CANCEL:
            PlaySound("Sound/Cancel");
            break;
        case EFFECT_GOLD:
            PlaySound("Sound/Gold");
            break;
            
    }
}



