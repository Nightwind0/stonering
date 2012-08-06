#include "Direction.h"


const StoneRing::Direction StoneRing::Direction::NORTH(270);
const StoneRing::Direction StoneRing::Direction::SOUTH(90);
const StoneRing::Direction StoneRing::Direction::EAST(0);
const StoneRing::Direction StoneRing::Direction::WEST(180);
const StoneRing::Direction StoneRing::Direction::NONE(-1);


StoneRing::Direction::Direction ( const std::string& str )
{
    if(str == "north"){
        *this =  Direction::NORTH;
    }else if(str == "south"){
        *this =  Direction::SOUTH;
    }else if(str == "west"){
        *this =  Direction::WEST;
    }else if(str == "east"){
        *this =  Direction::EAST;
    }
    else throw CL_Exception("Bad direction " + str);
}


StoneRing::Direction::operator std::string () const
{
    if(*this == Direction::NORTH)
        return "north";
    else if(*this == Direction::SOUTH){
        return "south";
    }else if(*this == Direction::WEST){
        return "west";
    }else if(*this == Direction::EAST){
        return "east";
    }else return "none";
}


