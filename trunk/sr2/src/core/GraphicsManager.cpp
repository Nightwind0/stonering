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

CL_Sprite * GraphicsManager::CreateSprite ( const std::string & name )
{
    CL_ResourceManager *pResources  = IApplication::GetInstance()->GetResources();

    CL_Sprite * pSprite = new CL_Sprite("Sprites/" +  name, pResources);

    return pSprite;
}

std::string GraphicsManager::LookUpMapWithSurface (CL_Surface * surface)
{
    for( std::map<std::string,CL_Surface*>::iterator i = m_tile_map.begin();
         i != m_tile_map.end();
         i++)
    {
        if ( i->second == surface)
        {
            return i->first;
        }
    }

    throw CL_Error ( "BAD! TILEMAP NOT FOUND IN lookupMapWithSurface" );

    return "die";

}

CL_Surface * GraphicsManager::GetTileMap ( const std::string & name )
{
    CL_ResourceManager *pResources  = IApplication::GetInstance()->GetResources();
    CL_Surface *pSurface;


    if(m_tile_map.find( name ) == m_tile_map.end())
    {
#ifndef NDEBUG
        std::cout << "TileMap now loading: " << name << std::endl;
#endif
        pSurface = new CL_Surface("Tilemaps/" + name, pResources);

        m_tile_map[ name ] = pSurface;
        return pSurface;
    }

    pSurface = m_tile_map[name];

    return pSurface;
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
        cl_assert(0);
    }

    cl_assert(0);
    return "";
}

CL_Sprite * GraphicsManager::CreateMonsterSprite (const std::string &monster, const std::string &sprite)
{
    CL_ResourceManager *pResources = IApplication::GetInstance()->GetResources();
    CL_Sprite * pSprite = new CL_Sprite("Sprites/Monsters/"+monster+'/'+sprite,pResources);

    return pSprite;
}

CL_Sprite * GraphicsManager::CreateCharacterSprite ( const std::string &player, const std::string &sprite)
{
    CL_ResourceManager *pResources = IApplication::GetInstance()->GetResources();
    CL_Sprite * pSprite = new CL_Sprite("Sprites/BattleSprites/"+player+'/'+sprite,pResources);

    return pSprite;
}


CL_Surface * GraphicsManager::GetBackdrop(const std::string &name)
{
    CL_ResourceManager *pResources = IApplication::GetInstance()->GetResources();
    CL_Surface *pSurface = new CL_Surface("Backdrops/" + name, pResources);
    assert(pSurface);

    return pSurface;
}

CL_Surface * GraphicsManager::GetOverlay(GraphicsManager::Overlay overlay)
{
    std::map<Overlay,CL_Surface*>::iterator foundIt = m_overlay_map.find(overlay);

    if(foundIt != m_overlay_map.end())
    {
        return foundIt->second;
    }
    else
    {
        CL_ResourceManager *pResources  = IApplication::GetInstance()->GetResources();
        CL_Surface * pSurface = NULL;
        pSurface  = new CL_Surface( std::string("Overlays/") + NameOfOverlay(overlay) + "/overlay", pResources );

        m_overlay_map [ overlay ] = pSurface;

        return pSurface;
    }
}


CL_Surface * GraphicsManager::GetIcon(const std::string& icon)
{
    std::map<std::string,CL_Surface*>::iterator foundIt = m_icon_map.find(icon);

    if(foundIt != m_icon_map.end())
    {
        return foundIt->second;
    }
    else
    {
        CL_ResourceManager *pResources  = IApplication::GetInstance()->GetResources();
        CL_Surface * pSurface = NULL;
        pSurface  = new CL_Surface( std::string("Icons/") + icon, pResources );

        m_icon_map [ icon ] = pSurface;

        return pSurface;
    }
}

CL_Font * GraphicsManager::GetFont(const std::string &name)
{
    std::map<std::string,CL_Font*>::iterator foundIt = m_font_map.find( name );

    if(foundIt != m_font_map.end())
    {
        return foundIt->second;
    }
    else
    {
        CL_ResourceManager *pResources  = IApplication::GetInstance()->GetResources();
        CL_Font * pFont = NULL;
        pFont  = new CL_Font( std::string("Fonts/") + name, pResources );

        m_font_map [ name ] = pFont;

        return pFont;
    }
}

CL_Font * GraphicsManager::GetFont( Overlay overlay, const std::string& type )
{
    std::map<Overlay,std::map<std::string,std::string> >::iterator mapIt = m_overlay_font_map.find( overlay  );
    CL_ResourceManager *pResources  = IApplication::GetInstance()->GetResources();
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
            fontname =  CL_String::load( std::string("Overlays/" + NameOfOverlay(overlay)
                + "/fonts/" + type ), pResources );
            mapIt->second[type] = fontname;
        }
    }
    else
    {
        // Overlay doesnt have an entry yet
        fontname = CL_String::load( std::string("Overlays/" + NameOfOverlay(overlay)
                + "/fonts/" + type ), pResources );

        m_overlay_font_map[overlay][type] = fontname;
    }


    return GetFont(fontname);
}


GraphicsManager::GraphicsManager()
{
}

GraphicsManager::~GraphicsManager()
{
    for( std::map<std::string, CL_Surface*>::iterator i = m_tile_map.begin();
         i != m_tile_map.end();
         i++)
    {
        delete i->second;
    }


}





