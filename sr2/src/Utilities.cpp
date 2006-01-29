#include "sr_defines.h"

std::string IntToString(const int &i)
{
    std::ostringstream os;
        
    os << i;
           

    return os.str();
    
}

std::string FloatToString(const float &f)
{
	std::ostringstream os;

	os << f;

	return os.str();
}
