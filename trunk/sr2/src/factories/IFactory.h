#ifndef SR_FACTORY_H
#define SR_FACTORY_H

#include <string>

#include "Element.h"

namespace StoneRing
{

    class IFactory
    {
    public:
        IFactory(){}
        virtual ~IFactory(){}
        virtual Element * createElement( const std::string &element )=0;
    };
};

#endif




