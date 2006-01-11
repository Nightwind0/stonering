#ifndef SR2_MAP_STATE_H
#define SR2_MAP_STATE_H

#include "State.h"


namespace StoneRing
{

    class MappablePlayer;
    class Level;

    class MapState : public State
    {
    public:
	MapState();
	virtual ~MapState();

	virtual bool isDone() const;
	virtual void handleKeyDown(const CL_InputEvent &key);
	virtual void handleKeyUp(const CL_InputEvent &key);
	virtual void draw(const CL_Rect &screenRect,CL_GraphicContext * pGC);
	virtual bool lastToDraw() const { return false; } // It'll be last anyway.... and if not, thats okay too
	virtual bool disableMappableObjects() const; // Should the app move the MOs? 
	virtual bool drawMappableObjects() const; // Should the app draw the MOs, including the player?
	virtual void mappableObjectMoveHook(); // Do stuff right after the mappable object movement
	virtual void start(); 
	virtual void finish(); // Hook to clean up or whatever after being popped

	void setDimensions(const CL_Rect &screenRect);
	void setLevel(Level *pLevel);
	void setPlayer(MappablePlayer * pPlayer);
	void moveMappableObjects();

    private:
	void recalculatePlayerPosition();
	void doTalk(bool prod=false);


	bool mbDone;
	int mLevelX; // Offset into level. TopLeft corner of our view into level
	int mLevelY;
	CL_Rect mScreenRect;
	Level * mpLevel;
	MappablePlayer  *mpPlayer;
#ifndef NDEBUG
	bool mbShowDebug;
#endif

    };
};


#endif
