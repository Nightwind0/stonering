#if !defined(__SR2__DIRECTION_H__)
#define __SR2__DIRECTION_H__

#include <ClanLib/core.h>

namespace StoneRing {
    
class Direction
{ 
public:
    static const Direction NONE;
    static const Direction NORTH;
    static const Direction SOUTH;
    static const Direction EAST;
    static const Direction WEST;
    
    
    Direction(){
        *this = NONE;
    }
    Direction(const Direction& other){
        m_none = other.m_none;
        m_value = other.m_value;
    }
    Direction(const int i):m_value(i%360),m_none(false){
    }
    Direction(bool none){
        m_none = true;
        m_value = 0;
    }
    Direction(const std::string& str);
    bool operator<(const Direction& other)const{
        return m_value < other.m_value;
    }
    Direction opposite() const {
        return Direction(m_value + 180);
    }
    bool operator==(const Direction& other)const{
        return other.m_none == m_none && other.m_value == m_value;
    }
    bool operator!=(const Direction& other)const{
        return !((*this) == other);
    }
    clan::Vec2<int> ToScreenVector()const{
        if(m_value == NORTH.m_value)
            return clan::Vec2<int>(0,-1);
        else if(m_value == SOUTH.m_value)
            return clan::Vec2<int>(0,1);
        else if(m_value == EAST.m_value)
            return clan::Vec2<int>(1,0);
        else if(m_value == WEST.m_value)
            return clan::Vec2<int>(-1,0);
        else return clan::Vec2<int>(0,0);
    }
    operator int() const {
        return m_value;
    }
    operator std::string() const;
private:
    int m_value;
    bool m_none;
};


}


#endif