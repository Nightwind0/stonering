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
  
      static const int WINDOW_HEIGHT = 600;
      static const int WINDOW_WIDTH = 800;

    private:




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
