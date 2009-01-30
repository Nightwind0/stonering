#ifndef SR2_APPLICATION_H
#define SR2_APPLICATION_H

#include <ClanLib/application.h>
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "Party.h"
#include "IApplication.h"
#include "ItemManager.h"
#include "CharacterManager.h"
#include "AppUtils.h"
#include <queue>
#include "MapState.h"
#include "SayState.h"
#include "BattleState.h"
#include "State.h"
#ifdef _WINDOWS_
#include <SteelInterpreter.h>
#else
#include <steel/SteelInterpreter.h>
#endif
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

        virtual CL_ResourceManager * GetResources()const;
  
        virtual int GetScreenWidth()const;
        virtual int GetScreenHeight()const;
        virtual void Pop(bool);
        virtual IParty * GetParty() const;
        virtual AbilityManager * GetAbilityManager();
        virtual ItemManager * GetItemManager();
        virtual IFactory * GetElementFactory() { return &mElementFactory; }
        virtual CharacterManager * GetCharacterManager() { return &mCharacterManager; }
        virtual CL_Rect GetDisplayRect() const;
        virtual ICharacterGroup * GetTargetCharacterGroup() const;
        virtual ICharacterGroup * GetActorCharacterGroup() const;
        virtual void StartBattle(const MonsterGroup &group,const std::string &backdrop);
        virtual void RequestRedraw(const State *pState);

        virtual AstScript * LoadScript(const std::string &name, const std::string &script);
        virtual SteelType RunScript(AstScript * pScript);
        virtual SteelType RunScript(AstScript *pScript, const ParameterList &params); 
    protected:


    private:
        // Steel functions.
        SteelType playScene(const std::string &animation);
        SteelType playSound(const std::string &sound);
        SteelType loadLevel(const std::string &level, uint startX, uint startY);
        SteelType startBattle(const std::string &monster, uint count, bool isBoss, const std::string &backdrop);
        SteelType say(const std::string &speaker, const std::string &text);
        SteelType pause(uint time);
        SteelType invokeShop(const std::string &shoptype);
        SteelType choice(const std::string &choiceText, const std::vector<SteelType> &choices);
        SteelType pop_(bool bAll);
        SteelType giveNamedItem(const std::string &item, uint count);
        SteelType getGold();
        SteelType hasItem(const std::string&name, uint count);
        SteelType hasGeneratedWeapon(const std::string &wepclass, const std::string &weptype);
        SteelType hasGeneratedArmor(const std::string &armclass, const std::string &armtype);
        SteelType didEvent(const std::string &event);
        SteelType doEvent(const std::string &event, bool bRemember);
        SteelType takeNamedItem(const std::string &item, uint count);
        SteelType useItem();
        SteelType addCharacter(const std::string &character, int level, bool announce);
        SteelType getPartyCount(void);
        SteelType getCharacter(uint index);
        // You can also take
        SteelType giveGold(int amount);
        // SteelType giveGeneratedWeapon(const std::string &wepclass, const std::string &weptype);
        // SteelType giveGeneratedArmor(const std::string &armclass, const std::string &armtype);

        // Character steel bindings
        SteelType getCharacterName(const SteelType::Handle hICharacter);
        SteelType getCharacterLevel(const SteelType::Handle hICharacter);
        SteelType addStatusEffect(SteelType::Handle hCharacter, const std::string &effect);
        SteelType removeStatusEffects(SteelType::Handle hCharacter, const std::string &effect);
        // This method directly affects a character's HP. No factors are taken into account
        // The handle is to an ICharacter
        SteelType doDamage(SteelType::Handle hICharacter, int damage);

        SteelType hasEquipment(SteelType::Handle hCharacter, int slot);
        SteelType getEquipment(SteelType::Handle hCharacter, int slot);
        SteelType equip(SteelType::Handle hCharacter, int slot, const std::string &);

        void steelConst(const std::string &name, int value);
        void steelConst(const std::string &name, double value);

        void setupClanLib();
        void teardownClanLib();
        void showRechargeableOnionSplash();
        void showIntro();
        void showError(int line, const std::string &script, const std::string &message);
        void registerSteelFunctions();
    
        void draw();
        void run();
        void loadscript(std::string &o_str, const std::string & filename);

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
        CharacterManager mCharacterManager;
        SteelInterpreter mInterpreter;
        ElementFactory mElementFactory;
        bool mbDone;
        CL_ResourceManager * mpResources;
        CL_DisplayWindow *mpWindow;

        std::string mGold;

        /* STATES */
        MapState mMapState;
        SayState mSayState;
        BattleState mBattleState;
        std::vector<State*> mStates;
        std::vector<IFactory*> mFactories;
        AppUtils mAppUtils;
      
    };

inline void Application::steelConst(const std::string &name, int value)
{
    SteelType val;
    val.set(value);
    mInterpreter.declare_const(name,val);
}

inline void Application::steelConst(const std::string &name, double value)
{
    SteelType val;
    val.set(value);
    mInterpreter.declare_const(name,val);
}
  
  
};



extern StoneRing::Application app;

#endif






