#ifndef SR2_APPLICATION_H
#define SR2_APPLICATION_H

#include <ClanLib/application.h>
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "Party.h"
#include "IApplication.h"
#include <queue>


namespace StoneRing 
{
class Level;


  
 class GameAction 
     {
      public:
	 GameAction();
	 virtual ~GameAction();


	 enum Type
	     {
		 SAY,
		 LOAD_LEVEL,
		 PAUSE,
		 INVOKE_SHOP,
		 PLAY_ANIMATION,
		 PLAY_SOUND,
		 START_BATTLE
	     };

	 virtual Type getType() const =0;
     private:
     }; 

 class SayAction : public GameAction
     {
     public:
	 SayAction(const std::string & speaker, 
		   const std::string & text);
	 ~SayAction();

	 std::string getSpeaker() const ;
	 std::string getText() const;

	 virtual Type getType() const { return SAY;}

     private:
	 std::string mSpeaker;
	 std::string mText;
     };

 class LoadLevelAction : public GameAction

     {
     public:
	 LoadLevelAction(const std::string & level,
			 uint startx, uint starty);
	 ~LoadLevelAction();

	 std::string getLevel() const;
	 uint getStartX() const;
	 uint getStartY() const;

	 virtual Type getType() const { return LOAD_LEVEL;}

     private:
	 uint mStartX;
	 uint mStartY;
	 std::string mLevel;
     };

 class StartBattleAction : public GameAction
     {
     public:
	 StartBattleAction(const std::string &monster,
			   uint count, bool boss);
	 ~StartBattleAction();
	 
	 virtual Type getType() const { return START_BATTLE;}
	 std::string getMonster() const;
	 uint getCount() const;
	 bool isBoss() const;
	 
     private:
	 std::string mMonster;
	 uint mCount;
	 bool mbBoss;
	     
     };

 class PlayAnimationAction : public GameAction
     {
     public:
	 PlayAnimationAction(const std::string &animation);
	 ~PlayAnimationAction();

	 virtual Type getType() const { return PLAY_ANIMATION;}

	 std::string getAnimation() const;
     private:
	 std::string mAnimation;
     };

 class PlaySoundAction : public GameAction
     {
     public:
	 PlaySoundAction(const std::string &sound);
	 ~PlaySoundAction();

	 virtual Type getType() const { return PLAY_SOUND;}

	 std::string getSound() const;
     private:
	 std::string mSound;
     };

 class PauseAction : public GameAction
     {
     public:
	 PauseAction(uint time);
	 ~PauseAction();

	 virtual Type getType() const { return PAUSE;}

	 uint getTime() const;
     private:
	 uint mTime;
     };

 class InvokeShopAction : public GameAction
     {
     public:
	 InvokeShopAction(const std::string &shop);
	 ~InvokeShopAction();

	 virtual Type getType() const { return INVOKE_SHOP;}

	 std::string getShopType() const;
     private:
	 std::string mShopType;
     };




  
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
      virtual LevelFactory *getLevelFactory() const;
      virtual inline CL_Rect getLevelRect() const;
      virtual CL_Rect getDisplayRect() const;


      virtual bool canMove(const CL_Rect &currently, const CL_Rect &destination, bool noHot, bool isPlayer);


      virtual void playAnimation(const std::string &animation);
      virtual void playSound(const std::string &sound);
      virtual void loadLevel(const std::string &level, uint startX, uint startY);
      virtual void startBattle(const std::string &monster, uint count, bool isBoss);
      virtual void say(const std::string &speaker, const std::string &text);
      virtual void pause(uint time);
      virtual void invokeShop(const std::string &shoptype);



    private:

      enum eDir{NORTH,SOUTH,EAST,WEST};
	  int calc_fps(int);



      enum eState 
	  {
	      INTRO,
	      MAIN,
	      TALKING,
	      MENU,
	      CHOOSING_MO,
	      BATTLE
	  };

      Party *mpParty;
      LevelFactory * mpLevelFactory;


      int mCurX;
      int mCurY;

      int mLevelX;
      int mLevelY;

      eDir mPlayerDir;

      bool mbDone;

      int mSpeed;

      void setupClanLib();
      void teardownClanLib();
      void showRechargeableOnionSplash();
      void showIntro();


      void processActionQueue();




      void recalculatePlayerPosition(eDir dir);

      bool move(eDir dir, int times=1);

      /* SIGNALS */
      void onSignalQuit();
      void onSignalKeyDown(const CL_InputEvent &key);
      //      void onSignalKeyUp();
      
      CL_ResourceManager * mpResources;

      CL_DisplayWindow *mpWindow;


      Level * mpLevel;

      std::queue<GameAction*> mActionQueue;

      bool mbPauseMovement;
      
    };
  
  
};

extern StoneRing::Application app;

#endif
