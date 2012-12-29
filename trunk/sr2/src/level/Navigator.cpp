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


#include "Navigator.h"
#include "MappableObject.h"
#include "Level.h"
#include "SoundManager.h"
#include <deque>
#include <vector>
#include <set>
#include <map>

#define USE_CLOSED_SET 1

namespace StoneRing {


NPCNavigator::NPCNavigator( MappableObject& mo ): Navigator( mo ) {
	m_eDirection = Direction::NONE;
}


void NPCNavigator::OnMove(StoneRing::Level&)
{
    moved_one_cell();
}

void NPCNavigator::Blocked()
{
    random_new_direction();
}



void NPCNavigator::Idle ( )
{

}

void NPCNavigator::Prod ( )
{
    random_new_direction();
}

Direction NPCNavigator::GetCurrentDirection() const
{
    return m_eDirection;
}

Direction NPCNavigator::GetFacingDirection() const
{
    return (m_eDirection == Direction::NONE)?m_mo.GetDefaultFacing():m_eDirection;
}

uint NPCNavigator::GetSpeed() const
{
    uint nMoves;
    switch(m_mo.GetMovementSpeed()){
        case MappableObject::SLOW:
            nMoves = 1;
            break;
        case MappableObject::MEDIUM:
            nMoves = 3;
            break;
        case MappableObject::FAST:
            nMoves = 5;
            break;
    }
    return nMoves;

}





void NPCNavigator::set_frame_direction ( )
{

}


void NPCNavigator::random_new_direction ( )
{
    Direction current = m_eDirection;

    while(m_eDirection == current)
    {
        int r= rand() % 5;

        switch (m_mo.GetMovementType())
        {
        case MappableObject::MOVEMENT_NONE:
            return;
        case MappableObject::MOVEMENT_WANDER:
            if(r == 0)
                m_eDirection = Direction::NORTH;
            else if(r == 1)
                m_eDirection = Direction::SOUTH;
            else if(r == 2)
                m_eDirection = Direction::EAST;
            else if(r == 3)
                m_eDirection = Direction::WEST;
            else if(r == 4)
                m_eDirection = Direction::NONE;
            break;
        case MappableObject::MOVEMENT_PACE_NS:
            if(m_eDirection == Direction::NONE)
                m_eDirection = Direction::SOUTH;
            if(r > 2) pick_opposite_direction();
            break;
        case MappableObject::MOVEMENT_PACE_EW:
            if(m_eDirection == Direction::NONE)
                m_eDirection = Direction::EAST;
            if(r > 2)
                pick_opposite_direction();
            break;
        default:
            break;

        }
    }

    m_nStepsUntilChange = rand() % 15 + 5;
    changed_direction();
}

void NPCNavigator::pick_opposite_direction(){
    m_eDirection = m_eDirection.opposite();
    changed_direction();
}

void NPCNavigator::moved_one_cell()
{
    if(++m_nStepsInDirection)
    {
        if(m_nStepsInDirection == m_nStepsUntilChange) ///@todo: get from somewhere
        {
            random_new_direction();
        }
    }

}

void NPCNavigator::changed_direction()
{
    m_nStepsInDirection = 0;
    set_frame_direction();
}

bool NPCNavigator::InMotion() const {
	return m_eDirection != Direction::NONE;
}


ControlNavigator::ControlNavigator(MappableObject& mo):Navigator(mo)
{
    m_dir = mo.GetDefaultFacing();
    m_facingDir = m_dir;
}

ControlNavigator::~ControlNavigator()
{
}

Direction ControlNavigator::GetCurrentDirection() const
{
    return m_dir;
}

void ControlNavigator::Idle ( )
{
    m_nextDir = Direction::NONE;
}

void ControlNavigator::moved_one_cell ( )
{
    m_dir = m_nextDir;
    if(m_dir != Direction::NONE){
        m_facingDir = m_dir;
    }
}


uint ControlNavigator::GetSpeed() const
{
    uint nMoves = 4;    
    if(m_isRunning) 
        nMoves = 8;

    return nMoves;
}


void ControlNavigator::Prod ( )
{
    m_dir = Direction::NONE;
}


void ControlNavigator::Blocked()
{
	// Without this line, after colliding with an MO, it tries repeatedly to go through it, 
	// causing tons of repeated collide type Events to fire (if any)	
    m_dir = Direction::NONE; 
}

Direction ControlNavigator::GetFacingDirection() const
{
    return m_facingDir;
}

void ControlNavigator::OnMove ( Level& level )
{
    moved_one_cell();
}


void ControlNavigator::SetNextDirection ( const StoneRing::Direction& next )
{
    m_nextDir = next;
    if(m_dir == Direction::NONE){
        m_dir = m_nextDir;
    }
}

void ControlNavigator::SetRunning ( bool running )
{
    m_isRunning = running;
}


bool ControlNavigator::InMotion() const {
	return m_dir != Direction::NONE;
}


StillNavigator::StillNavigator(MappableObject& mo):Navigator(mo)
{

}
StillNavigator::~StillNavigator()
{

}


void StillNavigator::Idle()
{
}

void StillNavigator::Prod ()
{

}

Direction StillNavigator::GetCurrentDirection() const
{
    return Direction::NONE;
}


void StillNavigator::Blocked()
{

}

Direction StillNavigator::GetFacingDirection() const
{
    return Direction::NONE;   
}

uint StillNavigator::GetSpeed() const
{
    return 0;
}

void StillNavigator::OnMove ( Level& level )
{

}








ScriptedNavigator::ScriptedNavigator ( Level& level, MappableObject& mo, const CL_Point& dest, int speed ):Navigator(mo),m_level(level)
{
    m_dest = dest;
    m_change_face = false;
    m_speed = speed;
    m_complete = false;
}

ScriptedNavigator::ScriptedNavigator ( Level& level, MappableObject& mo, const StoneRing::Direction& dir ):Navigator(mo),m_level(level)
{
    m_face_dir = dir;
    m_change_face = true;
    m_complete = false;
}

ScriptedNavigator::~ScriptedNavigator()
{

}


Direction ScriptedNavigator::GetCurrentDirection() const
{
    return m_dir;
}

void ScriptedNavigator::Idle ()
{

}

#if 0 
void ScriptedNavigator::MoveObject ( MappableObject& mo, Level& level )
{
    // A-Star
    
}
#endif


void ScriptedNavigator::Blocked()
{
#ifndef NDEBUG
    std::cerr << "Oops - hit something in my path. Recalc" << std::endl;
#endif
    m_dir = Direction::NONE;
    if(compute_astar(m_level))
        set_direction_to_next(true);
}

uint ScriptedNavigator::GetSpeed() const
{
    return m_speed;
}

std::list<int> ScriptedNavigator::neighbors ( const int& id, const Level& level )
{
    std::list<int> neighbors;
    CL_Point point = to_point(id);
    CL_Size size = m_mo.GetSize();

    CL_Rect origin(point,size);
    CL_Point up,down,left,right;
    up = down = left = right = point;
    --up.y;
    ++down.y;
    --left.x;
    ++right.x;
    
    
    if(up.y >= 0 && level.CanMove(&m_mo,origin,CL_Rect(up,size))){
        neighbors.push_back(point_id(up));
    }
    if(down.y < level.GetHeight() && level.CanMove(&m_mo,origin,CL_Rect(down,size))){
        neighbors.push_back(point_id(down));
    }
    if(left.x >= 0 && level.CanMove(&m_mo,origin,CL_Rect(left,size))){
        neighbors.push_back(point_id(left));
    }
    if(right.x < level.GetWidth() && level.CanMove(&m_mo,origin,CL_Rect(right,size))){
        neighbors.push_back(point_id(right));
    }
#ifndef NDEBUG
    //std::cout << "Neighbors for " << point.x << ',' << point.y << '=' << neighbors.size() << std::endl;
#endif
    
    return neighbors;
}

int ScriptedNavigator::point_id ( const CL_Point& pt) const 
{
    return pt.y * m_level.GetWidth() + pt.x;
}

CL_Point ScriptedNavigator::to_point ( int point_id ) const
{
    int y = point_id / m_level.GetWidth();
    int x = point_id % m_level.GetWidth();
    return CL_Point(x,y);
}


float ScriptedNavigator::heuristic_cost_estimate ( const CL_Point& a, const CL_Point& b ) const
{
    // Manhattan distance
    return  1.001 * (std::abs(a.x-b.x) + std::abs(a.y-b.y)); 
    //return a.distance(b);
}

bool ScriptedNavigator::compute_astar(const Level &level)
{
    //SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);
    CL_Point start = m_mo.GetPosition();
    const int start_id = point_id(start);
    const int dest_id = point_id(m_dest);
    m_path.clear();
    
    if(start == m_dest){
        m_dir = Direction::NONE;
        m_complete = true;
        return true;
    }
    
#ifndef NDEBUG
    std::cout << "Calc path from " << start.x << ',' << start.y << " to " << m_dest.x << ',' << m_dest.y << std::endl;
#endif
    
    // TODO: Maybe calculate the path once? 
    std::map<int, float> g_score;
    std::map<int, float> h_score;
    std::map<int, float> f_score;
    std::set<int> closed; // The set of nodes already evaluated.
    std::set<int> open;
    std::map<int, int> came_from;


    g_score[start_id] = 0;    // Cost from start along best known path.
    h_score[start_id] = heuristic_cost_estimate ( start, m_dest );
    f_score[start_id] = 0 + h_score[start_id];    // Estimated total cost from start to goal through y.
    open.insert ( start_id ); 
    
    bool success = false;

    while ( !open.empty() ) {
        float best_score = f_score[*open.begin()];
        std::set<int>::iterator best_it = open.begin();
        int best = *best_it;

        for ( std::set<int>::iterator it = open.begin(); it != open.end(); it++ ) {
            if ( f_score[*it] < best_score ) {
                best_score = f_score[*it];
                best = *it;
            }
        }

        if ( best == dest_id ){
            m_path.push_back(m_dest);
            reconstruct_path ( came_from, came_from[dest_id] );
            success = true;
            break;
        }

        open.erase(best);
        closed.insert ( best );
        std::list<int> neighbor_list = neighbors ( best, level );

        for ( std::list<int>::const_iterator neighbor = neighbor_list.begin();
                neighbor != neighbor_list.end(); neighbor++ ) {

            float tentative_g_score = g_score[best] + 1.0f; // all neighbors have a simple cost of 1

            if(open.count(*neighbor) && tentative_g_score < g_score[*neighbor]){
                open.erase(*neighbor);
            }
            if(closed.count(*neighbor) && tentative_g_score < g_score[*neighbor]){
                closed.erase(*neighbor);
            }
            if(!open.count(*neighbor) && !closed.count(*neighbor)){
                g_score[*neighbor] = tentative_g_score;
                open.insert(*neighbor);
                f_score[*neighbor] = g_score[*neighbor] + h_score[*neighbor];
                came_from[*neighbor] = best;
            }
      
        }

    }
    if(!success){
        // No way to path, but we want the cut scene to continue
        std::cerr << "Error: No way to destination." << std::endl;
        m_complete = true;
    }
    
    return success;
}

void ScriptedNavigator::set_direction_to_next(bool f)
{
    CL_Point start = m_mo.GetPosition();

#if 0 // What is this for??
    if(!f){
        if(start != m_next_step)
            return;
    }
#endif

    if(!m_path.empty()){
        m_next_step = m_path.back();
#ifndef NDEBUG
        std::cout << "Moving to: " << m_next_step.x << ',' << m_next_step.y << std::endl;
#endif
        m_path.pop_back();
        if(m_next_step.y > start.y){
            m_dir = Direction::SOUTH;
        }else if(m_next_step.y < start.y){
            m_dir = Direction::NORTH;
        }else if(m_next_step.x > start.x){
            m_dir = Direction::EAST;
        }else if(m_next_step.x < start.x){
            m_dir = Direction::WEST;
        }
        m_face_dir = m_dir;
    }else{
        m_complete = true;
    }
}


void ScriptedNavigator::OnMove ( Level& level )
{
    if(!m_complete && m_path.empty()){
        if(compute_astar(m_level)){
            set_direction_to_next();
        }
    }else{
        set_direction_to_next();
    }
}

void ScriptedNavigator::reconstruct_path ( const std::map<int,int>& came_from, const int& current )
{
    int point = current;
#ifndef NDEBUG
    m_level.ClearPath();
    std::cout << "Path" << std::endl;
#endif
    while(true){
        m_path.push_back(to_point(point));
#ifndef NDEBUG
        std::cout << "Point: " << m_path.back().x << ',' << m_path.back().y << std::endl;
        m_level.AddPathTile(to_point(point));
#endif
        std::map<int,int>::const_iterator it = came_from.find(point);
        if(it == came_from.end())
            break;
        point = it->second;
    }    
    m_path.pop_back();
}



Direction ScriptedNavigator::GetFacingDirection() const
{
    return m_face_dir;
}

void ScriptedNavigator::Prod (  )
{
    m_dir = Direction::SOUTH;
    m_face_dir = Direction::SOUTH;
    if(compute_astar(m_level))
        set_direction_to_next(true);
}

bool ScriptedNavigator::InMotion() const {
	return !m_complete && m_dir != Direction::NONE;
}




}