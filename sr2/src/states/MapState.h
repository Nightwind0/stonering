#ifndef SR2_MAP_STATE_H
#define SR2_MAP_STATE_H

#include "State.h"
#include "Level.h"
#include "sr_defines.h"
#include <stack>


namespace StoneRing
{

    class MappablePlayer;
    class Level;

    class MapState : public State
    {
    public:
        MapState();
        virtual ~MapState();

        virtual bool IsDone() const;
	virtual void HandleButtonUp(const IApplication::Button& button);
	virtual void HandleButtonDown(const IApplication::Button& button);
	virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir,float pos);
        virtual void HandleKeyDown(const CL_InputEvent &key);
        virtual void HandleKeyUp(const CL_InputEvent &key);
        virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
        virtual bool LastToDraw() const { return false; } // It'll be last anyway.... and if not, thats okay too
        virtual bool DisableMappableObjects() const; // Should the app move the MOs?
        virtual bool DrawMappableObjects() const; // Should the app draw the MOs, including the player?
        virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void Start();
        virtual void RegisterSteelFunctions(SteelInterpreter *);
        virtual void Finish(); // Hook to clean up or whatever after being popped

        void SetDimensions(const CL_Rect &screenRect);
        void PushLevel(Level *pLevel, uint startX, uint startY);
        void SetPlayerSprite(CL_Sprite player);

        void MoveMappableObjects();

        void Pop(bool bAll);

    private:
        void recalculate_player_position();
        void do_talk(bool prod=false);
        void switch_from_player(MappablePlayer * pPlayer);

        bool m_bDone;
        int m_LevelX; // Offset into level. TopLeft corner of our view into level
        int m_LevelY;
	bool m_horizontal_idle;
	bool m_vertical_idle;
        CL_Rect m_screen_rect;
        std::stack<Level*> m_levels;
        Level * m_pLevel;
        CL_Sprite  m_playerSprite;
        bool m_bShowDebug;
    };
}

#endif




