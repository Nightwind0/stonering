#ifndef SR_PLAY_SOUND_H
#define SR_PLAY_SOUND_H

#include "Element.h"

#ifdef PlaySound
#undef PlaySound
#endif

namespace StoneRing{
    
    class PlaySound : public Action, public Element
    {
    public:
        PlaySound();
        virtual ~PlaySound();
        virtual eElement whichElement() const{ return EPLAYSOUND; } 
        virtual void invoke();

        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

    protected:
        virtual void handleText(const std::string &);
        std::string mSound;
    };
};

#endif

