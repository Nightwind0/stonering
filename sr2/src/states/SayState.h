#ifndef SR2_SAY_STATE_H
#define SR2_SAY_STATE_H

#include "State.h"
#include "sr_defines.h"

namespace StoneRing
{

    class SayState : public State
    {
    public:
	SayState();
	virtual ~SayState();

	virtual bool isDone() const;
	virtual void handleKeyDown(const CL_InputEvent &key);
	virtual void handleKeyUp(const CL_InputEvent &key);
	virtual void draw(const CL_Rect &screenRect,CL_GraphicContext * pGC);
	virtual bool lastToDraw() const { return false; } // It'll be last anyway.... and if not, thats okay too
	virtual bool disableMappableObjects() const; // Should the app move the MOs? 
	virtual void mappableObjectMoveHook(); // Do stuff right after the mappable object movement
	virtual void start(); 
	virtual void finish(); // Hook to clean up or whatever after being popped

	virtual void init(const std::string &speaker, const std::string &text);

    private:
	
		std::string mSpeaker;
		std::string mText;
		CL_Rect mSpeakerRect;
		CL_Rect mTextRect;
		CL_Surface *mpSayOverlay;
		std::string::iterator miText;
		bool mbDone;
		uint mnDrawnThisFrame;
		uint mnTotalDrawn;

    };
};


#endif


