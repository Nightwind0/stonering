#ifndef SR_GRAPHICS_MANAGER
#define SR_GRAPHICS_MANAGER



#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <string>
#include <map>
#include <list>
#include "Font.h"

namespace StoneRing
{


    class GraphicsManager
    {
    public:

        enum Overlay
        {
            CHOICE,
            BATTLE_STATUS,
            BATTLE_MENU,
            BATTLE_POPUP_MENU,
            SAY,
	    EXPERIENCE,
	    LOOT,
            MAIN_MENU,
            SHOP,
            DYNAMIC_MENU,
	    ITEMS,
            SKILL_TREE,
            EQUIP,
            SAVE_LOAD,
            STARTUP,
            STATUS
        };

        enum DisplayFont
        {
          DISPLAY_HP_POSITIVE,
          DISPLAY_HP_NEGATIVE,
          DISPLAY_MP_POSITIVE,
          DISPLAY_MP_NEGATIVE,
          DISPLAY_MISS,
	  DISPLAY_FONT_SHADOW,
        };
	
	enum EquipmentSpriteType
	{
	    EQUIPMENT_SPRITE_WEAPON,
	    EQUIPMENT_SPRITE_ARMOR
	};
	static void       initialize();
        static void       SetTheme(const std::string & theme);
        static CL_Sprite  CreateSprite ( const std::string& name );
	static CL_Image   CreateImage ( const std::string& name );
        static CL_Image   GetOverlay( Overlay overlay );

        // TODO: Replace sprite string in these two APIs with some enum
        static CL_Sprite  CreateMonsterSprite ( const std::string& monster, const std::string& sprite);
        static CL_Sprite  CreateCharacterSprite ( const std::string& player, const std::string& sprite);
	static CL_Sprite  CreateEquipmentSprite ( EquipmentSpriteType type, const std::string& sprite_name); 
        static CL_Sprite  GetTileMap ( const std::string& name );
        static CL_Image   GetBackdrop (const std::string& name );
        static CL_Image   GetIcon ( const std::string& icon );
	static CL_Sprite  GetPortraits ( const std::string& character);
        static CL_Sprite  GetSpriteWithImage(const CL_Image image);


        // Returns the name associated with this surface
        static std::string LookUpMapWithSprite( CL_Sprite );

        static Font        GetFont(const std::string& name);
	static Font        GetFont( Overlay overlay, const std::string& type );
        static std::string GetFontName ( Overlay overlay, const std::string& type );
        static std::string GetFontName ( DisplayFont font );
        
        static CL_Colorf   GetColor ( Overlay overlay, const std::string& name );
        static CL_Pointf   GetPoint ( Overlay overlay, const std::string& name );
        static CL_Rectf    GetRect ( Overlay overlay, const std::string& name );
        static CL_Gradient GetGradient ( Overlay overlay, const std::string& name );
        static CL_Sprite   GetSprite ( Overlay overlay, const std::string& name );
        static CL_Image    GetImage ( Overlay overlay, const std::string& name );
        static CL_Gradient GetMenuGradient();
        static CL_Pointf   GetMenuInset();
        static std::string GetThemeName();
        static void        GetAvailableThemes(std::list<std::string>& o_themes);
    private:
        
        struct Theme {
            enum ColorRef { COLOR_MAIN, COLOR_HP, COLOR_MP, COLOR_SP, COLOR_ACCENT, COLOR_SHADOW, COLOR_COUNT };
            CL_Gradient m_menu_gradient;
            CL_Colorf   m_colors[COLOR_COUNT];
        };

        static std::string        NameOfOverlay(Overlay overlay);
        static std::string        NameOfDisplayFont(DisplayFont font);
        static Theme::ColorRef    GetColorRef(const std::string& color_name);        
        Font                      LoadFont(const std::string& name);
        Theme                     LoadTheme(const std::string& name);
        static CL_Colorf          LoadColor(CL_ResourceManager& resources, const std::string& path);

        std::map<Overlay,CL_Image>      m_overlay_map;
        std::map<std::string,CL_Sprite> m_tile_map;
        std::map<std::string,Font>      m_font_map;
        std::map<std::string,CL_Image>  m_icon_map;
        std::map<Overlay,std::map<std::string,std::string> > m_overlay_font_map;
        std::map<std::string,Theme>     m_theme_map;
        Theme                           m_theme;
        std::string                     m_theme_name;

        static GraphicsManager          *m_pInstance;
        GraphicsManager();
        ~GraphicsManager();

    };

};

#endif




