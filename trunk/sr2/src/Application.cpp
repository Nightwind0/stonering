#include <ClanLib/display.h>
#include <ClanLib/core.h>
#include <ClanLib/gl.h>

#include "Application.h"


using StoneRing::Application;

Application::Application()
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
		CL_DisplayWindow window ("Stone Ring", WINDOW_WIDTH, WINDOW_HEIGHT);
		
		
		teardownClanLib();
	}
	catch(...)
	{
	}
	
}

Application app;
