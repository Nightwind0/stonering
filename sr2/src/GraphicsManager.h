#ifndef SR_GRAPHICS_MANAGER
#define SR_GRAPHICS_MANAGER


#include "Application.h"
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <string>
#include <map>


namespace StoneRing 
{
  
  class GraphicsManager
    {
    public:
      static GraphicsManager * getInstance();
      
      CL_Sprite * createSprite ( const std::string & name );
      CL_Surface * getTileMap ( const std::string & name );
      
      
    private:
      std::map<std::string,CL_Surface *> mTileMap;
      
      static GraphicsManager *mInstance;
      GraphicsManager();
      ~GraphicsManager();
      
    };
  
};

#endif
