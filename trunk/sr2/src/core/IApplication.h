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
#include "AbilityFactory.h"
#include "AbilityManager.h"
#include "SteelInterpreter.h"


class AstScript;

namespace StoneRing
{

    class LevelFactory;
    class ItemFactory;
    class ItemManager;
    class CharacterFactory;
    class Choice;
    class State;

    class IApplication 
    {
    public:

        virtual ~IApplication(){}
        virtual CL_ResourceManager * getResources()const=0;
        virtual IParty * getParty() const=0;
        virtual ICharacterGroup * getSelectedCharacterGroup() const = 0;
        virtual AbilityManager * getAbilityManager() = 0;
        virtual ItemManager * getItemManager() = 0;
        virtual IFactory * getElementFactory() = 0;
        static IApplication * getInstance();
        virtual void pop(bool popAll)=0;

        virtual int getScreenWidth()const=0;
        virtual int getScreenHeight()const=0;

        virtual CL_Rect getDisplayRect() const=0;
        virtual void requestRedraw(const State *pState)=0;
        virtual AstScript * loadScript(const std::string &name, const std::string &script)=0;
        virtual SteelType runScript(AstScript * pScript)=0;
        virtual SteelType runScript(AstScript *pScript, const ParameterList &params)=0; 

    };

};
#endif




