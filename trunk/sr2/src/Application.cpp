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


void Application::playAnimation(const std::string &animation)const
{
#ifndef NDEBUG
    std::cout << "Playing animation " << animation << std::endl;
#endif
}

void Application::playSound(const std::string &sound)const
{
#ifndef NDEBUG
    std::cout << "Playing sound " << sound << std::endl;
#endif
}

void Application::loadLevel(const std::string &level, uint startX, uint startY)
{
#ifndef NDEBUG
    std::cout << "Load level " << level << std::endl;
#endif
}

void Application::startBattle(const std::string &monster, uint count, bool isBoss)
{
#ifndef NDEBUG
    std::cout << "Start battle " << monster << std::endl;
#endif
}

void Application::say(const std::string &speaker, const std::string &text)
{
#ifndef NDEBUG
    std::cout << "Say: " << speaker << ":" << text << std::endl;
#endif
}

void Application::pause(uint time)
{
}

void Application::invokeShop(const std::string &shoptype)
{
}




IApplication * IApplication::getInstance()
{
    return &sr_app;
}


IParty * Application::getParty() const
{
   
    return mpParty;
}


LevelFactory *Application::getLevelFactory() const
{
    return mpLevelFactory;
}


CL_ResourceManager * Application::getResources() const
{
    return mpResources;
}


bool Application::canMove(const CL_Rect &currently, const CL_Rect &destination, bool noHot, bool isPlayer)
{
    return mpLevel->canMove(currently,destination,noHot,isPlayer);
}

Application::Application():mpParty(0),mpLevelFactory(0),mCurX(0),mCurY(0),mLevelX(0),mLevelY(0),mbDone(false),mSpeed(1)
{
    mpParty = new Party();

    // We want the generic level factory.
    mpLevelFactory = new LevelFactory();
}

Application::~Application()
{
}


CL_Rect Application::getLevelRect() const
{

    return CL_Rect(mLevelX,mLevelY, mLevelX + getScreenWidth(), mLevelY + getScreenHeight());

}

CL_Rect Application::getDisplayRect() const
{
    return CL_Rect(0,0,getScreenWidth(), getScreenHeight());

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



void Application::recalculatePlayerPosition(eDir dir)
{
    int X = mpParty->getLevelX();
    int Y = mpParty->getLevelY();

    if(dir == EAST)
    {
	if( X - mLevelX > (WINDOW_WIDTH / 2))
	{
	    // Try to scroll right, otherwise, move guy east
	    
	    if(mLevelX + 1 + WINDOW_WIDTH < mpLevel->getWidth() * 32)
	    {
		mLevelX++;
	    }
	    else
	    {
		mCurX = X - mLevelX;
	    }
	}
	else
	{
	    mCurX = X - mLevelX;
	}
    }
    else if(dir == WEST)
    {
	if(X - mLevelX <= (WINDOW_WIDTH / 2))
	{
	    if(mLevelX -1 >0)
	    {
		mLevelX--;
	    }
	    else
	    {
		mCurX = X - mLevelX;
	    }
	}
	else
	{
	    mCurX = X - mLevelX;
	}
	
    }
    else if (dir == SOUTH)
    {
	if(Y - mLevelY > (WINDOW_HEIGHT /2))
	{
	    
	    if(mLevelY + 1 + WINDOW_HEIGHT < mpLevel->getHeight() * 32)
	    {
		mLevelY++;
	    }
	    else
	    {
		mCurY = Y - mLevelY;
	    }
	}
	else
	{
	    mCurY = Y - mLevelY;
	}
    }   
    else if (dir == NORTH)
    {
	if(Y - mLevelY <= (WINDOW_HEIGHT / 2))
	{
	    if(mLevelY -1 >0)
	    {
		mLevelY--;
	    }
	    else
	    {
		mCurY = Y - mLevelY;
	    }
	}
	else
	{
	    mCurY = Y - mLevelY;
	}
    }
    

}
bool Application::move(eDir dir, int times)
{
    for(int i=0;i<times;i++)
    {
    
	int nX = mpParty->getLevelX();
	int nY = mpParty->getLevelY();

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
	
	if(canMove ( CL_Rect(mpParty->getLevelX(),mpParty->getLevelY(),
			     mpParty->getLevelX()+64,mpParty->getLevelY()+64),
		     CL_Rect(nX,nY,nX+64,nY+64),false, true))
	{
	    mpParty->setLevelX( nX );
	    mpParty->setLevelY( nY );

	    recalculatePlayerPosition(dir);

	    mPlayerDir = dir;

	    mpLevel->step(CL_Rect(nX,nY,nX+64,nY+64));

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

    case CL_KEY_D:
	std::cout << "AT " << '(' << mCurX / 32 << ',' << mCurY / 32 << ')' << std::endl;
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
			



	    CL_Rect dst(0,0, min(WINDOW_WIDTH, (const int)mpLevel->getWidth()*32),min(WINDOW_HEIGHT, (const int)mpLevel->getHeight() * 32));




	
	    CL_Rect src = getLevelRect();



//	    CL_Rect src = dst;
	    mpWindow->get_gc()->push_cliprect( dst);

	    mpLevel->draw(src,dst, mpWindow->get_gc(), false,false,false);

	    mpWindow->get_gc()->draw_rect( CL_Rect(mCurX,mCurY,mCurX+64,mCurY+64), CL_Color::aqua ) ;
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


