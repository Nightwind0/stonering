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

Application::Application():mCurX(0),mCurY(0)
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

		std::string name = CL_String::load("Configuration/name", mpResources);
		std::string startinglevel = CL_String::load("Game/StartLevel",mpResources);
		mpWindow  = new CL_DisplayWindow(name, WINDOW_WIDTH, WINDOW_HEIGHT);
		
		CL_Display::clear();
		
		showRechargeableOnionSplash();
		showIntro();

		mpLevel = new Level(startinglevel, mpResources);

		CL_System::sleep( 200 );

		while(!CL_Keyboard::get_keycode(CL_KEY_ESCAPE) && !CL_Keyboard::get_keycode(CL_KEY_ENTER) && !CL_Keyboard::get_keycode( CL_KEY_SPACE))
		  {
		    
		    CL_Display::clear();
		    mpLevel->draw(0,0, mpWindow->get_gc());


		    
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


