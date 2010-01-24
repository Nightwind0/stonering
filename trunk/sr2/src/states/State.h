#ifndef SR2_STATE_H
#define SR2_STATE_H

#include <ClanLib/core.h>
#include <ClanLib/display.h>

#include "steel/SteelInterpreter.h"
#include "steel/SteelType.h"

namespace StoneRing
{

    class State
    {
    public:

        virtual bool IsDone() const = 0;
        virtual void HandleKeyDown(const CL_InputEvent &key)=0;
        virtual void HandleKeyUp(const CL_InputEvent &key)=0;
        virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC)=0;
        virtual bool LastToDraw() const =0; // Should we continue drawing more states?
        virtual bool DisableMappableObjects() const =0; // Should the app move the MOs?
        virtual void MappableObjectMoveHook() =0; // Do stuff right after the mappable object movement
        virtual void Start()=0;
        virtual void SteelInit      (SteelInterpreter *);
        virtual void SteelCleanup   (SteelInterpreter *);
        virtual void Finish()=0; // Hook to clean up or whatever after being popped
        void SteelConst(SteelInterpreter*,const std::string &name, int value);
        void SteelConst(SteelInterpreter*,const std::string &name, double value);
    };

    inline void State::SteelInit(SteelInterpreter* )
    {
    }

    inline void State::SteelCleanup(SteelInterpreter* )
    {
    }

    inline void State::SteelConst(SteelInterpreter* pInterpreter,const std::string &name, int value)
    {
        SteelType val;
        val.set(value);
        pInterpreter->declare_const(name,val);
    }

    inline void State::SteelConst(SteelInterpreter* pInterpreter,const std::string &name, double value)
    {
        SteelType val;
        val.set(value);
        pInterpreter->declare_const(name,val);
    }

}


#endif




