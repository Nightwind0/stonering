#ifndef SR2_APPLICATION_H
#define SR2_APPLICATION_H

#include <ClanLib/application.h>
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "Party.h"
#include "IApplication.h"
#include "ItemManager.h"
#include <queue>
#include "MapState.h"
#include "SayState.h"
#include "State.h"

namespace StoneRing 
{
    class Level;
    class Choice;





  
    class Application : public CL_ClanApplication, public IApplication
    {
    public:
	Application() ;
	~Application();
      
	virtual int main(int argc, char** argv);

	virtual CL_ResourceManager * getResources()const;
  
	virtual int getScreenWidth()const;
	virtual int getScreenHeight()const;

	virtual IParty * getParty() const;
	virtual ICharacterGroup * getSelectedCharacterGroup() const;
	virtual LevelFactory *getLevelFactory() const;
	virtual ItemFactory * getItemFactory() const ;
	virtual AbilityFactory * getAbilityFactory() const ;
	virtual const AbilityManager * getAbilityManager() const;
	virtual const ItemManager * getItemManager() const;
	virtual CL_Rect getDisplayRect() const;


	virtual void playAnimation(const std::string &animation);
	virtual void playSound(const std::string &sound);
	virtual void loadLevel(const std::string &level, uint startX, uint startY);
	virtual void startBattle(const std::string &monster, uint count, bool isBoss);
	virtual void say(const std::string &speaker, const std::string &text);
	virtual void pause(uint time);
	virtual void invokeShop(const std::string &shoptype);
	virtual void choice(const std::string &choiceText, const std::vector<std::string> &choices, Choice * pChoice);
	virtual void pop(bool bAll);

    private:
	void setupClanLib();
	void teardownClanLib();
	void showRechargeableOnionSplash();
	void showIntro();
	
	void loadItems(const std::string &filename);
	void loadSpells(const std::string &filename);
	void loadStatusEffects(const std::string &filename);

	void draw();
	void run();

	void startKeyUpQueue();
	void stopKeyUpQueue();

	/* SIGNALS */
	void onSignalQuit();
	void onSignalKeyDown(const CL_InputEvent &key);
	void onSignalKeyUp(const CL_InputEvent &key);
	void onSignalMovementTimer();
  
	int calc_fps(int);

	Party *mpParty;
	LevelFactory * mpLevelFactory;
	ItemFactory * mpItemFactory;
	ItemManager mItemManager;
	AbilityManager mAbilityManager;
	AbilityFactory * mpAbilityFactory;
	bool mbDone;
	CL_ResourceManager * mpResources;
	CL_DisplayWindow *mpWindow;
	CL_Timer *mpMovementTimer;
	

	/* STATES */
	MapState mMapState;
	SayState mSayState;
	std::vector<State*> mStates;

      
    };
  
  
};

extern StoneRing::Application app;

#endif
