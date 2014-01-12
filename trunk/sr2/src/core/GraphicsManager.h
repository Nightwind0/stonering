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
        BANNER,
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
        STATUS,
        GAMEOVER,
        ITEM_GET,
        GOLD_GET,
        SKILL_GET,
        ITEM_GET_SINGLE
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
    static void       SetTheme ( const std::string & theme );
    static clan::Sprite  CreateSprite ( const std::string& name, bool add_category = true );
    static clan::Image   CreateImage ( const std::string& name );
    static clan::Image   GetOverlay ( Overlay overlay );

    // TODO: Replace sprite string in these two APIs with some enum
    static clan::Sprite  CreateMonsterSprite ( const std::string& monster, const std::string& sprite );
    static clan::Sprite  CreateCharacterSprite ( const std::string& player, const std::string& sprite );
    static clan::Sprite  CreateEquipmentSprite ( EquipmentSpriteType type, const std::string& sprite_name );
    static clan::Texture2D   GetTileMap ( const std::string& name );
    static clan::Image   GetBackdrop ( const std::string& name );
    static clan::Image   GetIcon ( const std::string& icon );
    static clan::Sprite  GetPortraits ( const std::string& character );
    static clan::Sprite  GetSpriteWithImage ( const clan::Image image );


    // Returns the name associated with this surface
    static std::string LookUpMapWithImage ( clan::Texture );

    static Font        GetFont ( const std::string& name );
    static Font        GetFont ( Overlay overlay, const std::string& type );
    static std::string GetFontName ( Overlay overlay, const std::string& type );
    static std::string GetFontName ( DisplayFont font );

    static clan::Colorf   GetColor ( Overlay overlay, const std::string& name );
    static clan::Pointf   GetPoint ( Overlay overlay, const std::string& name );
    static clan::Rectf    GetRect ( Overlay overlay, const std::string& name );
    static clan::Gradient GetGradient ( Overlay overlay, const std::string& name );
    static clan::Sprite   GetSprite ( Overlay overlay, const std::string& name );
    static clan::Image    GetImage ( Overlay overlay, const std::string& name );
    static clan::Gradient GetMenuGradient();
    static clan::Pointf   GetMenuInset();
    static std::string GetThemeName();
    static void        GetAvailableThemes ( std::list<std::string>& o_themes );

    static void        GetComplementaryColors ( const clan::Colorf& color, clan::Colorf& one, clan::Colorf& two );
private:

    struct Theme
    {
        enum ColorRef { COLOR_MAIN, COLOR_HP, COLOR_MP, COLOR_SP, COLOR_ACCENT, COLOR_SHADOW, COLOR_COUNT };
        clan::Gradient m_menu_gradient;
        clan::Colorf   m_colors[COLOR_COUNT];
    };

    struct HSVColor
    {
        float h;
        float s;
        float v;
    };

    static std::string        NameOfOverlay ( Overlay overlay );
    static std::string        NameOfDisplayFont ( DisplayFont font );
    static Theme::ColorRef    GetColorRef ( const std::string& color_name );
    static HSVColor           RGBToHSV ( const clan::Colorf& );
    static clan::Colorf          HSVToRGB ( const HSVColor& );
    static float              RotateHue ( const float hue, const float angle_rads );
    static clan::Colorf          GetOppositeColor ( const clan::Colorf& );
    static clan::Colorf          GetTriadic ( const clan::Colorf&, bool left );
    static clan::Colorf          GetSplit ( const clan::Colorf&, bool left );
    static clan::Colorf          GetAnalog ( const clan::Colorf&, bool left );
    static clan::Colorf          GetComplement ( const clan::Colorf&, float degs );
    Font                      LoadFont ( const std::string& name );
    Theme                     LoadTheme ( const std::string& name );
    static clan::Colorf          LoadColor ( clan::ResourceManager& resources, const std::string& path );

    std::map<Overlay,clan::Image>      m_overlay_map;
    std::map<std::string,clan::Texture2D>  m_tile_map;
    std::map<std::string,Font>      m_font_map;
    std::map<std::string,clan::Image>  m_icon_map;
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




