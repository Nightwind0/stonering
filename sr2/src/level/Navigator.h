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


#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include "sr_defines.h"
#include "Direction.h"
#include <list>
#include <deque>

namespace StoneRing{
    
class MappableObject;
class Level;

class Navigator
{
public:
    Navigator(MappableObject& mo):m_mo(mo){}
    virtual ~Navigator(){}
    virtual void OnMove(Level& level)=0;
    virtual void Blocked()=0;
    virtual void Idle()=0;
    virtual void Prod()=0;
    virtual Direction GetCurrentDirection()const=0;
    virtual Direction GetFacingDirection()const=0;
    virtual uint GetSpeed()const=0;
	virtual bool InMotion() const=0;
protected:
    MappableObject& m_mo;
};

class NPCNavigator : public Navigator
{
public:
    NPCNavigator(MappableObject& mo);
    virtual ~NPCNavigator(){}
    virtual void OnMove(Level& level);
    virtual void Blocked();
    virtual void Idle();
    virtual void Prod();
    virtual Direction GetCurrentDirection()const;
    virtual Direction GetFacingDirection()const;
    virtual uint GetSpeed()const;
	virtual bool InMotion() const;
private:
    bool single_move(MappableObject& mo, Level& level);
    void moved_one_cell();
    void random_new_direction();
    void changed_direction();
    void pick_opposite_direction();
    void set_frame_direction();
    Direction m_eDirection;
    uint m_nStepsInDirection;
    uint m_nStepsUntilChange;
};

class ControlNavigator : public Navigator
{
public:
    ControlNavigator(MappableObject& mo);
    virtual ~ControlNavigator();
    virtual void OnMove(Level& level);
    virtual void Blocked();
    virtual void Idle();
    virtual void Prod();
    virtual Direction GetCurrentDirection()const;
    virtual Direction GetFacingDirection()const;
    virtual uint GetSpeed()const;
    void SetRunning(bool running);
    void SetNextDirection(const Direction& next);
	virtual bool InMotion() const;
private:
    void moved_one_cell();
    bool single_move();
    bool m_isRunning;
    Direction m_nextDir;
    Direction m_dir;    
    Direction m_facingDir;
};

class StillNavigator : public Navigator 
{
public:
    StillNavigator(MappableObject& mo);
    virtual ~StillNavigator();
    virtual void OnMove(Level& level);
    virtual void Blocked();
    virtual void Idle();
    virtual void Prod();
    virtual Direction GetCurrentDirection()const;    
    virtual Direction GetFacingDirection()const;
    virtual uint GetSpeed()const;
	virtual bool InMotion()const { return false; }
private:
};


class ScriptedNavigator : public Navigator 
{
public:
    ScriptedNavigator(Level& level,MappableObject& mo,const CL_Point& dest, int speed);
    ScriptedNavigator(Level& level,MappableObject& mo,const Direction& face_dir);
    virtual ~ScriptedNavigator();
    virtual void OnMove(Level& level);
    virtual void Blocked();
    virtual void Idle();
    virtual void Prod();
    virtual Direction GetCurrentDirection()const;   
    virtual Direction GetFacingDirection()const;
    virtual uint GetSpeed()const;
	virtual bool InMotion()const;
    bool Complete() const { return m_complete; }
private:
    float heuristic_cost_estimate(const CL_Point& a, const CL_Point& b)const;
    void reconstruct_path( const std::map<int,int> & came_from, const int& current);
    std::list<int> neighbors(const int& point, const Level& level);
    bool compute_astar(const Level& level);
    void set_direction_to_next(bool f=false);
    //void compute_dijkstra(const Level& level);
    int point_id(const CL_Point& pt)const;
    CL_Point to_point(int point_id)const;
    bool m_change_face;
    CL_Point m_dest;
    CL_Point m_next_step;
    int m_speed;
    Direction m_dir;
    Direction m_face_dir;
    bool m_complete;
    std::deque<CL_Point> m_path;
    Level& m_level;
};

}

#endif // NAVIGATOR_H
