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


#ifndef SKILLTREESTATE_H
#define SKILLTREESTATE_H
#include "State.h"
#include "Menu.h"

namespace StoneRing { 

class SkillTreeState : public State, public Menu
{
public:
    SkillTreeState();
    virtual ~SkillTreeState();
    void Init(Character* pCharacter, bool buy);
    virtual bool IsDone() const;
    // Handle joystick / key events that are processed according to mappings
    virtual void HandleButtonUp(const IApplication::Button& button);
    virtual void HandleButtonDown(const IApplication::Button& button);
    virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);
    virtual bool DisableMappableObjects() const { return false; }
    virtual void MappableObjectMoveHook() {}
    
    virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
    virtual bool LastToDraw() const { return false; }; // Should we continue drawing more states?
    virtual bool DisableMappableObjects() { return true; }; // Should the app move the MOs?
    virtual void Start();
    virtual void Finish(); // Hook to clean up or whatever after being popped
    SkillTreeNode* GetSelectedSkillNode() const;
protected:
    virtual CL_Rectf get_rect();
    virtual void draw_option(int option, bool selected, float x, float y, CL_GraphicContext& gc);
    virtual int height_for_option(CL_GraphicContext& gc);
    virtual void process_choice(int selection);
    virtual int get_option_count();
private:
    enum eUseMode 
    {
        BUY,
        USE,
    };
    void fill_vector(std::list<SkillTreeNode*>::const_iterator begin, std::list<SkillTreeNode*>::const_iterator end);
    CL_Sprite m_arrow;
    CL_Sprite m_lock;
    CL_Image m_portrait_shadow;

    CL_Rectf m_description;
    CL_Rectf m_menu;
    CL_Rectf m_reqs;
    CL_Rectf m_path_rect;
    CL_Rectf m_char_rect;
    CL_Rectf m_cost_rect;
    CL_Pointf m_icon_point;
    CL_Pointf m_cost_point;
    CL_Pointf m_arrow_point;
    CL_Pointf m_lock_point;
    CL_Pointf m_skill_size;
    CL_Pointf m_name_offset;
    CL_Pointf m_portrait_offset;
    CL_Pointf m_char_name_pt;
    CL_Pointf m_char_sp_pt;
    CL_Pointf m_use_cost_pt;
    Font   m_desc_font;
    Font   m_option_font;
    Font   m_char_name_font;
    Font   m_char_sp_font;
    Font   m_char_mp_font;
    Font   m_char_bp_font;
    Font   m_selection_font;
    Font   m_obtained_font;
    Font   m_points_font;
    Font   m_unavailable_font;
    Font   m_not_enough_points_font;
    Font   m_unmet_reqs_font;
    Font   m_reqs_font;
    Font   m_unmet_reqs_desc_font;
    Font   m_path_font;
    Font   m_mp_cost;
    Font   m_bp_cost;
    bool   m_bDone;
    eUseMode m_eUse;
    Character* m_pChar;
    SkillTreeNode* m_pNode;
    SkillTreeNode* m_pSelectedNode;
    std::vector<SkillTreeNode*> m_skills;
};

}

#endif // SKILLTREESTATE_H
