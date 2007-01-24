#ifndef SR_CHECK_H
#define SR_CHECK_H   

#include "Element.h"
#include "Item.h"

namespace StoneRing{

    // Things that evaluate by examining the party
    class Check : public Element
    {
    public:
        Check(){}
        virtual ~Check(){}

        virtual bool evaluate()=0;
    };


};

#endif

