#include "IApplication.h"
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <string>
#include <map>
#include "GraphicsManager.h"


using namespace StoneRing;


const char * const gFontName[GraphicsManager::__LAST_FONT__] =
{
    "SpeakerText", // FONT_SPEAKER
    "SayText", // FONT_SAY_TEXT
    "Choice", // FONT_CHOICE
    "Option",
    "CurrentOption"
};

GraphicsManager * GraphicsManager::mInstance;

GraphicsManager * GraphicsManager::getInstance()
{
    if(mInstance == NULL)
        mInstance = new GraphicsManager();

    return mInstance;
}

CL_Sprite * GraphicsManager::createSprite ( const std::string & name )
{
    CL_ResourceManager *pResources  = IApplication::getInstance()->getResources();

    CL_Sprite * pSprite = new CL_Sprite("Sprites/" +  name, pResources);

    return pSprite;
}

std::string GraphicsManager::lookUpMapWithSurface (CL_Surface * surface)
{
    for( std::map<std::string,CL_Surface*>::iterator i = mTileMap.begin();
         i != mTileMap.end();
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

CL_Surface * GraphicsManager::getTileMap ( const std::string & name )
{
    CL_ResourceManager *pResources  = IApplication::getInstance()->getResources();
    CL_Surface *pSurface;


    if(mTileMap.find( name ) == mTileMap.end())
    {
#ifndef NDEBUG
        std::cout << "TileMap now loading: " << name << std::endl;
#endif
        pSurface = new CL_Surface("Tilemaps/" + name, pResources);

        mTileMap[ name ] = pSurface;
        return pSurface;
    }
    
    pSurface = mTileMap[name];

    return pSurface;
}

CL_Sprite * GraphicsManager::createMonsterSprite (const std::string &monster, const std::string &sprite)
{
    CL_ResourceManager *pResources = IApplication::getInstance()->getResources();
    CL_Sprite * pSprite = new CL_Sprite("Sprites/Monsters/"+monster+'/'+sprite,pResources);

    return pSprite;
}

CL_Surface * GraphicsManager::getBackdrop(const std::string &name)
{
    CL_ResourceManager *pResources = IApplication::getInstance()->getResources();
    CL_Surface *pSurface = new CL_Surface("Backdrops/" + name, pResources);
    assert(pSurface);

    return pSurface;
}

CL_Font * GraphicsManager::getFont(StoneRing::GraphicsManager::eFont font)
{
    std::map<eFont,CL_Font*>::iterator foundIt = mFontMap.find( font );

    if(foundIt != mFontMap.end())
    {
        return foundIt->second;
    }
    else
    {
        CL_ResourceManager *pResources  = IApplication::getInstance()->getResources();
        CL_Font * pFont = NULL;
        pFont  = new CL_Font( std::string("Fonts/") + gFontName [ font ], pResources );

        mFontMap [ font ] = pFont;

        return pFont;
    }
}
            
            
GraphicsManager::GraphicsManager()
{
}

GraphicsManager::~GraphicsManager()
{
    for( std::map<std::string, CL_Surface*>::iterator i = mTileMap.begin();
         i != mTileMap.end();
         i++)
    {
        delete i->second;
    }


}
    




