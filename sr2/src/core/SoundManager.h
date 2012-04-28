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


#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include <string>
#include <ClanLib/sound.h>
#include <ClanLib/vorbis.h>
#include <ClanLib/core.h>
#include <deque>

namespace StoneRing {

class SoundManager
{

public:
    
    enum Effect {
        EFFECT_CHANGE_OPTION,
        EFFECT_SELECT_OPTION,
        EFFECT_BAD_OPTION,
        EFFECT_REWARD,
        EFFECT_CANCEL,
        EFFECT_GOLD
    };
    
    static void         initialize();
    static void         PlayEffect(Effect effect);
    static void         PlaySound(const std::string&);
    static void         SetMusic(const std::string&);
    static void         SetMusicVolume(float vol);
    static void         SetMusicMaxVolume(float vol);
    static void         SetSoundVolume(float vol);
    static float        GetSoundVolume();
    static float        GetMusicVolume();
    static void         PushMusic();
    static void         PopMusic();
private:
    static SoundManager * m_pInstance;
    void set_music(CL_SoundBuffer song);
    void onTransitionTimer();
    SoundManager();
    virtual ~SoundManager();
    CL_SoundBuffer m_buffer;
    CL_SoundBuffer_Session m_session;
    CL_Timer m_transition_timer;
    float m_music_max;
    float m_sound_vol;
    std::deque<CL_SoundBuffer> m_song_stack;
};


}
#endif // SOUNDMANAGER_H
