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
  
#ifdef _MSC_VER
      enum { WINDOW_HEIGHT=600};
      enum { WINDOW_WIDTH=800};
#else
      static const int WINDOW_HEIGHT = 600 ;
      static const int WINDOW_WIDTH = 800;
#endif

      static Application * getApplication();

      CL_Rect getLevelRect() const;
      CL_Rect getDisplayRect() const;

    private:

      int mCurX;
      int mCurY;

      bool mbDone;

      void setupClanLib();
      void teardownClanLib();
      void showRechargeableOnionSplash();
      void showIntro();

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
