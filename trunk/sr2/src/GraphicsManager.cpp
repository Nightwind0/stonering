#include "Application.h"
#include <ClanLib/core.h>
#include <ClanLib/display.h>
#include <string>
#include <map>
#include "GraphicsManager.h"


using namespace StoneRing;

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


	if(mTileMap.count( name ) == 0)
	{
		
		pSurface = new CL_Surface("Tilemaps/" + name, pResources);

		mTileMap[ name ] = pSurface;

		return pSurface;
	}
	
	pSurface = mTileMap[name];

	return pSurface;
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
	
