#ifndef __ICharacterGroup_h_
#define __ICharacterGroup_h_

#include "steel/SteelType.h"


namespace StoneRing { 
    
    class ICharacter;

    class ICharacterGroup: public SteelType::IHandle
    {
    public:
        virtual ~ICharacterGroup(){}
        virtual uint GetCharacterCount() const = 0;
        virtual ICharacter * GetCharacter(uint index) const = 0;
    private:
    };
    
}

#endif