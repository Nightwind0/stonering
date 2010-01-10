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

        CL_Sprite * CreateSprite ( const std::string& name );
        CL_Surface * GetOverlay( Overlay overlay );

        // TODO: Replace sprite string in these two APIs with some enum
        CL_Sprite * CreateMonsterSprite ( const std::string& monster, const std::string& sprite);
        CL_Sprite * CreateCharacterSprite ( const std::string& player, const std::string& sprite);
        CL_Surface * GetTileMap ( const std::string& name );
        CL_Surface * GetBackdrop (const std::string& name );
        CL_Surface * GetIcon ( const std::string& icon );

        // Returns the name associated with this surface
        std::string LookUpMapWithSurface( CL_Surface * );

        CL_Font * GetFont(const std::string& name);
        CL_Font * GetFont( Overlay overlay, const std::string& type );

    private:

        static std::string NameOfOverlay(Overlay overlay);
        std::map<Overlay,CL_Surface*> m_overlay_map;
        std::map<std::string,CL_Surface *> m_tile_map;
        std::map<std::string,CL_Font*> m_font_map;
        std::map<std::string,CL_Surface*> m_icon_map;
        std::map<Overlay,std::map<std::string,std::string> > m_overlay_font_map;

        static GraphicsManager *m_pInstance;
        GraphicsManager();
        ~GraphicsManager();

    };

};

#endif




