#include <ClanLib/display.h>
#include <ClanLib/core.h>
#include <ClanLib/gl.h>
#include <iostream>

#include "Application.h"
#include "Level.h"
#include "Party.h"


using StoneRing::Application;
using namespace StoneRing;

#ifndef _MSC_VER

using std::min;
using std::max;

#endif

const int WINDOW_HEIGHT = 600 ;
const int WINDOW_WIDTH = 800 ;




Application sr_app;


int Application::getScreenWidth()const
{
    return WINDOW_WIDTH;
}

int Application::getScreenHeight()const
{
    return WINDOW_HEIGHT;
}


IApplication * IApplication::getInstance()
{
    return &sr_app;
}


IParty * Application::getParty() const
{
   
    return mpParty;
}


CL_ResourceManager * Application::getResources() const
{
    return mpResources;
}


bool Application::canMove(const CL_Rect &currently, const CL_Rect &destination, bool noHot)
{
    return mpLevel->canMove(currently,destination,noHot);
}

Application::Application():mpParty(0),mCurX(0),mCurY(0),mbDone(false),mSpeed(1)
{
    mpParty = new Party();
}

Application::~Application()
{
}


CL_Rect Application::getLevelRect() const
{
    // TODO: Make this real
    return CL_Rect(0,0,mpLevel->getWidth() * 32, mpLevel->getHeight() * 32);

}

CL_Rect Application::getDisplayRect() const
{
    return CL_Rect(40,40,mpLevel->getWidth() * 32, mpLevel->getHeight() * 32);

}

void Application::setupClanLib()
{
    CL_SetupCore::init();
    CL_SetupDisplay::init();
    CL_SetupGL::init();
	
}

void Application::teardownClanLib()
{
    CL_SetupGL::deinit();
    CL_SetupDisplay::deinit();
    CL_SetupCore::deinit();
}


bool Application::move(eDir dir, int times)
{
    for(int i=0;i<times;i++)
    {
    
	int nX = mCurX;
	int nY = mCurY;

	switch(dir)
	{
	case NORTH:
	    nY--;
	    break;
	case SOUTH:
	    nY++;
	    break;
	case EAST:
	    nX++;
	    break;
	case WEST:
	    nX--;
	    break;
	}

	if(canMove ( CL_Rect(mCurX,mCurY,mCurX+32,mCurY+32), CL_Rect(nX,nY,nX+32,nY+32), true))
	{
	    mCurX = nX;
	    mCurY = nY;
	}
	else return false;
	
    }

	return true;


}

void Application::onSignalKeyDown(const CL_InputEvent &key)
{

    CL_DomDocument document;

	
    int nX =mCurX;
    int nY =mCurY;
    
    switch(key.id)
    {
    case CL_KEY_ESCAPE:
	mbDone = true;
	break;
    case CL_KEY_DOWN:
	move(SOUTH,mSpeed);
	break;
    case CL_KEY_UP:
	move(NORTH,mSpeed);
	break;
    case CL_KEY_LEFT:
	move(WEST,mSpeed);
	break;
    case CL_KEY_RIGHT:
	move(EAST,mSpeed);
	break;
    case CL_KEY_S:
	mSpeed--;
	break;
    case CL_KEY_F:
	mSpeed++;
	break;

    case CL_KEY_T:

	document.append_child ( mpLevel->createDomElement(document) );

	delete mpLevel;

	mpLevel = new Level();
	mpLevel->load (document );


	break;


    case CL_KEY_W:

	document.append_child ( mpLevel->createDomElement(document) );

	document.save( new CL_OutputSource_File( "foo.xml"),true,false);

	break;

    case CL_KEY_D:
	std::cout << "AT " << '(' << mCurX / 32 << ',' << mCurY / 32 << ')' << std::endl;
    default:
	break;
    }
	
    

}


void Application::onSignalQuit()
{
	
}




int Application::main(int argc, char ** argv)
{
  
#ifdef _DEBUG

    CL_ConsoleWindow console("Stone Ring Debug",80,1000);
    console.redirect_stdio();
#endif
	
    try
    {

	setupClanLib();
		
	mpResources = new CL_ResourceManager ( "Media/resources.xml" );

#ifdef NDEBUG
	std::string name = CL_String::load("Configuration/name", mpResources);
#else
	std::string name = CL_String::load("Configuration/name", mpResources) + " (DEBUG)";
#endif
	std::string startinglevel = CL_String::load("Game/StartLevel",mpResources);
	mpWindow  = new CL_DisplayWindow(name, WINDOW_WIDTH, WINDOW_HEIGHT);
		
	CL_Display::clear();
		
	showRechargeableOnionSplash();
	showIntro();

	mpLevel = new Level(startinglevel, mpResources);

	CL_System::sleep( 50 );

	CL_Slot slot_quit = mpWindow->sig_window_close().connect(this, &Application::onSignalQuit);

	CL_Slot slot_key_down = CL_Keyboard::sig_key_down().connect(this, &Application::onSignalKeyDown);

	CL_Display::clear();		

	while(!mbDone)
	{
			



	    CL_Rect dst(40,40, min(WINDOW_WIDTH, (const int)mpLevel->getWidth()*32),min(WINDOW_HEIGHT, (const int)mpLevel->getHeight() * 32));



	    // CL_Rect src(0,0,mpLevel->getWidth() * 32, mpLevel->getHeight() * 32);
	    CL_Rect src(mCurX,mCurY,mCurX + dst.get_width(), mCurY +dst.get_height());

//	    CL_Rect src = dst;
	    mpWindow->get_gc()->push_cliprect( dst);

	    mpLevel->draw(src,dst, mpWindow->get_gc(), false);

//	    mpWindow->get_gc()->draw_rect( CL_Rect(mCurX,mCurY,mCurX+32,mCurY+32), CL_Color::aqua ) ;
	    mpLevel->drawMappableObjects( src,dst, mpWindow->get_gc());
	    mpLevel->drawFloaters(src,dst, mpWindow->get_gc());


	    mpWindow->get_gc()->pop_cliprect();

		    
	
	    CL_Display::flip();
	    CL_System::keep_alive();
	}
		

		

		
	teardownClanLib();
    }
    catch(CL_Error error)
    {
	std::cerr << "Exception Caught!!" << std::endl;
	std::cerr << error.message.c_str() << std::endl;
    }
	

	return 0;

}

void Application::showRechargeableOnionSplash()
{
}

void Application::showIntro()
{


    CL_Surface splash("Configuration/splash", mpResources);
    CL_Surface background("Configuration/splashbg", mpResources);

    CL_GraphicContext *gc = mpWindow->get_gc();

    int displayX = (WINDOW_WIDTH - splash.get_width()) / 2;
    int displayY = (WINDOW_HEIGHT - splash.get_height()) / 2;



    while(!CL_Keyboard::get_keycode(CL_KEY_ESCAPE) && !CL_Keyboard::get_keycode(CL_KEY_ENTER) && !CL_Keyboard::get_keycode( CL_KEY_SPACE))
    {

	background.draw(0,0);
	splash.draw(displayX,displayY);

	CL_Display::flip();
	CL_System::keep_alive();
    }
	

}


