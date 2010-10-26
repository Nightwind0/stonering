#include "IApplication.h"
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <string>
#include <map>
#include "GraphicsManager.h"


using namespace StoneRing;


GraphicsManager * GraphicsManager::m_pInstance;

GraphicsManager * GraphicsManager::GetInstance()
{
    if (m_pInstance == NULL)
        m_pInstance = new GraphicsManager();

    return m_pInstance;
}

CL_Sprite GraphicsManager::CreateSprite ( const std::string & name )
{
    CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();

    CL_Sprite  sprite(GET_MAIN_GC(),"Sprites/" +  name, &resources);
    
    CL_Sprite clone(GET_MAIN_GC());
    clone.clone(sprite); 
    
    sprite.set_alignment(origin_center);

    return clone;
}

std::string GraphicsManager::LookUpMapWithSprite (CL_Sprite surface)
{
    for ( std::map<std::string,CL_Sprite>::iterator i = m_tile_map.begin();
            i != m_tile_map.end();
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

CL_Sprite GraphicsManager::GetTileMap ( const std::string & name )
{
    CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();

    CL_Sprite surface;


    if (m_tile_map.find( name ) == m_tile_map.end())
    {
#ifndef NDEBUG
        std::cout << "TileMap now loading: " << name << std::endl;
#endif
        surface = CL_Sprite(GET_MAIN_GC(),"Tilemaps/" + name, &resources);

        m_tile_map[ name ] = surface;
        return surface;
    }

    surface = m_tile_map[name];

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
    std::map<Overlay,CL_Image>::iterator foundIt = m_overlay_map.find(overlay);

    if (foundIt != m_overlay_map.end())
    {
        return foundIt->second;
    }
    else
    {
        CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();

        CL_Image surface( GET_MAIN_GC(),std::string("Overlays/") + NameOfOverlay(overlay) + "/overlay", &resources );

        m_overlay_map [ overlay ] = surface;

        return surface;
    }
}


CL_Image GraphicsManager::GetIcon(const std::string& icon)
{
    std::map<std::string,CL_Image>::iterator foundIt = m_icon_map.find(icon);

    if (foundIt != m_icon_map.end())
    {
        return foundIt->second;
    }
    else
    {
        CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();

        CL_Image surface( GET_MAIN_GC(), std::string("Icons/") + icon, &resources );

        m_icon_map [ icon ] = surface;

        return surface;
    }
}

CL_Font  GraphicsManager::GetFont(const std::string &name)
{
    std::map<std::string,CL_Font>::iterator foundIt = m_font_map.find( name );

    if (foundIt != m_font_map.end())
    {
        return foundIt->second;
    }
    else
    {
        CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();

        CL_Font_Sprite font( GET_MAIN_GC(), "Fonts/" + name, &resources );

        m_font_map [ name ] = font;

        return font;
    }
}

CL_Font  GraphicsManager::GetFont( Overlay overlay, const std::string& type )
{
    std::map<Overlay,std::map<std::string,std::string> >::iterator mapIt = m_overlay_font_map.find( overlay  );
    CL_ResourceManager & resources  = IApplication::GetInstance()->GetResources();
    std::string fontname;

    if (mapIt != m_overlay_font_map.end())
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

        m_overlay_font_map[overlay][type] = fontname;
    }


    return GetFont(fontname);
}

CL_Font  GraphicsManager::GetDisplayFont( DisplayFont font )
{
    std::map<DisplayFont,CL_Font>::iterator mapIt = m_display_font_map.find(font);

    if (mapIt == m_display_font_map.end())
    {
        CL_ResourceManager & resources  = IApplication::GetInstance()->GetResources();
        CL_Resource resource = resources.get_resource("Fonts/" + NameOfDisplayFont(font));

        CL_String filename = resource.get_element().get_attribute("file");
        CL_String fontname = resource.get_element().get_attribute("font_name");
        CL_String size_str = resource.get_element().get_attribute("size");
        CL_String color =    resource.get_element().get_attribute("color");

        m_display_font_colors[font] = CL_Colorf(color);
        int size = atoi(size_str.c_str());
        /*
         	CL_Font_Freetype::CL_Font_Freetype(
        	CL_GraphicContext & gc,
        	const CL_StringRef & typeface_name,
        	int height,
        	const CL_VirtualDirectory & directory);

        */
        CL_IODevice file = resources.get_directory(resource).open_file_read(filename);
        CL_Font_Freetype thefont(GET_MAIN_GC(),
                                 filename,
                                 size,
                                 file);

        if(thefont.is_null()) throw CL_Exception("Bad display font");


        m_display_font_map[font] = thefont;
        return thefont;
    }
    else
    {
        return mapIt->second;
    }

}

CL_Colorf GraphicsManager::GetFontColor ( DisplayFont font )
{
    return m_display_font_colors[font];
    //return CL_Colorf::teal;
}


GraphicsManager::GraphicsManager()
{
}

GraphicsManager::~GraphicsManager()
{


}





