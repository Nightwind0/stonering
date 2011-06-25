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

#ifndef EXPERIENCESTATE_H
#define EXPERIENCESTATE_H

#include <State.h>

#include "GraphicsManager.h"

namespace StoneRing{

class Character;    
    
class ExperienceState : public StoneRing::State
{
public:
    ExperienceState();
    virtual ~ExperienceState();
    void Init();
    void AddCharacter(Character* pCharacter, int xp_gained, int old_level, int sp);
    virtual void Finish();
    virtual void Start();
    virtual void MappableObjectMoveHook();
    virtual bool DisableMappableObjects() const;
    virtual bool LastToDraw() const;
    virtual void Draw(const CL_Rect& screenRect, CL_GraphicContext& GC);
    virtual bool IsDone() const;
    virtual void HandleButtonUp(const StoneRing::IApplication::Button& button);

    virtual void SteelInit(SteelInterpreter* );
    virtual void SteelCleanup(SteelInterpreter* );
private:
    double getTNL(int level);
    double getLNT(int xp);
    struct Char{
	Character* m_pCharacter;
	int m_nXP;
	int m_nOldLevel;
        int m_nSP;
    };
    CL_Pointf m_offset;
    CL_Pointf m_portraitOffset;
    CL_Pointf m_textOffset;
    CL_Image m_overlay;
    CL_Image m_xpbar;
    CL_Rectf m_barRect;
    CL_Pointf m_barPoint;
    CL_Gradient m_barGradient;
    StoneRing::Font m_characterFont;
    StoneRing::Font m_xpFont;
    StoneRing::Font m_spFont;
    StoneRing::Font m_oldlevelFont;
    StoneRing::Font m_levelFont;

    uint m_start_time;
    AstScript* m_pTNL;
    AstScript* m_pLNT;
    bool m_bDone;
    std::vector<Char> m_characters;
};
}
#endif // EXPERIENCESTATE_H
