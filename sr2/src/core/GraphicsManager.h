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
            BATTLE_POPUP_MENU,
            SAY,
            MAIN_MENU,
            SHOP
        };

        enum DisplayFont
        {
          DISPLAY_HP_POSITIVE,
          DISPLAY_HP_NEGATIVE,
          DISPLAY_MP_POSITIVE,
          DISPLAY_MP_NEGATIVE,
          DISPLAY_MISS
        };

        CL_Sprite  CreateSprite ( const std::string& name );
        CL_Image  GetOverlay( Overlay overlay );

        // TODO: Replace sprite string in these two APIs with some enum
        CL_Sprite  CreateMonsterSprite ( const std::string& monster, const std::string& sprite);
        CL_Sprite  CreateCharacterSprite ( const std::string& player, const std::string& sprite);
        CL_Sprite  GetTileMap ( const std::string& name );
        CL_Image  GetBackdrop (const std::string& name );
        CL_Image  GetIcon ( const std::string& icon );

        // Returns the name associated with this surface
        std::string LookUpMapWithSprite( CL_Sprite );

        CL_Font  GetFont(const std::string& name);
        CL_Font  GetFont( Overlay overlay, const std::string& type );
        CL_Font  GetDisplayFont( DisplayFont font );
        CL_Colorf GetFontColor ( DisplayFont font );

    private:

        static std::string NameOfOverlay(Overlay overlay);
        static std::string NameOfDisplayFont(DisplayFont font);
        std::map<Overlay,CL_Image> m_overlay_map;
        std::map<std::string,CL_Sprite> m_tile_map;
        std::map<std::string,CL_Font> m_font_map;
        std::map<std::string,CL_Image> m_icon_map;
        std::map<Overlay,std::map<std::string,std::string> > m_overlay_font_map;
        std::map<DisplayFont,CL_Font> m_display_font_map;
        std::map<DisplayFont,CL_Colorf> m_display_font_colors;
        static GraphicsManager *m_pInstance;
        GraphicsManager();
        ~GraphicsManager();

    };

};

#endif




