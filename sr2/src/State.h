#ifndef SR2_STATE_H
#define SR2_STATE_H

#include <ClanLib/core.h>
#include <ClanLib/display.h>

namespace StoneRing
{

    class State
    {
    public:

        virtual bool isDone() const = 0;
        virtual void handleKeyDown(const CL_InputEvent &key)=0;
        virtual void handleKeyUp(const CL_InputEvent &key)=0;
        virtual void draw(const CL_Rect &screenRect,CL_GraphicContext * pGC)=0;
        virtual bool lastToDraw() const =0; // Should we continue drawing more states?
        virtual bool disableMappableObjects() const =0; // Should the app move the MOs? 
        virtual void mappableObjectMoveHook() =0; // Do stuff right after the mappable object movement
        virtual void start()=0; 
        virtual void finish()=0; // Hook to clean up or whatever after being popped

    };
};


#endif


