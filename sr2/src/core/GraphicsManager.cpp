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
        
        pSurface = new CL_Surface("Tilemaps/" + name, pResources);
#ifndef NDEBUG
        std::cout << std::dec;
        std::cout << "Surface Depth: " << pSurface->get_pixeldata().get_format().get_depth() << std::endl;
        std::cout << "Red: " << std::hex << pSurface->get_pixeldata().get_format().get_red_mask() << std::endl;
        std::cout << "Green: " << std::hex << pSurface->get_pixeldata().get_format().get_green_mask() << std::endl;
        std::cout << "Blue: " << std::hex << pSurface->get_pixeldata().get_format().get_blue_mask() << std::endl;
        std::cout << "Alpha: " << std::hex << pSurface->get_pixeldata().get_format().get_alpha_mask() << std::endl;
        std::cout << std::dec;
#endif

        mTileMap[ name ] = pSurface;

        return pSurface;
    }
    
    pSurface = mTileMap[name];

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
    


