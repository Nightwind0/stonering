#ifndef SR2_MAP_STATE_H
#define SR2_MAP_STATE_H

#include "State.h"
#include "Level.h"
#include "sr_defines.h"
#include "Navigator.h"
#include "DebugTileVisitors.h"
#include "MappableObject.h"
#include "TimedInterpolator.h"
#include <deque>


namespace StoneRing
{

class ControlNavigator;
class Level;

class MapState : public State
{
public:
    MapState();
    virtual ~MapState();

    virtual bool IsDone() const;

    virtual void HandleQuit() {
        m_bDone = true;
    }

    virtual void Draw(const clan::Rect &screenRect,clan::Canvas& GC);
    virtual bool LastToDraw() const {
        return false;    // It'll be last anyway.... and if not, thats okay too
    }
    virtual bool DisableMappableObjects() const; // Should the app move the MOs?
    virtual bool DrawMappableObjects() const; // Should the app draw the MOs, including the player?
    virtual void Update(); // Do stuff right after the mappable object movement
    virtual void Start();
    virtual void RegisterSteelFunctions(SteelInterpreter *);
    virtual void Finish(); // Hook to clean up or whatever after being popped
	virtual void Covered();
	clan::Point GetCurrentCenter() const;
    void SetDimensions(const clan::Rect &screenRect);
    void PushLevel(Level *pLevel, uint startX, uint startY);
	void LoadLevel(Level *pLevel, uint startX, uint startY);
    void SetPlayerSprite(clan::Sprite player);
    void MoveMappableObjects();
    void Pop(bool bAll);
    Level * GetCurrentLevel()const;
	void BringDown();

    void SerializeState(std::ostream& out);
    void DeserializeState(std::istream& in);
protected:
    virtual void HandleButtonUp(const IApplication::Button& button);
    virtual void HandleButtonDown(const IApplication::Button& button);
    virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir,float pos);	
    virtual void HandleKeyDown(const clan::InputEvent &key);
    virtual void HandleKeyUp(const clan::InputEvent &key);	
private:
    void recalculate_player_position(bool lerp=true);
    void do_talk(bool prod=false);

    bool m_bDone;
    int m_LevelX; // Offset into level. TopLeft corner of our view into level
    int m_LevelY;
	TimedInterpolator<float> m_lerpLevelX;
	TimedInterpolator<float> m_lerpLevelY;
    bool m_horizontal_idle;
    bool m_vertical_idle;
    MappablePlayer  m_player;
    clan::Rect m_screen_rect;
    std::deque<Level*> m_levels;
    Level * m_pLevel;
    clan::Sprite  m_playerSprite;
    bool m_bShowDebug;
	std::deque<clan::Point> m_position_stack;
#ifndef NDEBUG
    void add_debug_drawers();
    void remove_debug_drawers();
    TileSideBlockDrawer m_block_drawer;
    TileHotDrawer            m_hot_drawer;
    TileFloaterDrawer        m_floater_drawer;
#endif
};
}

#endif




