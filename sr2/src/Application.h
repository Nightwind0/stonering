#ifndef SR2_APPLICATION_H
#define SR2_APPLICATION_H

#include <ClanLib/application.h>
#include <ClanLib/core.h>

namespace StoneRing 
{
  
  class Application : public CL_ClanApplication
    {
    public:
      Application() ;
      ~Application();
      
      virtual int main(int argc, char** argv);
      
      void setupClanLib();
      void teardownClanLib();
      
      
      static const int WINDOW_HEIGHT = 600;
      static const int WINDOW_WIDTH = 800;
      
    };
  
  
};

extern StoneRing::Application app;

#endif
