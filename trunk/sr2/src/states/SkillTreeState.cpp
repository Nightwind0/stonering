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
#include <sstream>
#include <iomanip>
#include <Graphic.h>


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
    m_path_rect = GraphicsManager::GetRect(GraphicsManager::SKILL_TREE,"path");
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
    
    m_available_gradient = GraphicsManager::GetGradient(GraphicsManager::SKILL_TREE,"available");
    m_unavilable_gradient = GraphicsManager::GetGradient(GraphicsManager::SKILL_TREE,"unavailable");
    m_selected_gradient = GraphicsManager::GetGradient(GraphicsManager::SKILL_TREE,"selected");
    m_req_gradient = GraphicsManager::GetGradient(GraphicsManager::SKILL_TREE,"reqs");
    
    
    m_overlay = GraphicsManager::GetOverlay(GraphicsManager::SKILL_TREE);
    
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

int SkillTreeState::height_for_option ( CL_GraphicContext& gc )
{
    return m_skill_size.y;
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

void SkillTreeState::draw_option ( int option, bool selected, float x, float y, CL_GraphicContext& gc )
{
    SkillRef* pSkillRef = m_skills[option]->GetRef();
    StoneRing::Font font;
    StoneRing::Font costFont;
    CL_Gradient gradient;
        
    bool has_skill = m_pChar->HasSkill(pSkillRef->GetRef());
    bool can_afford = m_pChar->GetSP() >= m_skills[option]->GetSPCost();
    bool met_reqs = m_skills[option]->CanLearn(m_pChar) && ! has_skill;
    
    bool can_use = (pSkillRef->GetSkill()->GetType() == Skill::WORLD ||
            pSkillRef->GetSkill()->GetType() == Skill::BOTH);
    
    if(selected){
        gradient = m_selected_gradient;
    }else{
        if(met_reqs)
            gradient = m_available_gradient;
        else
            gradient = m_unavilable_gradient;
    }
    
    CL_Rectf option_rect(x,y,x+m_menu.get_width(),y + height_for_option(gc));
    CL_Draw::gradient_fill(gc,option_rect,gradient);
     

    std::ostringstream cost_stream;
    
    if(pSkillRef->GetSkill()->GetBPCost()){
        cost_stream << std::setw(5) << pSkillRef->GetSkill()->GetBPCost() << ' ' << "BP";
        if(m_eUse != USE || m_pChar->GetAttribute(ICharacter::CA_BP) > pSkillRef->GetSkill()->GetBPCost()){
            m_bp_cost.draw_text(gc,x + m_use_cost_pt.x,y + m_use_cost_pt.y, cost_stream.str(),Font::TOP_LEFT);
        }else{
            m_not_enough_points_font.draw_text(gc,x + m_use_cost_pt.x,y + m_use_cost_pt.y, cost_stream.str(),Font::TOP_LEFT);
            can_use = false;
        }
    }else if(pSkillRef->GetSkill()->GetMPCost()){
        cost_stream << std::setw(5) << pSkillRef->GetSkill()->GetMPCost() << ' ' << "MP";
        if(m_eUse != USE || m_pChar->GetAttribute(ICharacter::CA_MP) > pSkillRef->GetSkill()->GetMPCost()){       
            m_mp_cost.draw_text(gc,x + m_use_cost_pt.x,y + m_use_cost_pt.y, cost_stream.str(), Font::TOP_LEFT);
        }else{
            m_not_enough_points_font.draw_text(gc,x + m_use_cost_pt.x,y + m_use_cost_pt.y, cost_stream.str(), Font::TOP_LEFT);
            can_use = false;
        }
    }
 
 
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
    
    pSkillRef->GetSkill()->GetIcon().draw(gc,x+m_icon_point.x,y+m_icon_point.y);
    
    if(!has_skill)
    {
        std::ostringstream cost_stream;
            cost_stream << std::setw(5) << m_skills[option]->GetSPCost() << ' ' << "SP";
        if(!can_afford)
            costFont = m_not_enough_points_font;
        else
            costFont = m_points_font;
        costFont.draw_text(gc,x + m_cost_point.x,y + m_cost_point.y, cost_stream.str(), Font::TOP_LEFT);
    }
    


    if(m_skills[option]->GetSubSkillsBegin() != m_skills[option]->GetSubSkillsEnd())
    {
        // There are subskills, so draw the arrow
        m_arrow.draw(gc, x + m_arrow_point.x, y + m_arrow_point.y);
    }
    

    
    font.draw_text(gc, x + m_name_offset.x, 
                   y +  m_name_offset.y,m_skills[option]->GetRef()->GetRef(),Font::TOP_LEFT);
    
}


CL_Rectf SkillTreeState::get_rect()
{
    return m_menu;
}

void SkillTreeState::Draw ( const CL_Rect& screenRect, CL_GraphicContext& GC )
{
    m_overlay.draw(GC,0,0);
    CL_Rectf box = m_menu;
    box.expand(2,2);
    CL_Draw::box(GC,box,CL_Colorf::white);

    
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
    
    
    draw_text(GC, m_path_font, m_path_rect, pathDesc.str());  
    Menu::Draw( GC );
    
    // Draw portrait
    // shadow first
    m_portrait_shadow.draw(GC,m_portrait_offset.x + 4, m_portrait_offset.y + 4);
    m_pChar->GetPortrait(Character::PORTRAIT_DEFAULT).draw(GC,m_portrait_offset.x,m_portrait_offset.y);
    
    // Now draw description
    SkillTreeNode * pNode = m_skills[ get_current_choice() ];
    Skill * pSkill = pNode->GetRef()->GetSkill();
    SkillRef* pSkillRef = pNode->GetRef();
    
    draw_text(GC, m_desc_font, m_description, pSkill->GetDescription()); 
    
    m_char_name_font.draw_text(GC,m_char_name_pt.x,m_char_name_pt.y, m_pChar->GetName());
    m_char_sp_font.draw_text(GC,m_char_sp_pt.x,m_char_sp_pt.y, "SP " +IntToString(m_pChar->GetSP()));
    if(m_eUse == USE){
        m_char_mp_font.draw_text(GC,m_char_sp_pt.x,m_char_sp_pt.y + m_char_sp_font.get_font_metrics(GC).get_height(),
                             "MP " + IntToString(m_pChar->GetAttribute(ICharacter::CA_MP)));
        m_char_bp_font.draw_text(GC,m_char_sp_pt.x,m_char_sp_pt.y + m_char_sp_font.get_font_metrics(GC).get_height() 
                            + m_char_mp_font.get_font_metrics(GC).get_height(),
                             "BP " + IntToString(m_pChar->GetAttribute(ICharacter::CA_BP)));
    }
    
    bool has_skill = m_pChar->HasSkill(pSkillRef->GetRef());
    bool can_afford = m_pChar->GetSP() >= pNode->GetSPCost();
    bool met_reqs = pNode->CanLearn(m_pChar);
    
    CL_Rectf req_box = m_reqs;
    req_box.expand(2,2);
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

    if(!met_reqs){       
        CL_Draw::gradient_fill(GC,m_reqs,m_req_gradient);
        CL_Draw::box(GC,req_box,CL_Colorf::white);
        draw_text(GC,m_reqs_font,m_reqs,reqs.str());
    }else { 
        CL_Draw::gradient_fill(GC,m_reqs,m_available_gradient);
        CL_Draw::box(GC,req_box,CL_Colorf::white);
        draw_text(GC,m_reqs_font,m_reqs,reqs.str());
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
        }
        else
        {
            // play sound bbzzzt
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
            }
            else {
                // Play sound bbzt
            }
        }
        else
        {
            // play sound bbzzzt
        }
    }
    
}

SkillTreeNode* SkillTreeState::GetSelectedSkillNode() const
{
    return m_pSelectedNode;
}
