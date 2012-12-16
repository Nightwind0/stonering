#ifndef __ICharacterGroup_h_
#define __ICharacterGroup_h_

#ifdef _WIN32
#include "SteelType.h"
#else
#include "steel/SteelType.h"
#endif

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