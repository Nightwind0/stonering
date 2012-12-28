#ifndef SR_IAPPLICATION_H
#define SR_IAPPLICATION_H


#include <ClanLib/core.h>
#include <ClanLib/display.h>
#ifdef _WINDOWS_
#include <SteelType.h>
#else
#include <steel/SteelType.h>
#endif
#include "AbilityManager.h"
#include "MonsterGroup.h"
#include "ElementFactory.h"

//using Steel::AstScript;



namespace StoneRing {

class LevelFactory;
class ItemFactory;
class ItemManager;
class CharacterManager;
class Choice;
class State;
class Level;
class MonsterRef;
class BattleConfig;
class Party;

class IApplication {
public:
	enum Button {
		BUTTON_INVALID,
		BUTTON_CONFIRM,
		BUTTON_CANCEL,
		BUTTON_MENU,
		BUTTON_ALT,
		BUTTON_SELECT,
		BUTTON_START,
		BUTTON_R,
		BUTTON_L
	};

	enum Axis {
		AXIS_HORIZONTAL,
		AXIS_VERTICAL
	};

	enum AxisDirection {
		AXIS_NEUTRAL,
		AXIS_LEFT,
		AXIS_RIGHT,
		AXIS_UP,
		AXIS_DOWN
	};

	enum MouseButton {
		MOUSE_LEFT,
		MOUSE_RIGHT,
		MOUSE_MIDDLE,
		MOUSE_UNKNOWN
	};

	enum KeyState {
		KEY_SHIFT = 1,
		KEY_ALT = ( 1 << 1 ),
		KEY_CTRL = ( 1 << 2 )
	};


	enum Utility {
		XP_FOR_LEVEL,
		LEVEL_FOR_XP,
		ON_ATTACK
	};

	// Functor for running code on main thread
	class Functor {
	public:
		Functor() {}
		virtual ~Functor() {}
		virtual void operator()() {
		}
	};

	static IApplication * GetInstance();
	virtual ~IApplication() {}
	virtual CL_ResourceManager&GetResources() = 0;
	virtual CL_DisplayWindow&  GetApplicationWindow() = 0;
	virtual Party *        GetParty() const = 0;
	// virtual Party * GetReserveParty() const=0;
	virtual AbilityManager *GetAbilityManager() = 0;
	virtual IFactory *      GetElementFactory() = 0;
	virtual void            StartBattle( const MonsterGroup &group, const std::string &backdrop ) = 0;
	virtual Level*          GetCurrentLevel()const = 0;
	virtual void            MainMenu() = 0;
	virtual void            LoadMainMenu( CL_DomDocument& ) = 0;
	virtual CL_Rect         GetDisplayRect() const = 0;
	// TODO: Shouldn't this be on the Party ??
	virtual uint            GetMinutesPlayed()const = 0;
	virtual void            RequestRedraw( const State *pState ) = 0;
	virtual void            RunState( State *pState, bool threaded = false ) = 0;
	virtual void            StartGame( bool load ) = 0;
	virtual Steel::AstScript * LoadScript( const std::string &name, const std::string &script ) = 0;
	virtual SteelType       RunScript( Steel::AstScript * pScript ) = 0;
	virtual SteelType       RunScript( Steel::AstScript *pScript, const ParameterList &params ) = 0;
	virtual Steel::AstScript *     GetUtility( Utility util )const = 0;
	virtual std::string     GetCurrencyName()const = 0;
	virtual void            RunOnMainThread( CL_Event& event, Functor* functor ) = 0;
	virtual bool            Serialize( std::ostream& stream ) = 0;
	virtual bool            Deserialize( std::istream& stream ) = 0;
	virtual void            Banner( const std::string& str, int time ) = 0;
	virtual CL_IODevice     OpenResource(const std::string& str)=0;
};

}

//#define  GET_MAIN_GC ()   CL_GraphicContext GC = IApplication::GetInstance()->GetApplicationWindow().get_gc()
#define GET_MAIN_GC() IApplication::GetInstance()->GetApplicationWindow().get_gc()
#endif




