#ifndef SR_ACTION_H
#define SR_ACTION_H

#include "Element.h"

namespace StoneRing{

    // Things which take actions on the party
    class Action
    {
    public:
        Action(){}
        virtual ~Action(){}

        virtual void invoke()=0;
    };

};


#endif

