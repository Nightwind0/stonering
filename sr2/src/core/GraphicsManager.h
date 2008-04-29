#ifndef SR_GRAPHICS_MANAGER
#define SR_GRAPHICS_MANAGER



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
        // TODO: Replace sprite string in these two APIs with some enum
        CL_Sprite * createMonsterSprite ( const std::string &monster, const std::string &sprite);
        CL_Sprite * createCharacterSprite ( const std::string &player, const std::string &sprite);
        CL_Surface * getTileMap ( const std::string & name );
        CL_Surface * getBackdrop (const std::string & name );

        // Returns the name associated with this surface
        std::string lookUpMapWithSurface( CL_Surface * );

        enum eFont
        {
            FONT_SPEAKER,
            FONT_SAY_TEXT,
            FONT_CHOICE,
            FONT_OPTION,
            FONT_CURRENT_OPTION,
            __LAST_FONT__
        };

        CL_Font * getFont(eFont font);
      
    private:
        std::map<std::string,CL_Surface *> mTileMap;
        std::map<eFont,CL_Font*> mFontMap;
      
      
        static GraphicsManager *mInstance;
        GraphicsManager();
        ~GraphicsManager();
      
    };
  
};

#endif




