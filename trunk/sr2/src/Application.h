#ifndef SR2_APPLICATION_H
#define SR2_APPLICATION_H

#include <ClanLib/application.h>
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include "Party.h"




namespace StoneRing 
{
class Level;
  
  class Application : public CL_ClanApplication
    {
    public:
      Application() ;
      ~Application();
      
      virtual int main(int argc, char** argv);

      CL_ResourceManager * getResources();
  
      static const int WINDOW_HEIGHT = 600;
      static const int WINDOW_WIDTH = 800;

      static Application * getApplication();

    private:

      int mCurX;
      int mCurY;


      void setupClanLib();
      void teardownClanLib();
      void showRechargeableOnionSplash();
      void showIntro();
      
      CL_ResourceManager * mpResources;

      CL_DisplayWindow *mpWindow;


      Level * mpLevel;
      
    };
  
  
};

extern StoneRing::Application app;

#endif
