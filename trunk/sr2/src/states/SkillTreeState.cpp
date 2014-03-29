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


#include "SkillTreeState.h"
#include "GraphicsManager.h"
#include "SoundManager.h"
#include "MenuBox.h"
#include <sstream>
#include <iomanip>
#include "Graphic.h"
#include "SkillGetState.h"

using namespace StoneRing;

SkillTreeState::SkillTreeState()
{

}

SkillTreeState::~SkillTreeState()
{

}


void SkillTreeState::Init ( StoneRing::Character* pCharacter, bool buy )
{
    m_pChar = pCharacter;
    m_pNode = NULL;

    if(buy) m_eUse = BUY;
    else m_eUse = USE;
    
    m_arrow = GraphicsManager::GetSprite(GraphicsManager::SKILL_TREE,"arrow");
    m_lock = GraphicsManager::GetSprite(GraphicsManager::SKILL_TREE,"lock");
    m_description = GraphicsManager::GetRect(GraphicsManager::SKILL_TREE,"desc");
    m_menu = GraphicsManager::GetRect(GraphicsManager::SKILL_TREE,"menu");
    m_reqs = GraphicsManager::GetRect(GraphicsManager::SKILL_TREE,"reqs");
    m_char_rect = GraphicsManager::GetRect(GraphicsManager::SKILL_TREE,"char");
    m_path_rect = GraphicsManager::GetRect(GraphicsManager::SKILL_TREE,"path");
    m_cost_rect = GraphicsManager::GetRect(GraphicsManager::SKILL_TREE,"cost");
    m_icon_point = GraphicsManager::GetPoint(GraphicsManager::SKILL_TREE,"icon");
    m_cost_point = GraphicsManager::GetPoint(GraphicsManager::SKILL_TREE,"points");
    m_arrow_point = GraphicsManager::GetPoint(GraphicsManager::SKILL_TREE,"arrow");
    m_lock_point = GraphicsManager::GetPoint(GraphicsManager::SKILL_TREE,"lock");
    m_skill_size = GraphicsManager::GetPoint(GraphicsManager::SKILL_TREE,"skill_size");
    m_name_offset = GraphicsManager::GetPoint(GraphicsManager::SKILL_TREE,"name");
    m_portrait_offset = GraphicsManager::GetPoint(GraphicsManager::SKILL_TREE,"portrait");
    m_char_name_pt = GraphicsManager::GetPoint(GraphicsManager::SKILL_TREE,"char_name");
    m_char_sp_pt =   GraphicsManager::GetPoint(GraphicsManager::SKILL_TREE,"char_sp");
    m_use_cost_pt = GraphicsManager::GetPoint(GraphicsManager::SKILL_TREE,"use_cost");
    
    
    m_desc_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"desc");
    m_obtained_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"Obtained");
    m_selection_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"Selection");
    m_points_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"EnoughPoints");
    m_not_enough_points_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"NotEnoughPoints");
    m_option_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"Option");
    m_unavailable_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"Unavailable");
    m_unmet_reqs_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"UnmetReqs");
    m_reqs_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"reqs");
    m_path_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"Path");
    m_char_name_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"char_name");
    m_char_sp_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"char_sp");
    m_char_bp_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"char_bp");
    m_char_mp_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"char_mp");
    m_mp_cost = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"MPCost");
    m_bp_cost = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"BPCost");
    m_unmet_reqs_desc_font = GraphicsManager::GetFont(GraphicsManager::SKILL_TREE,"unmet_reqs_desc");
   
    
    
    m_portrait_shadow = GraphicsManager::CreateImage("Overlays/MainMenu/portrait_shadow");

    Menu::Init();    
}

void SkillTreeState::fill_vector ( std::list< StoneRing::SkillTreeNode* >::const_iterator begin, std::list< StoneRing::SkillTreeNode* >::const_iterator end )
{
    // TODO: The awesome STL way...
    m_skills.clear();
    for(std::list<SkillTreeNode*>::const_iterator iter = begin; iter != end; iter++)
    {
        m_skills.push_back ( *iter );
    }
}

int SkillTreeState::get_option_count()
{
    return m_skills.size();
}

int SkillTreeState::height_for_option ( clan::Canvas& gc )
{
    return 38;
}

void SkillTreeState::Start()
{
    m_bDone = false;
    m_pNode = NULL;
    m_pSelectedNode = NULL;
    // Start with the base skills
    fill_vector(m_pChar->GetClass()->GetSkillTreeNodesBegin(),
                m_pChar->GetClass()->GetSkillTreeNodesEnd());
}

void SkillTreeState::draw_option ( int option, bool selected, float x, float y, clan::Canvas& gc )
{
    SkillRef* pSkillRef = m_skills[option]->GetRef();
    StoneRing::Font font;
    StoneRing::Font costFont;
        
    bool has_skill = m_pChar->HasSkill(pSkillRef->GetRef());
    bool can_afford = m_pChar->GetSP() >= m_skills[option]->GetSPCost();
    bool met_reqs = m_skills[option]->CanLearn(m_pChar) && ! has_skill;
    
    bool can_use = (pSkillRef->GetSkill()->GetType() == Skill::WORLD ||
            pSkillRef->GetSkill()->GetType() == Skill::BOTH);
    

    
    clan::Rectf option_rect(x,y,x+get_rect().get_width(),y + height_for_option(gc));     
   // clan::Draw::box(gc,option_rect,clan::Colorf(0.0f,0.0f,0.0f,0.1f));
    //gradient_rect.shrink(GraphicsManager::GetMenuInset().x,0);
    //gradient_rect.translate(GraphicsManager::GetMenuInset());
 
 
    if(m_eUse == BUY){
        if(has_skill){
            font = m_obtained_font;
        }else if(can_afford && met_reqs){
            font = m_option_font;
        }else if(!met_reqs){
            font = m_unmet_reqs_font;
        }else if(!can_afford)
            font = m_unavailable_font;  
    }else if(m_eUse == USE){
        if(has_skill){
            if(can_use){
                font = m_obtained_font;
            }else{
                font = m_unavailable_font;
            }
        }else {
            font = m_unavailable_font;
        }
    }
    
    if(selected)
        font = m_selection_font;
    
    pSkillRef->GetSkill()->GetIcon().draw(gc,x+m_icon_point.x,y+m_icon_point.y);   


    if(m_skills[option]->GetSubSkillsBegin() != m_skills[option]->GetSubSkillsEnd())
    {
        // There are subskills, so draw the arrow
        m_arrow.draw(gc, x + m_arrow_point.x, y + m_arrow_point.y);
    }


    
    font.draw_text(gc, x + m_name_offset.x, 
                   y +  m_name_offset.y,m_skills[option]->GetRef()->GetRef(),Font::TOP_LEFT);
    
}


clan::Rectf SkillTreeState::get_rect()
{
    clan::Rect menu = m_menu;
    menu.set_width( menu.get_width() - (GraphicsManager::GetMenuInset().x * 2) );
    menu.set_height ( menu.get_height() - (GraphicsManager::GetMenuInset().y * 2) );
    menu.translate(GraphicsManager::GetMenuInset());
    return menu;
}

void SkillTreeState::Draw ( const clan::Rect& screenRect, clan::Canvas& GC )
{
    //MenuBox::Draw(GC,screenRect);
    GC.fill_rect(screenRect,clan::Colorf::black);
    clan::Rectf box = m_menu;
    MenuBox::Draw(GC,m_menu,false, kEmptyPoint);
    MenuBox::Draw(GC,m_path_rect,false, kEmptyPoint);
    MenuBox::Draw(GC,m_description,false, kEmptyPoint);
    MenuBox::Draw(GC,m_char_rect,false,kEmptyPoint);
    MenuBox::Draw(GC,m_cost_rect,false,kEmptyPoint);
    
    
    std::deque<SkillTreeNode*> skillStack;
    std::ostringstream pathDesc;
    
    SkillTreeNode * pCurrentNode = m_pNode;
    if(pCurrentNode == NULL){
        pathDesc << "Basic skills";
    }
    
    while(pCurrentNode != NULL){
        skillStack.push_front(pCurrentNode);
        pCurrentNode = pCurrentNode->GetParent();
    }
    
    for(std::deque<SkillTreeNode*>::const_iterator iter = skillStack.begin();
        iter != skillStack.end(); iter++)
    {
        pathDesc << (*iter)->GetRef()->GetRef();
        std::deque<SkillTreeNode*>::const_iterator next = iter;
        next++;
        if(next != skillStack.end())
            pathDesc << " > ";
    }
    
    clan::Rect path_rect = m_path_rect;
    path_rect.shrink ( GraphicsManager::GetMenuInset().x, GraphicsManager::GetMenuInset().y );
    draw_text(GC, m_path_font, path_rect, pathDesc.str());  
    Menu::Draw( GC );
    
    // Draw portrait
    // shadow first
    m_portrait_shadow.draw(GC,m_portrait_offset.x, m_portrait_offset.y);
    m_pChar->GetPortrait(Character::PORTRAIT_DEFAULT).draw(GC,m_portrait_offset.x,m_portrait_offset.y);
    
    // Now draw description
    SkillTreeNode * pNode = m_skills[ get_current_choice() ];
    Skill * pSkill = pNode->GetRef()->GetSkill();
    SkillRef* pSkillRef = pNode->GetRef();
    bool has_skill = m_pChar->HasSkill(pSkillRef->GetRef());    
    bool met_reqs = pNode->CanLearn(m_pChar) && ! has_skill;
    bool can_afford = m_pChar->GetSP() >= pNode->GetSPCost();
    
    bool can_use = (pSkillRef->GetSkill()->GetType() == Skill::WORLD ||
            pSkillRef->GetSkill()->GetType() == Skill::BOTH);    
    
    clan::Rect desc_rect = m_description;
    desc_rect.shrink ( GraphicsManager::GetMenuInset().x, GraphicsManager::GetMenuInset().y );
    draw_text(GC, m_desc_font, desc_rect, pSkill->GetDescription()); 
    
    std::ostringstream cost_stream;
    cost_stream << "Uses: ";
    
    if(pSkillRef->GetSkill()->GetBPCost()){
        cost_stream << std::setw(5) << pSkillRef->GetSkill()->GetBPCost() << ' ' << "BP";
        if(m_eUse != USE || m_pChar->GetAttribute(ICharacter::CA_BP) > pSkillRef->GetSkill()->GetBPCost()){
            m_bp_cost.draw_text(GC,m_use_cost_pt.x,m_use_cost_pt.y, cost_stream.str(),Font::TOP_LEFT);
        }else{
            m_not_enough_points_font.draw_text(GC,m_use_cost_pt.x,m_use_cost_pt.y, cost_stream.str(),Font::TOP_LEFT);
        }
    }else if(pSkillRef->GetSkill()->GetMPCost()){
        cost_stream << std::setw(5) << pSkillRef->GetSkill()->GetMPCost() << ' ' << "MP";
        if(m_eUse != USE || m_pChar->GetAttribute(ICharacter::CA_MP) > pSkillRef->GetSkill()->GetMPCost()){       
            m_mp_cost.draw_text(GC,m_use_cost_pt.x,m_use_cost_pt.y, cost_stream.str(), Font::TOP_LEFT);
        }else{
            m_not_enough_points_font.draw_text(GC,m_use_cost_pt.x,m_use_cost_pt.y, cost_stream.str(), Font::TOP_LEFT);
        }
    }
    
    if(has_skill){
        m_points_font.draw_text(GC,m_cost_point.x,m_cost_point.y,"Learned",Font::TOP_LEFT);
    }else{
        Font costFont;
        std::ostringstream cost_stream;
        cost_stream << "Cost: " << std::setw(5) << pNode->GetSPCost() << ' ' << "SP";
        if(!can_afford)
            costFont = m_not_enough_points_font;
        else
            costFont = m_points_font;
        costFont.draw_text(GC,m_cost_point.x,m_cost_point.y, cost_stream.str(), Font::TOP_LEFT);
    }    
    
    
    m_char_name_font.draw_text(GC,m_char_name_pt.x,m_char_name_pt.y, m_pChar->GetName());
    m_char_sp_font.draw_text(GC,m_char_sp_pt.x,m_char_sp_pt.y, "SP " +IntToString(m_pChar->GetLerpSP()));
    if(m_eUse == USE){
        m_char_mp_font.draw_text(GC,m_char_sp_pt.x,m_char_sp_pt.y + m_char_sp_font.get_font_metrics(GC).get_height(),
                             "MP " + IntToString(m_pChar->GetAttribute(ICharacter::CA_MP)));
        m_char_bp_font.draw_text(GC,m_char_sp_pt.x,m_char_sp_pt.y + m_char_sp_font.get_font_metrics(GC).get_height() 
                            + m_char_mp_font.get_font_metrics(GC).get_height(),
                             "BP " + IntToString(m_pChar->GetAttribute(ICharacter::CA_BP)));
    }
    
    
    clan::Rectf req_box = m_reqs;

    req_box.shrink(GraphicsManager::GetMenuInset().x,GraphicsManager::GetMenuInset().y);
    //req_box.translate(GraphicsManager::GetMenuInset());
    std::ostringstream reqs;
    if(pNode->GetParent())
    {
        reqs << "Requires " << pNode->GetParent()->GetRef()->GetRef() << '\n';
    }
    if(pNode->GetMinLevel())
    {
        reqs << "Minimum level " << pNode->GetMinLevel() << '\n';
    }
    reqs << pNode->GetRequirements();

    MenuBox::Draw(GC,m_reqs,false);
 
    if(!met_reqs){       
        draw_text(GC,m_unmet_reqs_desc_font,req_box,reqs.str());
    }else { 
        draw_text(GC,m_reqs_font,req_box,reqs.str());
    }
    

    
}

bool SkillTreeState::IsDone() const
{
    return m_bDone;
}

void SkillTreeState::HandleButtonUp ( const StoneRing::IApplication::Button& button )
{
    switch(button) 
    {
        case IApplication::BUTTON_CANCEL:
            SoundManager::PlayEffect(SoundManager::EFFECT_CANCEL);
            m_bDone = true;
            break;
        case IApplication::BUTTON_CONFIRM:
            Menu::Choose();
            break;
        case IApplication::BUTTON_R:
            // TODO: Load the children of selected node if any
            break;
        case IApplication::BUTTON_L:
            // Return to the parent of this node
            break;
            
            
    }
    
}

void SkillTreeState::Finish()
{

}

void SkillTreeState::HandleAxisMove ( const StoneRing::IApplication::Axis& axis, const StoneRing::IApplication::AxisDirection dir, float pos )
{
    if(axis == IApplication::AXIS_VERTICAL)
    {
        if(dir == IApplication::AXIS_DOWN)
            Menu::SelectDown();
        else if(dir == IApplication::AXIS_UP)
            Menu::SelectUp();
    }
    else if(axis == IApplication::AXIS_HORIZONTAL)
    {
        if(dir == IApplication::AXIS_RIGHT)
        {
            // Are there subskills?
            if(m_skills[get_current_choice()]->GetSubSkillsBegin() !=
                m_skills[get_current_choice()]->GetSubSkillsEnd())
            {
                m_pNode = m_skills[get_current_choice()];
                fill_vector(m_skills[get_current_choice()]->GetSubSkillsBegin(),
                    m_skills[get_current_choice()]->GetSubSkillsEnd());
                PushMenu();
                SoundManager::PlayEffect(StoneRing::SoundManager::EFFECT_CHANGE_OPTION);
            }else {
                SoundManager::PlayEffect(StoneRing::SoundManager::EFFECT_BAD_OPTION);
            }
        }
        else if(dir == IApplication::AXIS_LEFT)
        {
            if(m_pNode)
            {
                m_pNode = m_pNode->GetParent();
                if(m_pNode)
                    fill_vector(m_pNode->GetSubSkillsBegin(),m_pNode->GetSubSkillsEnd());
                else
                {
                     // Back to the base skills
                    fill_vector(m_pChar->GetClass()->GetSkillTreeNodesBegin(),
                        m_pChar->GetClass()->GetSkillTreeNodesEnd());
                }
                PopMenu();
            }
        }
    }
}

void SkillTreeState::HandleButtonDown ( const StoneRing::IApplication::Button& button )
{
    StoneRing::State::HandleButtonDown ( button );
}

void SkillTreeState::process_choice ( int selection )
{
    SkillTreeNode* node = m_skills[selection];
    SkillRef* pSkillRef = m_skills[selection]->GetRef();
    bool has_skill = m_pChar->HasSkill(pSkillRef->GetRef());
    bool can_afford = m_pChar->GetSP() >= node->GetSPCost();
    bool met_reqs = node->CanLearn(m_pChar) && ! has_skill;      
    
    if(m_eUse == BUY)
    {
        if(!has_skill && can_afford && met_reqs)
        {
            m_pChar->SetSP ( m_pChar->GetSP() - node->GetSPCost() );
            m_pChar->LearnSkill(pSkillRef->GetRef());
			SkillGetState state;
			state.SetSkill(pSkillRef->GetSkill());
			IApplication::GetInstance()->RunState(&state);
        }
        else
        {
            // play sound bbzzzt
            SoundManager::PlayEffect(SoundManager::EFFECT_BAD_OPTION);
        }
    }else if(m_eUse == USE)
    {
        if(has_skill && (pSkillRef->GetSkill()->GetType() == Skill::WORLD ||
            pSkillRef->GetSkill()->GetType() == Skill::BOTH))
        {
            if(pSkillRef->GetSkill()->GetMPCost() <= m_pChar->GetAttribute(ICharacter::CA_MP) 
                && pSkillRef->GetSkill()->GetBPCost() <= m_pChar->GetAttribute(ICharacter::CA_BP))
            {
                m_pSelectedNode = node;
                m_bDone = true;
                SoundManager::PlayEffect(SoundManager::EFFECT_SELECT_OPTION);
            }
            else {
                // Play sound bbzt
                SoundManager::PlayEffect(SoundManager::EFFECT_BAD_OPTION);
            }
        }
        else
        {
            // play sound bbzzzt
            SoundManager::PlayEffect(SoundManager::EFFECT_BAD_OPTION);
        }
    }
    
}

SkillTreeNode* SkillTreeState::GetSelectedSkillNode() const
{
    return m_pSelectedNode;
}

