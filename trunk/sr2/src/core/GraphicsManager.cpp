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
    if(m_pInstance == NULL)
        m_pInstance = new GraphicsManager();

    return m_pInstance;
}

CL_Sprite GraphicsManager::CreateSprite ( const std::string & name )
{
    CL_ResourceManager& resources  = IApplication::GetInstance()->GetResources();

    CL_Sprite  sprite(GET_MAIN_GC(),"Sprites/" +  name, &resources);

    return sprite;
}

std::string GraphicsManager::LookUpMapWithSprite (CL_Sprite surface)
{
    for( std::map<std::string,CL_Sprite>::iterator i = m_tile_map.begin();
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


    if(m_tile_map.find( name ) == m_tile_map.end())
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
    switch(overlay)
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

CL_Sprite  GraphicsManager::CreateMonsterSprite (const std::string &monster, const std::string &sprite_id)
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_Sprite  sprite = CL_Sprite(GET_MAIN_GC(),"Sprites/Monsters/"+monster+'/'+sprite_id,&resources);

    return sprite;
}

CL_Sprite GraphicsManager::CreateCharacterSprite ( const std::string &player, const std::string &sprite_id)
{
    CL_ResourceManager& resources = IApplication::GetInstance()->GetResources();
    CL_Sprite sprite = CL_Sprite(GET_MAIN_GC(),"Sprites/BattleSprites/"+player+'/'+sprite_id,&resources);

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

    if(foundIt != m_overlay_map.end())
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

    if(foundIt != m_icon_map.end())
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

    if(foundIt != m_font_map.end())
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

    if(mapIt != m_overlay_font_map.end())
    {
        std::map<std::string,std::string>::iterator foundIt = mapIt->second.find(type);
        if(foundIt != mapIt->second.end())
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


GraphicsManager::GraphicsManager()
{
}

GraphicsManager::~GraphicsManager()
{


}





