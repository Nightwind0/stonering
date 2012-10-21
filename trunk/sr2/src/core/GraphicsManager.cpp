#include "IApplication.h"
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <string>
#include <map>
#include <cmath>
#include "GraphicsManager.h"


using namespace StoneRing;
using StoneRing::Font;

#ifndef M_PI
#define M_PI 3.1415926
#endif

GraphicsManager * GraphicsManager::m_pInstance=NULL;


void GraphicsManager::initialize()
{
    if(!m_pInstance){
	m_pInstance = new GraphicsManager();
    }
}

CL_Gradient GraphicsManager::GetMenuGradient()
{
    return m_pInstance->m_theme.m_menu_gradient;
}

CL_Pointf GraphicsManager::GetMenuInset()
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_Resource resource = resources.get_resource("Game/MenuInset");
    
    if(resource.get_type() != "point")
        throw CL_Exception("Point resource element was not 'point' type");
    
    
    float x = atof(resource.get_element().get_attribute("x").c_str());
    float y = atof(resource.get_element().get_attribute("y").c_str());
    
    return CL_Pointf(x,y);        
}


CL_Sprite GraphicsManager::GetPortraits ( const std::string& character)
{
    CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();

    CL_Sprite  sprite(GET_MAIN_GC(),"Sprites/Portraits/" +  character, &resources);
    
    CL_Sprite clone;
    clone.clone(sprite); 
    
    sprite.set_alignment(origin_center);
    return clone;
}

CL_Image GraphicsManager::CreateImage ( const std::string& name )
{
    CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();
    
    CL_Image image(GET_MAIN_GC(),name,&resources);
    
    return image;
}


CL_Sprite GraphicsManager::CreateSprite ( const std::string & name, bool add_category )
{
    CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();

    CL_Sprite  sprite(GET_MAIN_GC(), (add_category?"Sprites/":"") +  name, &resources);
    
    CL_Sprite clone;
    clone.clone(sprite); 
    
    sprite.set_alignment(origin_center);

    return clone;
}

std::string GraphicsManager::LookUpMapWithImage (CL_Image surface)
{
    for ( std::map<std::string,CL_Image>::iterator i = m_pInstance->m_tile_map.begin();
            i != m_pInstance->m_tile_map.end();
            i++)
    {
        if ( i->second == surface)
        {
            return i->first;
        }
    }

    throw CL_Exception ( "BAD! TILEMAP NOT FOUND IN lookupMapWithSurface" );

    return "die";

}

CL_Image GraphicsManager::GetTileMap ( const std::string & name )
{
    CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();

    CL_Image surface;


    if (m_pInstance->m_tile_map.find( name ) == m_pInstance->m_tile_map.end())
    {
#ifndef NDEBUG
        std::cout << "TileMap now loading: " << name << std::endl;
#endif
        surface = CL_Image(GET_MAIN_GC(),"Tilemaps/" + name, &resources);

        m_pInstance->m_tile_map[ name ] = surface;
        return surface;
    }

    surface = m_pInstance->m_tile_map[name];

    return surface;
}

std::string GraphicsManager::NameOfOverlay(Overlay overlay)
{
    switch (overlay)
    {
    case BATTLE_STATUS:
        return "BattleStatus";
    case BATTLE_MENU:
        return "BattleMenu";
    case BATTLE_POPUP_MENU:
        return "BattlePopup";
    case CHOICE:
        return "Choice";
    case SAY:
        return "Say";
    case EXPERIENCE:
	return "Experience";
    case MAIN_MENU:
	return "MainMenu";
    case ITEMS:
	return "ItemSelect";
    case DYNAMIC_MENU:
        return "DynamicMenu";
    case SKILL_TREE:
        return "SkillTree";
    case EQUIP:
        return "Equip";
    case SHOP:
        return "Shop";
    case SAVE_LOAD:
        return "SaveLoad";
    case STARTUP:
        return "Startup";
    case STATUS:
        return "Status";
    case BANNER:
        return "Banner";
	case GAMEOVER:
		return "Gameover";
    default:
        assert(0);
    }

    assert(0);
    return "";
}

std::string GraphicsManager::NameOfDisplayFont(DisplayFont font)
{
    switch (font)
    {
    case DISPLAY_HP_POSITIVE:
        return "Font_hp_plus";
    case DISPLAY_HP_NEGATIVE:
        return "Font_hp_minus";
    case DISPLAY_MP_POSITIVE:
        return "Font_mp_plus";
    case DISPLAY_MP_NEGATIVE:
        return "Font_mp_minus";
    case DISPLAY_MISS:
        return "Font_miss";
    case DISPLAY_FONT_SHADOW:
	return "Font_display_shadow";
    default:
        assert(0);
        return "";
    }

}

CL_Sprite  GraphicsManager::CreateMonsterSprite (const std::string &monster, const std::string &sprite_id)
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_Sprite  sprite(GET_MAIN_GC(),"Sprites/Monsters/"+monster+'/'+sprite_id,&resources);


    sprite.set_alignment(origin_center);
  
    return sprite;
}

CL_Sprite GraphicsManager::CreateCharacterSprite ( const std::string &player, const std::string &sprite_id)
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_Sprite sprite = CL_Sprite(GET_MAIN_GC(),"Sprites/BattleSprites/"+player+'/'+sprite_id,&resources);


    sprite.set_alignment(origin_center);
    return sprite;
}

CL_Sprite  GraphicsManager::CreateEquipmentSprite ( EquipmentSpriteType type, const std::string& sprite_name)
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    std::string item_type;
    switch(type)
    {
	case EQUIPMENT_SPRITE_WEAPON:
	    item_type = "Weapon";
	    break;
	case EQUIPMENT_SPRITE_ARMOR:
	    item_type = "Armor";
	    break;
    }
    
    CL_Sprite sprite = CL_Sprite(GET_MAIN_GC(),"Sprites/Equipment/" + item_type + '/' + sprite_name, &resources);
    
    
    sprite.set_alignment(origin_center);
    return sprite;
}


CL_Image  GraphicsManager::GetBackdrop(const std::string &name)
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_Image surface = CL_Image(GET_MAIN_GC(),"Backdrops/" + name, &resources);

    return surface;
}

CL_Image GraphicsManager::GetOverlay(GraphicsManager::Overlay overlay)
{
    std::map<Overlay,CL_Image>::iterator foundIt = m_pInstance->m_overlay_map.find(overlay);

    if (foundIt != m_pInstance->m_overlay_map.end())
    {
        return foundIt->second;
    }
    else
    {
        CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();

        CL_Image surface( GET_MAIN_GC(),std::string("Overlays/") + NameOfOverlay(overlay) + "/overlay", &resources );

        m_pInstance->m_overlay_map [ overlay ] = surface;

        return surface;
    }
}


CL_Image GraphicsManager::GetIcon(const std::string& icon)
{
    std::map<std::string,CL_Image>::iterator foundIt = m_pInstance->m_icon_map.find(icon);

    if (foundIt != m_pInstance->m_icon_map.end())
    {
        return foundIt->second;
    }
    else
    {
        CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();
        CL_Image surface;
        try {
            surface =  CL_Image( GET_MAIN_GC(), std::string("Icons/") + icon, &resources );
        }
        catch(CL_Exception e){
            surface = CL_Image( GET_MAIN_GC(), std::string("Icons/no_icon"), &resources );
        }
        m_pInstance->m_icon_map [ icon ] = surface;

        return surface;
    }
}

StoneRing::Font  GraphicsManager::GetFont(const std::string &name)
{
    std::map<std::string,Font>::iterator foundIt = m_pInstance->m_font_map.find( name );

    if (foundIt != m_pInstance->m_font_map.end())
    {
        return foundIt->second;
    }
    else
    {
	return m_pInstance->LoadFont(name); // also puts it into map
    }
}

GraphicsManager::Theme::ColorRef GraphicsManager::GetColorRef ( const std::string& color_name )
{
    if(color_name == "color_hp")
        return Theme::COLOR_HP;
    else if(color_name == "color_mp")
        return Theme::COLOR_MP;
    else if(color_name == "color_sp")
        return Theme::COLOR_SP;
    else if(color_name == "color_main")
        return Theme::COLOR_MAIN;
    else if(color_name == "color_accent")
        return Theme::COLOR_ACCENT;
    else return Theme::COLOR_COUNT;
}


StoneRing::Font GraphicsManager::LoadFont(const std::string& name)
{
    CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();
    const std::string fontname = "Fonts/" + name;
    CL_Resource font_resource = resources.get_resource(fontname);
    
    CL_Font font;
    CL_Pointf shadow_offset;
    CL_Colorf color = CL_Colorf::white;
    if(font_resource.get_type() == "font")
    {
	 CL_Font_Sprite spritefont( GET_MAIN_GC(), fontname, &resources );
	 font = spritefont;
    }
    else if(font_resource.get_type() == "freetype")
    {
        CL_String filename = font_resource.get_element().get_attribute("file");
        CL_String fontname = font_resource.get_element().get_attribute("font_name");
        CL_String size_str = font_resource.get_element().get_attribute("size");
        CL_String color_str =  font_resource.get_element().get_attribute("color");
        Theme::ColorRef color_ref = GetColorRef(color_str);
        if(color_ref != Theme::COLOR_COUNT){
            color = m_theme.m_colors[color_ref];
        }else{
            color = CL_Colorf(color_str);
        }
	float shadowx, shadowy;
	if(font_resource.get_element().has_attribute("shadowx"))
	    shadowx = atof(font_resource.get_element().get_attribute("shadowx").c_str());
	if(font_resource.get_element().has_attribute("shadowy"))
	    shadowy = atof(font_resource.get_element().get_attribute("shadowy").c_str());	
	
	shadow_offset = CL_Pointf(shadowx,shadowy);
        int size = atoi(size_str.c_str());

        CL_IODevice file = resources.get_directory(font_resource).open_file_read(filename);
        CL_Font_Freetype thefont(filename,
                                 size,
                                 file);
	font = thefont;
    }
    else if(font_resource.get_type() == "systemfont")
    {
	CL_String fontname = font_resource.get_element().get_attribute("font_name");
	CL_String size_str = font_resource.get_element().get_attribute("size");
	CL_String color_str = font_resource.get_element().get_attribute("color");
        Theme::ColorRef color_ref = GetColorRef(color_str);
        if(color_ref != Theme::COLOR_COUNT){
            color = m_theme.m_colors[color_ref];
        }else{
            color = CL_Colorf(color_str);
        }
	
	CL_Font_System thefont(GET_MAIN_GC(), fontname, atoi(size_str.c_str()));
	
	font = thefont;
    }
    
    Font thefont;
    thefont.m_font = font;
    thefont.m_color = color;
    thefont.m_shadow_offset = shadow_offset;
    thefont.m_shadow_color = m_theme.m_colors[Theme::COLOR_SHADOW];
    m_pInstance->m_font_map[name] = thefont;;
    
    return thefont;
}

std::string  GraphicsManager::GetFontName ( Overlay overlay, const std::string& type )
{
    std::map<Overlay,std::map<std::string,std::string> >::iterator mapIt = m_pInstance->m_overlay_font_map.find( overlay  );
    CL_ResourceManager & resources  = IApplication::GetInstance()->GetResources();
    std::string fontname;

    if (mapIt != m_pInstance->m_overlay_font_map.end())
    {
        std::map<std::string,std::string>::iterator foundIt = mapIt->second.find(type);
        if (foundIt != mapIt->second.end())
        {
            fontname = foundIt->second;
        }
        else
        {
            // This overlay has an entry, but not this font yet
            fontname =  CL_String_load( std::string("Overlays/" + NameOfOverlay(overlay)
                                                    + "/fonts/" + type ), resources );
            mapIt->second[type] = fontname;
        }
    }
    else
    {
        // Overlay doesnt have an entry yet
        fontname = CL_String_load( std::string("Overlays/" + NameOfOverlay(overlay)
                                               + "/fonts/" + type ), resources );

        m_pInstance->m_overlay_font_map[overlay][type] = fontname;
    }


    return fontname;
}

CL_Colorf GraphicsManager::GetColor ( Overlay overlay, const std::string& name )
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_String colorStr = CL_String_load( std::string("Overlays/" + NameOfOverlay(overlay) + "/colors/" + name),resources);
    
    return CL_Colorf(colorStr);
}


std::string  GraphicsManager::GetFontName( DisplayFont font )
{
    return NameOfDisplayFont(font);

}


StoneRing::Font GraphicsManager::GetFont( Overlay overlay, const std::string& type )
{
    return GetFont( GetFontName ( overlay, type ) );
}


CL_Pointf GraphicsManager::GetPoint ( Overlay overlay, const std::string& name )
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_String pointname = CL_String("Overlays/" + NameOfOverlay(overlay) + "/points/" + name);
    
    CL_Resource resource = resources.get_resource(pointname);
    
    if(resource.get_type() != "point")
        throw CL_Exception("Point resource element was not 'point' type");
    
    
    float x = atof(resource.get_element().get_attribute("x").c_str());
    float y = atof(resource.get_element().get_attribute("y").c_str());
    
    return CL_Pointf(x,y);    
}

CL_Image GraphicsManager::GetImage ( GraphicsManager::Overlay overlay, const std::string& i_name )
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_String name = CL_String("Overlays/" + NameOfOverlay(overlay) + '/' + i_name);
    
    CL_Image image(GET_MAIN_GC(),name,&resources);

    return image;
}


CL_Rectf GraphicsManager::GetRect ( Overlay overlay, const std::string& name )
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_String pointname = CL_String("Overlays/" + NameOfOverlay(overlay) + "/rects/" + name);
    
    CL_Resource resource = resources.get_resource(pointname);
    
    if(resource.get_type() != "rect")
        throw CL_Exception("Rect resource element was not 'rect' type");
    
    
    float top = atof(resource.get_element().get_attribute("top").c_str());
    float left = atof(resource.get_element().get_attribute("left").c_str());
    float right = atof(resource.get_element().get_attribute("right").c_str());
    float bottom = atof(resource.get_element().get_attribute("bottom").c_str());    
    return CL_Rectf(left,top,right,bottom);
}

CL_Gradient GraphicsManager::GetGradient ( Overlay overlay, const std::string& name )
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_String gradientname = CL_String("Overlays/" + NameOfOverlay(overlay) + "/gradients/" + name);
    
    CL_Resource resource = resources.get_resource(gradientname);
    
    if(resource.get_type() != "gradient")
        throw CL_Exception("Gradient resource element was not 'gradient' type");
    
    CL_Colorf top_left(resource.get_element().get_attribute("top_left"));
    CL_Colorf top_right(resource.get_element().get_attribute("top_right"));
    CL_Colorf bottom_left(resource.get_element().get_attribute("bottom_left"));
    CL_Colorf bottom_right(resource.get_element().get_attribute("bottom_right"));

    return CL_Gradient(top_left,top_right,bottom_left,bottom_right);
}

CL_Sprite GraphicsManager::GetSprite ( GraphicsManager::Overlay overlay, const std::string& name )
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_Sprite sprite = CL_Sprite(GET_MAIN_GC(),"Overlays/"+NameOfOverlay(overlay)+"/sprites/"+name,&resources);


    sprite.set_alignment(origin_center);
    return sprite;
}

CL_Sprite GraphicsManager::GetSpriteWithImage ( const CL_Image image )
{
    // TODO: Is there a better way to do this? Such as loading the spriate frame directly from the resource manager?
    CL_Texture texture(GET_MAIN_GC(), image.get_width(), image.get_height(), cl_rgba8);
    CL_FrameBuffer framebuffer(GET_MAIN_GC());  
    framebuffer.attach_color_buffer(0, texture);
    GET_MAIN_GC().set_frame_buffer(framebuffer);
    image.draw(GET_MAIN_GC(),0,0);
    GET_MAIN_GC().reset_frame_buffer();
    
    // Okay. Now the text should have the image on it..
    CL_SpriteDescription desc;
    desc.add_frame(texture);
    
    CL_Sprite sprite(GET_MAIN_GC(),desc);
    sprite.set_alignment(origin_center);
    
    return sprite;
}

CL_Colorf GraphicsManager::LoadColor ( CL_ResourceManager& resources, const std::string& path )
{
    CL_Resource resource = resources.get_resource(path);
    CL_String color_str= CL_String_load(path,resources);
    return CL_Colorf(color_str);
}


GraphicsManager::Theme GraphicsManager::LoadTheme ( const std::string& name )
{
    Theme theme;
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    
    CL_String theme_path = CL_String("Themes/" + name + "/");    
    CL_String gradientname = CL_String(theme_path + "MenuGradient");
    
    CL_Resource resource = resources.get_resource(gradientname);
    
    if(resource.get_type() != "gradient")
        throw CL_Exception("Gradient resource element was not 'gradient' type");
    
    CL_Colorf top_left(resource.get_element().get_attribute("top_left"));
    CL_Colorf top_right(resource.get_element().get_attribute("top_right"));
    CL_Colorf bottom_left(resource.get_element().get_attribute("bottom_left"));
    CL_Colorf bottom_right(resource.get_element().get_attribute("bottom_right"));
    
    theme.m_menu_gradient = CL_Gradient(top_left,top_right,bottom_left,bottom_right);
    
    
    theme.m_colors[Theme::COLOR_MAIN] = LoadColor(resources, theme_path + "color_main");
    theme.m_colors[Theme::COLOR_HP] = LoadColor(resources, theme_path + "color_hp");
    theme.m_colors[Theme::COLOR_MP] = LoadColor(resources, theme_path + "color_mp");
    theme.m_colors[Theme::COLOR_SP] = LoadColor(resources, theme_path + "color_sp");
    theme.m_colors[Theme::COLOR_ACCENT] = LoadColor(resources, theme_path + "color_accent");   
    theme.m_colors[Theme::COLOR_SHADOW] = LoadColor(resources, theme_path + "color_shadow");
    return theme;
}


void GraphicsManager::SetTheme ( const std::string& theme )
{
    if(theme !=m_pInstance->m_theme_name){
        const std::map<std::string,Theme>::const_iterator it = m_pInstance->m_theme_map.find(theme);
        if(it == m_pInstance->m_theme_map.end()){
            m_pInstance->m_theme_map[theme] = m_pInstance->LoadTheme(theme);
        }
        m_pInstance->m_theme = m_pInstance->m_theme_map[theme];
        m_pInstance->m_font_map.clear(); // Dump font cache
        m_pInstance->m_theme_name = theme;
    }
}

std::string GraphicsManager::GetThemeName()
{
    return m_pInstance->m_theme_name;
}


void GraphicsManager::GetAvailableThemes ( std::list< std::string >& o_themes )
{
    CL_ResourceManager & resources = IApplication::GetInstance()->GetResources();
    std::vector<CL_String> sections = resources.get_resource_names("Themes");
    for(std::vector<CL_String>::const_iterator it = sections.begin(); it != sections.end(); it++){
        o_themes.push_back ( *it );
    }
}



CL_Colorf GraphicsManager::HSVToRGB ( const StoneRing::GraphicsManager::HSVColor& color )
{
    CL_Colorf rgb_color;
    if ( color.s == 0.0f )                       //HSV from 0 to 1
    {
        rgb_color.set_red(color.v);
        rgb_color.set_green(color.v);
        rgb_color.set_blue(color.v);
    }
    else
    {
        float var_h = color.h * 6.0f;
        if ( var_h == 6.0f ) 
            var_h = 0.0f;      //H must be < 1
        float var_i = int( var_h );  
        float var_1 = color.h * ( 1.0f - color.s );
        float var_2 = color.h * ( 1.0f - color.s * ( var_h - var_i ) );
        float var_3 = color.v * ( 1.0f - color.s * ( 1.0 - ( var_h - var_i ) ) );

        float var_r, var_g, var_b;
        if      ( var_i == 0 ) { var_r = color.v     ; var_g = var_3 ; var_b = var_1; }
        else if ( var_i == 1 ) { var_r = var_2 ; var_g = color.v     ; var_b = var_1; }
        else if ( var_i == 2 ) { var_r = var_1 ; var_g = color.v     ; var_b = var_3; }
        else if ( var_i == 3 ) { var_r = var_1 ; var_g = var_2 ; var_b = color.v;     }
        else if ( var_i == 4 ) { var_r = var_3 ; var_g = var_1 ; var_b = color.v;     }
        else                   { var_r = color.v     ; var_g = var_1 ; var_b = var_2; }

         rgb_color.set_red(var_r);
         rgb_color.set_green(var_g);
         rgb_color.set_blue(var_b);
    }

    return rgb_color;
}

GraphicsManager::HSVColor GraphicsManager::RGBToHSV ( const CL_Colorf& color)
{
    HSVColor hsv_color;
    float var_Min = min( color.get_red(), min( color.get_green(), color.get_blue() ) );    //Min. value of RGB
    float var_Max = max( color.get_red(), max( color.get_green(), color.get_blue() ) );   //Max. value of RGB
    float del_Max = var_Max - var_Min;             //Delta RGB value

    hsv_color.v = var_Max;

    if ( del_Max == 0.0 )                     //This is a gray, no chroma...
    {
        hsv_color.h = 0.0;                                //HSV results from 0 to 1
        hsv_color.s = 0.0;
    }
    else                                    //Chromatic data...
    {
        hsv_color.s = del_Max / var_Max;

        float del_R = ( ( ( var_Max - color.get_red() ) / 6.0f ) + ( del_Max / 2.0f ) ) / del_Max;
        float del_G = ( ( ( var_Max - color.get_green() ) / 6.0f ) + ( del_Max / 2.0f ) ) / del_Max;
        float del_B = ( ( ( var_Max - color.get_blue() ) / 6.0f ) + ( del_Max / 2.0f ) ) / del_Max;

        if      ( color.get_red() == var_Max ) hsv_color.h = del_B - del_G;
        else if ( color.get_green() == var_Max ) hsv_color.h = ( 1.0 / 3.0 ) + del_R - del_B;
        else if ( color.get_blue() == var_Max ) hsv_color.h = ( 2.0 / 3.0 ) + del_G - del_R;

        if ( hsv_color.h < 0.0f ) hsv_color.h += 1.0f;
        if ( hsv_color.h > 1.0f ) hsv_color.h -= 1.0f;
    }
    return hsv_color;
}

float GraphicsManager::RotateHue ( const float hue, const float angle_rads  )
{
    float radians = hue * M_PI * 2.0;
    return radians + angle_rads;
}

CL_Colorf GraphicsManager::GetAnalog ( const CL_Colorf& color, bool left )
{
    float degs = left?30.0:-30.0;
    return GetComplement(color,degs);
}

CL_Colorf GraphicsManager::GetOppositeColor ( const CL_Colorf& color )
{
    return GetComplement(color,180.0f);
}

CL_Colorf GraphicsManager::GetSplit ( const CL_Colorf& color, bool left )
{
    return GetComplement(color,left?150.0f:-150.0f);
}

CL_Colorf GraphicsManager::GetTriadic ( const CL_Colorf& color, bool left )
{
    return GetComplement(color,left?120.0f:-120.f);
}


CL_Colorf GraphicsManager::GetComplement ( const CL_Colorf& color, float degs )
{
    const float deg_per_rad = 57.29577951f;
    const float radians = degs * deg_per_rad;
    HSVColor hsv = RGBToHSV(color);
    hsv.v = RotateHue(hsv.v,radians);
    return HSVToRGB(hsv);
}

void GraphicsManager::GetComplementaryColors ( const CL_Colorf& color, CL_Colorf& one, CL_Colorf& two )
{
    // For now, use triadic
    one = GetTriadic(color,true);
    two = GetTriadic(color,false);
}





GraphicsManager::GraphicsManager()
{
}

GraphicsManager::~GraphicsManager()
{


}





