#include <ClanLib/display.h>
#include <ClanLib/core.h>
#include <ClanLib/gl.h>
#include <iostream>
#include <sstream>
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



std::string IntToString (int i)
{
    std::ostringstream os;

    os << i;

    return os.str();
}


  
GameAction::GameAction()
{
}

GameAction::~GameAction()
{
}

SayAction::SayAction(const std::string & speaker, 
		     const std::string & text):
    mSpeaker(speaker),mText(text)
{
}

SayAction::~SayAction()
{
}

std::string SayAction::getSpeaker() const
{
    return mSpeaker;
}

std::string SayAction::getText() const
{
    return mText;
}


LoadLevelAction::LoadLevelAction(const std::string & level,
				 uint startx, uint starty):
    mLevel(level),mStartX(startx),mStartY(starty)
{
}

LoadLevelAction::~LoadLevelAction()
{
}

std::string LoadLevelAction::getLevel() const
{
    return mLevel;
}
uint LoadLevelAction::getStartX() const
{
    return mStartX;
}
uint LoadLevelAction::getStartY() const
{
    return mStartY;
}

StartBattleAction::StartBattleAction(const std::string &monster,
				     uint count, bool boss):
    mMonster(monster),mCount(count),mbBoss(boss)
{
}

StartBattleAction::~StartBattleAction()
{
}
	 
std::string StartBattleAction::getMonster() const
{
    return mMonster;
}
uint StartBattleAction::getCount() const
{
    return mCount;
}

bool StartBattleAction::isBoss() const
{
    return mbBoss;
}



PlayAnimationAction::PlayAnimationAction(const std::string &animation)
    :mAnimation(animation)
{
}

PlayAnimationAction::~PlayAnimationAction()
{
}

std::string PlayAnimationAction::getAnimation() const
{
    return mAnimation;
}


PlaySoundAction::PlaySoundAction(const std::string &sound):mSound(sound)
{
    
}

PlaySoundAction::~PlaySoundAction()
{
}

std::string PlaySoundAction::getSound() const
{
    return mSound;
}



PauseAction::PauseAction(uint time):mTime(time)
{
}

PauseAction::~PauseAction()
{
}

uint PauseAction::getTime() const
{
    return mTime;
}





InvokeShopAction::InvokeShopAction(const std::string &shop):mShopType(shop)
{
}

InvokeShopAction::~InvokeShopAction()
{
}

std::string InvokeShopAction::getShopType() const
{
    return mShopType;
}



Application sr_app;


int Application::getScreenWidth()const
{
    return WINDOW_WIDTH;
}

int Application::getScreenHeight()const
{
    return WINDOW_HEIGHT;
}


void Application::playAnimation(const std::string &animation)
{
#ifndef NDEBUG
    std::cout << "Playing animation " << animation << std::endl;
#endif

    mActionQueue.push ( new PlayAnimationAction ( animation ) );
}

void Application::playSound(const std::string &sound)
{
#ifndef NDEBUG
    std::cout << "Playing sound " << sound << std::endl;
#endif

    mActionQueue.push( new PlaySoundAction( sound ) );
}

void Application::loadLevel(const std::string &level, uint startX, uint startY)
{
#ifndef NDEBUG
    std::cout << "Load level " << level << ':' << startX << ',' << startY << std::endl;
#endif

    mActionQueue.push( new LoadLevelAction ( level, startX, startY ));
}

void Application::startBattle(const std::string &monster, uint count, bool isBoss)
{
#ifndef NDEBUG
    std::cout << "Start battle " << monster << std::endl;
#endif

    mActionQueue.push( new StartBattleAction( monster, count, isBoss ) );
}

void Application::say(const std::string &speaker, const std::string &text)
{
#ifndef NDEBUG
    std::cout << "Say: " << speaker << ":" << text << std::endl;
#endif

    meState = TALKING;

    static const CL_Rect textRect(16,388, 783, 580);
    static const CL_Rect speakerRect(15,315,783,369);

    std::string::const_iterator textIter= text.begin();

    bool done = false;

    mbPauseMovement = true;
    mpfSBBlack->set_scale(2.0,2.0);

    int totalDrawn = 0;

    while(!done)
    {
	int drawCount;
	bool doneFrame = false;
	while(!doneFrame)
	{
	    drawMap();

	    mpWindow->get_gc()->fill_rect( speakerRect, CL_Color(255,255,255,200) );
	    mpWindow->get_gc()->fill_rect( textRect, CL_Color(0,0,0,128) ) ;

	    mpSayOverlay->draw(0,300, mpWindow->get_gc());
	    
	    drawCount = mpfBWhite->draw(textRect, textIter, text.end(), mpWindow->get_gc() );
	    mpfBGray->draw(speakerRect, speaker.begin(),speaker.end(),mpWindow->get_gc() );

	    if(CL_Keyboard::get_keycode(CL_KEY_ENTER) || CL_Keyboard::get_keycode(CL_KEY_SPACE))
	    {

		if(totalDrawn + drawCount == text.size())
		{
		    done = true;
		}

		doneFrame = true;

		CL_System::sleep(333); // Part hack to prevent enter from being detected twice, partly for effect...
	    }
	    else if (CL_Keyboard::get_keycode(CL_KEY_ESCAPE))
	    {
		doneFrame = true;
		done = true;
	    }

	    if(totalDrawn + drawCount < text.size())
	    {
		mpWindow->get_gc()->fill_rect( CL_Rect(WINDOW_WIDTH - 20, WINDOW_HEIGHT - 20, WINDOW_WIDTH - 10, WINDOW_HEIGHT - 10), CL_Color::black );
	    }

	    CL_Display::flip();

	    CL_System::keep_alive();
	}

	totalDrawn += drawCount;
	textIter += drawCount;
	
    }

    mpfSBBlack->set_scale(1.0,1.0);
    mbPauseMovement = false;
    meState = MAIN;

}

void Application::pause(uint time)
{

    mActionQueue.push ( new PauseAction ( time ) );
}

void Application::invokeShop(const std::string &shoptype)
{
    mActionQueue.push ( new InvokeShopAction ( shoptype ) );
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

Application::Application():mpParty(0),mpLevelFactory(0),mCurX(0),mCurY(0),
			   mLevelX(0),mLevelY(0),mbDone(false),mSpeed(4),mbPauseMovement(false),
			   mbShowDebug(false),  mePlayerDirection(SOUTH)
    
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

    static uint startTime = CL_System::get_time();

    uint now = CL_System::get_time();

    if(now - startTime > 300) //@todo: get from resources at startup
    {
	startTime = now;
	mbStep  = mbStep? false: true;
    }


    mePlayerDirection = dir;


    int nX = mpParty->getLevelX();
    int nY = mpParty->getLevelY();
    
    int offX = mpParty->getWidth() / 2;
    int offY = mpParty->getHeight() / 2;

    CL_Rect oldLocation = CL_Rect(mpParty->getLevelX(), mpParty->getLevelY() + offY, 
				  mpParty->getLevelX() + mpParty->getWidth(),
				  mpParty->getLevelY() + offY + offY);
    
    for(int i=0;i<times;i++)
    {
    	
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
	
	if(canMove ( CL_Rect(mpParty->getLevelX(),
			     mpParty->getLevelY() + offY ,
			     mpParty->getLevelX() + offX + offX,
			     mpParty->getLevelY() + offY + offY),
		     CL_Rect(nX,nY + offY,nX + offX + offX,nY+offY + offY),false, true))
	{
	    mpParty->setLevelX( nX );
	    mpParty->setLevelY( nY );

	    recalculatePlayerPosition(dir);
	    

	    mPlayerDir = dir;

	}
	else 
	{
	    mpLevel->step(CL_Rect(mpParty->getLevelX(),
				  mpParty->getLevelY() + offY,
				  mpParty->getLevelX() + offX + offX, 
				  mpParty->getLevelY() + offY + offY), oldLocation);
	    
	    return false;
	}
	
    }

    mpLevel->step(CL_Rect(mpParty->getLevelX(),
			  mpParty->getLevelY() + offY,
			  mpParty->getLevelX() + offX + offX, 
			  mpParty->getLevelY() + offY + offY), oldLocation);

    return true;


}

void Application::onSignalKeyDown(const CL_InputEvent &key)
{


    switch(meState)
    {
    case MAIN:
    {
	int nX =mCurX;
	int nY =mCurY;
	
	bool running = false;
	int speed = mSpeed;
	
	if(CL_Keyboard::get_keycode( CL_KEY_SHIFT ) && mpLevel->allowsRunning())
	{
	    speed *= 2;
	}
	
	switch(key.id)
	{
	case CL_KEY_ESCAPE:
	    mbDone = true;
	    break;
	case CL_KEY_DOWN:
	    move(SOUTH,speed);
	    break;
	case CL_KEY_UP:
	    move(NORTH,speed);
	    break;
	case CL_KEY_LEFT:
	    move(WEST,speed);
	    break;
	case CL_KEY_RIGHT:
	    move(EAST,speed);
	    break;
#ifndef NDEBUG
	case CL_KEY_S:
	    mSpeed--;
	    break;
	case CL_KEY_F:
	    mSpeed++;
	    break;
	    
	case CL_KEY_P:
	mbPauseMovement = mbPauseMovement?false:true;
	break;
	
	case CL_KEY_D:
	    mbShowDebug = mbShowDebug?false:true;
	    break;
#endif
	default:
	    break;
	}
	break;
    }
    case TALKING:
	break;
    } // switch(meState)
    
}


void Application::onSignalQuit()
{
	
}


void Application::loadFonts()
{

#ifndef NDEBUG
    std::cout << "Loading fonts." << std::endl;
#endif

    mpfSBBlack = new CL_Font("Fonts/sb_black", mpResources);
    mpfBWhite = new CL_Font("Fonts/bold_white", mpResources);
    mpfBPowderBlue = new CL_Font("Fonts/bold_powder_blue",mpResources );
    mpfBGray = new CL_Font("Fonts/bold_gray", mpResources );
}

void Application::drawMap()
{
    CL_Rect dst(0,0, min(WINDOW_WIDTH, (const int)mpLevel->getWidth()*32),min(WINDOW_HEIGHT, (const int)mpLevel->getHeight() * 32));
    
    CL_Rect src = getLevelRect();
    
//	    CL_Rect src = dst;
    mpWindow->get_gc()->push_cliprect( dst);
    
    mpLevel->draw(src,dst, mpWindow->get_gc(), false,false,false);
    
    //mpWindow->get_gc()->draw_rect( CL_Rect(mCurX,mCurY,mCurX+64,mCurY+64), CL_Color::aqua ) ;
    
    drawPlayer();
    
    mpLevel->drawMappableObjects( src,dst, mpWindow->get_gc(), 
				  mbPauseMovement ? false:true);
    mpLevel->drawFloaters(src,dst, mpWindow->get_gc());
    
    
    mpWindow->get_gc()->pop_cliprect();
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
	std::string defaultplayersprite = CL_String::load("Game/DefaultPlayerSprite",mpResources );

	// Load special overlay for say.


	mpWindow  = new CL_DisplayWindow(name, WINDOW_WIDTH, WINDOW_HEIGHT);
		
	
	CL_Display::clear();

	mpSayOverlay = new CL_Surface("Overlays/say_overlay", mpResources );

	loadFonts();
	showRechargeableOnionSplash();
	showIntro();

	mpLevel = new Level(startinglevel, mpResources);

	mpPlayerSprite = new CL_Sprite(defaultplayersprite, mpResources );

	CL_System::sleep( 50 );

	CL_Slot slot_quit = mpWindow->sig_window_close().connect(this, &Application::onSignalQuit);

	CL_Slot slot_key_down = CL_Keyboard::sig_key_down().connect(this, &Application::onSignalKeyDown);

	CL_Display::clear();
	
	static int start_time = CL_System::get_time();
	static long fpscounter = 0;

	meState = MAIN;

	while(!mbDone)
	{
			
	    processActionQueue();


	    drawMap();


	    int cur_time = CL_System::get_time();
	    int delta_time = cur_time - start_time;	
	    start_time = cur_time;	


#ifndef NDEBUG
	    int fps = calc_fps ( delta_time );

	    if(mbShowDebug)
	    {
		mpfSBBlack->set_alpha(0.8);
//		mpfSBBlack->set_scale(2.0,2.0);
		mpfSBBlack->draw(0,0, mpLevel->getName());
		mpfSBBlack->draw(0,  mpfSBBlack->get_height(), std::string("FPS: ") +IntToString(fps) );
	    }
	    
#endif
	    
	    if(delta_time < 16)
	    {
		CL_System::keep_alive(16 - delta_time);
	    }
	    else
	    {
		CL_System::keep_alive();
	    }

	    CL_Display::flip();


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

int Application::calc_fps(int frame_time)
{
	static int fps_result = 0;
	static int fps_counter = 0;
	static int total_time = 0;
	
	total_time += frame_time;
	if(total_time >= 1000)	// One second has passed
	{
		fps_result = fps_counter + 1;
		fps_counter = total_time = 0;
	}
	fps_counter++;	// Increase fps

	return fps_result;
}


void Application::processActionQueue()
{

}


void Application::drawPlayer()
{
    uint frame = 0;



    switch(mePlayerDirection)
    {
    case NORTH:
	frame = mbStep? 6:7;
	break;
    case SOUTH:
	frame = mbStep? 4:5;
	break;
    case EAST:
	frame = mbStep? 0:1;
	break;
    case WEST:
	frame = mbStep? 2:3;
	break;
    }

    mpPlayerSprite->set_frame ( frame );

    mpPlayerSprite->draw( mCurX, mCurY, mpWindow->get_gc() );
}
