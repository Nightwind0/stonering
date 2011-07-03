#ifndef SR_GRAPHICS_MANAGER
#define SR_GRAPHICS_MANAGER



#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <string>
#include <map>
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
            SKILL_TREE
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
	static void initialize();
        static CL_Sprite  CreateSprite ( const std::string& name );
	static CL_Image   CreateImage ( const std::string& name );
        static CL_Image  GetOverlay( Overlay overlay );

        // TODO: Replace sprite string in these two APIs with some enum
        static CL_Sprite  CreateMonsterSprite ( const std::string& monster, const std::string& sprite);
        static CL_Sprite  CreateCharacterSprite ( const std::string& player, const std::string& sprite);
	static CL_Sprite  CreateEquipmentSprite ( EquipmentSpriteType type, const std::string& sprite_name); 
        static CL_Sprite  GetTileMap ( const std::string& name );
        static CL_Image  GetBackdrop (const std::string& name );
        static CL_Image  GetIcon ( const std::string& icon );
	static CL_Sprite GetPortraits ( const std::string& character);


        // Returns the name associated with this surface
        static std::string LookUpMapWithSprite( CL_Sprite );

        static Font  GetFont(const std::string& name);
	static Font  GetFont( Overlay overlay, const std::string& type );
        static std::string GetFontName ( Overlay overlay, const std::string& type );
        static std::string GetFontName ( DisplayFont font );
        
        static CL_Colorf GetColor ( Overlay overlay, const std::string& name );
        static CL_Pointf GetPoint ( Overlay overlay, const std::string& name );
        static CL_Rectf GetRect ( Overlay overlay, const std::string& name );
        static CL_Gradient GetGradient ( Overlay overlay, const std::string& name );
        static CL_Sprite GetSprite ( Overlay overlay, const std::string& name );
        static CL_Image  GetImage ( Overlay overlay, const std::string& name );
        static CL_Gradient GetMenuGradient();
        static CL_Pointf   GetMenuInset();
    private:

        static std::string NameOfOverlay(Overlay overlay);
        static std::string NameOfDisplayFont(DisplayFont font);
		Font LoadFont(const std::string& name);
        std::map<Overlay,CL_Image> m_overlay_map;
        std::map<std::string,CL_Sprite> m_tile_map;
        std::map<std::string,Font> m_font_map;
        std::map<std::string,CL_Image> m_icon_map;
        std::map<Overlay,std::map<std::string,std::string> > m_overlay_font_map;

        static GraphicsManager *m_pInstance;
        GraphicsManager();
        ~GraphicsManager();

    };

};

#endif




