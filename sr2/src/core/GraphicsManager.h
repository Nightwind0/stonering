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
        static GraphicsManager * GetInstance();

        enum Overlay
        {
            CHOICE,
            BATTLE_STATUS,
            BATTLE_MENU,
            SAY,
            MAIN_MENU,
            SHOP
        };
      
        CL_Sprite * CreateSprite ( const std::string & name );
        // TODO: Replace sprite string in these two APIs with some enum
        CL_Sprite * CreateMonsterSprite ( const std::string &monster, const std::string &sprite);
        CL_Sprite * CreateCharacterSprite ( const std::string &player, const std::string &sprite);
        CL_Surface * GetTileMap ( const std::string & name );
        CL_Surface * GetBackdrop (const std::string & name );

        // Returns the name associated with this surface
        std::string LookUpMapWithSurface( CL_Surface * );

        CL_Font * GetFont(const std::string &name);
      
    private:
        std::map<std::string,CL_Surface *> m_tile_map;
        std::map<std::string,CL_Font*> m_font_map;
      
      
        static GraphicsManager *m_pInstance;
        GraphicsManager();
        ~GraphicsManager();
      
    };
  
};

#endif




