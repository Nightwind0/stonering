#include <ClanLib/display.h>
#include <ClanLib/core.h>
#include <ClanLib/gl.h>
#include <iostream>

#include "Application.h"
#include "Level.h"


using StoneRing::Application;
using namespace StoneRing;


Application sr_app;

Application * Application::getApplication()
{
  return &sr_app;
}

CL_ResourceManager * Application::getResources()
{
  return mpResources;
}

Application::Application():mCurX(0),mCurY(0),mbDone(false)
{
}

Application::~Application()
{
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


void Application::onSignalKeyDown(const CL_InputEvent &key)
{

	switch(key.id)
	{
	case CL_KEY_ESCAPE:
		mbDone = true;
		break;
	case CL_KEY_DOWN:
		mCurY+=3;
		break;
	case CL_KEY_UP:
		mCurY-=3;
		break;
	case CL_KEY_LEFT:
		mCurX-=3;
		break;
	case CL_KEY_RIGHT:
		mCurX+=3;
		break;
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
		std::cout << sizeof(Tile) << std::endl;
		std::cout << sizeof(Tilemap) << std::endl;
		setupClanLib();
		
		mpResources = new CL_ResourceManager ( "Media/resources.xml" );

		std::string name = CL_String::load("Configuration/name", mpResources);
		std::string startinglevel = CL_String::load("Game/StartLevel",mpResources);
		mpWindow  = new CL_DisplayWindow(name, WINDOW_WIDTH, WINDOW_HEIGHT);
		
		CL_Display::clear();
		
		showRechargeableOnionSplash();
		showIntro();

		mpLevel = new Level(startinglevel, mpResources);

		CL_System::sleep( 50 );

		CL_Slot slot_quit = mpWindow->sig_window_close().connect(this, &Application::onSignalQuit);

		CL_Slot slot_key_down = CL_Keyboard::sig_key_down().connect(this, &Application::onSignalKeyDown);
		

		while(!mbDone)
		{
			
		    CL_Display::clear();

		    CL_Rect dst(mCurX, mCurY, mCurX + mpLevel->getWidth() * 32, mCurY + mpLevel->getHeight()*32);
		    CL_Rect src(0,0,mpLevel->getWidth() * 32, mpLevel->getHeight() * 32);

		    mpWindow->get_gc()->push_cliprect( dst);

		    mpLevel->draw(src,dst, mpWindow->get_gc(), false);
		    //  CL_System::sleep( 300 );
		    mpLevel->drawFloaters(src,dst, mpWindow->get_gc());
		    mpWindow->get_gc()->draw_rect( CL_Rect(96,96,128,128), CL_Color::aqua ) ;

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


