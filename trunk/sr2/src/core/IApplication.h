#ifndef SR_IAPPLICATION_H
#define SR_IAPPLICATION_H


#include <ClanLib/core.h>
#include <ClanLib/display.h>
#ifdef _WINDOWS_
#include <SteelType.h>
#else
#include <steel/SteelType.h>
#endif
#include "IParty.h"
#include "AbilityManager.h"
#include "MonsterGroup.h"
#include "ElementFactory.h"


class AstScript;

namespace StoneRing
{

    class LevelFactory;
    class ItemFactory;
    class ItemManager;
    class CharacterManager;
    class Choice;
    class State;
    class MonsterRef;
    class BattleConfig;

    class IApplication
    {
    public:
	enum Button{
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
	
	
	enum Utility {
	    XP_FOR_LEVEL,
	    LEVEL_FOR_XP,
            ON_ATTACK
	};

	static IApplication * GetInstance();
        virtual ~IApplication(){}
        virtual CL_ResourceManager&  GetResources()=0;
        virtual CL_DisplayWindow& GetApplicationWindow()=0;
        virtual IParty * GetParty() const=0;
       // virtual IParty * GetReserveParty() const=0;
        virtual AbilityManager * GetAbilityManager() = 0;
        virtual ItemManager * GetItemManager() = 0;
        virtual IFactory * GetElementFactory() = 0;
        virtual CharacterManager * GetCharacterManager() = 0;
        virtual void StartBattle(const MonsterGroup &group, const std::string &backdrop)=0;
	virtual void PopLevelStack(bool popAll)=0;
	virtual void MainMenu()=0;
	virtual void LoadMainMenu(CL_DomDocument&)=0;
        virtual CL_Rect GetDisplayRect() const=0;
        virtual void RequestRedraw(const State *pState)=0;
        virtual void RunState(State *pState)=0;
        virtual AstScript * LoadScript(const std::string &name, const std::string &script)=0;
        virtual SteelType RunScript(AstScript * pScript)=0;
        virtual SteelType RunScript(AstScript *pScript, const ParameterList &params)=0;
	virtual AstScript * GetUtility(Utility util)const=0;

    };

}

//#define  GET_MAIN_GC ()   CL_GraphicContext GC = IApplication::GetInstance()->GetApplicationWindow().get_gc()
#define GET_MAIN_GC() IApplication::GetInstance()->GetApplicationWindow().get_gc()
#endif




