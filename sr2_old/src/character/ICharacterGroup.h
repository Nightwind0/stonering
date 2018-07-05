#ifndef __ICharacterGroup_h_
#define __ICharacterGroup_h_


#include "sr_defines.h"

namespace StoneRing { 
    
    class ICharacter;

    class ICharacterGroup: public Steel::SteelType::IHandle
    {
    public:
        virtual ~ICharacterGroup(){}
        virtual uint GetCharacterCount() const = 0;
        virtual ICharacter * GetCharacter(uint index) const = 0;
    private:
    };
    
}

#endif