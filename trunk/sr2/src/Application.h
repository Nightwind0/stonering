#ifndef SR2_APPLICATION_H
#define SR2_APPLICATION_H

#include <ClanLib/application.h>
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "Party.h"
#include "IApplication.h"
#include "ItemManager.h"
#include "AppUtils.h"
#include <queue>
#include "MapState.h"
#include "SayState.h"
#include "State.h"
#include "SteelInterpreter.h"
#include "ElementFactory.h"


namespace StoneRing 
{
    class Level;
    class Choice;

 
    class Application : public CL_ClanApplication, public IApplication
    {
    public:
        Application() ;
        virtual ~Application();
      
        virtual int main(int argc, char** argv);

        virtual CL_ResourceManager * getResources()const;
  
        virtual int getScreenWidth()const;
        virtual int getScreenHeight()const;
        virtual void pop(bool);
        virtual IParty * getParty() const;
        virtual ICharacterGroup * getSelectedCharacterGroup() const;
        virtual AbilityManager * getAbilityManager();
        virtual ItemManager * getItemManager();
        virtual IFactory * getElementFactory() { return &mElementFactory; }
        virtual CL_Rect getDisplayRect() const;

        virtual void requestRedraw(const State *pState);

        virtual AstScript * loadScript(const std::string &name, const std::string &script);
        virtual SteelType runScript(AstScript * pScript);
    protected:


    private:
        // Steel functions.
        SteelType playScene(const std::string &animation);
        SteelType playSound(const std::string &sound);
        SteelType loadLevel(const std::string &level, uint startX, uint startY);
        SteelType startBattle(const std::string &monster, uint count, bool isBoss);
        SteelType say(const std::string &speaker, const std::string &text);
        SteelType pause(uint time);
        SteelType invokeShop(const std::string &shoptype);
        SteelType choice(const std::string &choiceText, const std::vector<SteelType> &choices);
        SteelType pop_(bool bAll);
        SteelType giveNamedItem(const std::string &item, uint count);
        SteelType getGold();
        SteelType hasItem(const std::string&name, uint count);
        SteelType hasGeneratedWeapon(const std::string &wepclass, const std::string &webtype);
        SteelType hasGeneratedArmor(const std::string &armclass, const std::string &armtype);
        SteelType didEvent(const std::string &event);
        SteelType doEvent(const std::string &event, bool bRemember);
        SteelType takeNamedItem(const std::string &item, uint count);
        // You can also take
        SteelType giveGold(int amount);
        // SteelType giveGeneratedWeapon(const std::string &wepclass, const std::string &weptype);
        // SteelType giveGeneratedArmor(const std::string &armclass, const std::string &armtype);

        void setupClanLib();
        void teardownClanLib();
        void showRechargeableOnionSplash();
        void showIntro();
        void showError(int line, const std::string &script, const std::string &message);
        void registerSteelFunctions();
    
        void draw();
        void run();

        void startKeyUpQueue();
        void stopKeyUpQueue();

        /* SIGNALS */
        void onSignalQuit();
        void onSignalKeyDown(const CL_InputEvent &key);
        void onSignalKeyUp(const CL_InputEvent &key);
  
        int calc_fps(int);

        Party *mpParty;
        ItemManager mItemManager;
        AbilityManager mAbilityManager;
        SteelInterpreter mInterpreter;
        ElementFactory mElementFactory;
        bool mbDone;
        CL_ResourceManager * mpResources;
        CL_DisplayWindow *mpWindow;

        std::string mGold;
        //CL_Timer *mpMovementTimer;
        //CL_Mutex mMovementMutex;

        /* STATES */
        MapState mMapState;
        SayState mSayState;
        std::vector<State*> mStates;
        std::vector<IFactory*> mFactories;
        AppUtils mAppUtils;
      
    };
  
  
};

extern StoneRing::Application app;

#endif






