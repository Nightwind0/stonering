#ifndef SR2_APPLICATION_H
#define SR2_APPLICATION_H

#include <ClanLib/application.h>
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "IParty.h"
#include "IApplication.h"



namespace StoneRing 
{
class Level;
  
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
      virtual CL_Rect getLevelRect() const;
      virtual CL_Rect getDisplayRect() const;


      virtual bool canMove(const CL_Rect &currently, const CL_Rect &destination, bool noHot);

    private:

      IParty *mpParty;
      LevelFactory * mpLevelFactory;


      int mCurX;
      int mCurY;

      bool mbDone;

      int mSpeed;

      void setupClanLib();
      void teardownClanLib();
      void showRechargeableOnionSplash();
      void showIntro();


      enum eDir{NORTH,SOUTH,EAST,WEST};

      bool move(eDir dir, int times=1);

      /* SIGNALS */
      void onSignalQuit();
      void onSignalKeyDown(const CL_InputEvent &key);
      //      void onSignalKeyUp();
      
      CL_ResourceManager * mpResources;

      CL_DisplayWindow *mpWindow;


      Level * mpLevel;
      
    };
  
  
};

extern StoneRing::Application app;

#endif
