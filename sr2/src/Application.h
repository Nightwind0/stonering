#ifndef SR2_APPLICATION_H
#define SR2_APPLICATION_H

#include <ClanLib/application.h>
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "Party.h"
#include "IApplication.h"
#include "ItemManager.h"
#include <queue>


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
      virtual inline CL_Rect getLevelRect() const;
      virtual CL_Rect getDisplayRect() const;


      virtual void playAnimation(const std::string &animation);
      virtual void playSound(const std::string &sound);
      virtual void loadLevel(const std::string &level, uint startX, uint startY);
      virtual void startBattle(const std::string &monster, uint count, bool isBoss);
      virtual void say(const std::string &speaker, const std::string &text);
      virtual void pause(uint time);
      virtual void invokeShop(const std::string &shoptype);
      virtual void choice(const std::string &choiceText, const std::vector<std::string> &choices, Choice * pChoice);


    private:

  
      int calc_fps(int);


	


      enum eState 
	  {
	      INTRO,
	      MAIN,
	      TALKING,
	      MENU,
	      CHOOSING_MO,
	      BATTLE,
	      CHOICE
	  };

      Party *mpParty;
      LevelFactory * mpLevelFactory;
      ItemFactory * mpItemFactory;
      ItemManager mItemManager;
      AbilityManager mAbilityManager;
      AbilityFactory * mpAbilityFactory;

      int mLevelX; // Offset into level
      int mLevelY;

      bool mbDone;
	  bool mbStep;
      bool mbMoveFast;

      void setupClanLib();
      void teardownClanLib();
      void showRechargeableOnionSplash();
      void showIntro();
      void loadFonts();
      void loadItems(const std::string &filename);
      void loadSpells(const std::string &filename);
	  void loadStatusEffects(const std::string &filename);


      void processActionQueue();

      void drawPlayer();

      void drawMap();

      void recalculatePlayerPosition(IParty::eDirection dir);

      bool movePlayer();

      void doTalk(bool prod=false);

      void startKeyUpQueue();
      void stopKeyUpQueue();

      /* SIGNALS */
      void onSignalQuit();
      void onSignalKeyDown(const CL_InputEvent &key);
      void onSignalKeyUp(const CL_InputEvent &key);
      void onSignalMovementTimer();


      CL_ResourceManager * mpResources;

      CL_DisplayWindow *mpWindow;


      Level * mpLevel;
      bool mbShowDebug;

      CL_Sprite *mpPlayerSprite;

	  CL_Font *mpfSBBlack;
      CL_Font *mpfBWhite;
      CL_Font *mpfBPowderBlue;
      CL_Font *mpfBGray;
      CL_Timer *mpMovementTimer;
      CL_Surface *mpSayOverlay;

      bool mbHasNextDirection;
      IParty::eDirection meNextDirection;
      ushort mnSkippedMoves;
      eState meState;

#ifndef NDEBUG
      CL_Rect mLastTalkRect;
	  bool mbShowLevelCenter;
#endif
      bool mbQueueKeyUps;
      std::queue<int> mKeyUpQueue;

	  bool mbDraw;
      
    };
  
  
};

extern StoneRing::Application app;

#endif
