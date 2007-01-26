#include <ClanLib/display.h>
#include <ClanLib/core.h>
#include <ClanLib/gl.h>
#include <iostream>
#include <sstream>
#include "Application.h"
#include "Level.h"
#include "Party.h"
#include "LevelFactory.h"
#include "ItemFactory.h"
#include "CharacterFactory.h"
#include "ChoiceState.h"
//
//
//
using StoneRing::Application;
using namespace StoneRing;

#ifndef _MSC_VER

using std::min;
using std::max;

#endif

const int WINDOW_WIDTH = 800 ;
const int WINDOW_HEIGHT = 600 ;
const int MS_BETWEEN_MOVES = 20;


bool gbDebugStop;


Application sr_app;


int Application::getScreenWidth()const
{
    return WINDOW_WIDTH;
}

int Application::getScreenHeight()const
{
    return WINDOW_HEIGHT;
}


void Application::playScene(const std::string &animation)
{
#ifndef NDEBUG
    std::cout << "Playing scene " << animation << std::endl;
#endif

   
}

void Application::playSound(const std::string &sound)
{
#ifndef NDEBUG
    std::cout << "Playing sound " << sound << std::endl;
#endif

   
}
void Application::loadLevel(const std::string &level, uint startX, uint startY)
{
    mMapState.pushLevel( new Level(level,mpResources), startX, startY );
}

void Application::pop(bool bAll)
{
    mMapState.pop(bAll);
}

void Application::startBattle(const std::string &monster, uint count, bool isBoss)
{
#ifndef NDEBUG
    std::cout << "Start battle " << monster << std::endl;
#endif

}




void Application::choice(const std::string &choiceText,
                         const std::vector<std::string> &choices, Choice * pChoice)
{

    ChoiceState * pChoiceState = new ChoiceState();

    pChoiceState->init(choiceText,choices,pChoice);

    mStates.push_back ( pChoiceState );

    run(); // Run pops for us.

    delete pChoiceState;
}


void Application::say(const std::string &speaker, const std::string &text)
{
    mSayState.init(speaker,text);

    mStates.push_back(&mSayState);

    run();
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

ICharacterGroup * Application::getSelectedCharacterGroup() const
{
    return mpParty;
}


ItemManager * Application::getItemManager()
{
    return &mItemManager;
}

AbilityManager * Application::getAbilityManager()
{
    return &mAbilityManager;
}

LevelFactory *Application::getLevelFactory() const
{
    return mpLevelFactory;
}

AbilityFactory * Application::getAbilityFactory() const
{
    return mpAbilityFactory;
}

CharacterFactory * Application::getCharacterFactory() const
{
    return mpCharacterFactory;
}


ItemFactory * Application::getItemFactory() const
{
    return mpItemFactory;
}

CL_ResourceManager * Application::getResources() const
{
    return mpResources;
}


Application::Application():mpParty(0),mpLevelFactory(0),
                           mbDone(false)
    
{
    mpParty = new Party();

    // We want the generic level factory.
    mpLevelFactory = new LevelFactory();

    mpItemFactory = new ItemFactory();

    mpAbilityFactory = new AbilityFactory();

    mpCharacterFactory = new CharacterFactory();
}

Application::~Application()
{
    delete mpLevelFactory;
    delete mpItemFactory;
    delete mpAbilityFactory;
    delete mpCharacterFactory;
}


CL_Rect Application::getDisplayRect() const
{
    return CL_Rect(0,0,getScreenWidth(), getScreenHeight());

}

void Application::setupClanLib()
{
    CL_SetupCore::init();
    CL_SetupGL::init();
    CL_SetupDisplay::init();
   
        
}

void Application::teardownClanLib()
{
    CL_SetupGL::deinit();
    CL_SetupDisplay::deinit();
    CL_SetupCore::deinit();
}


void Application::onSignalKeyDown(const CL_InputEvent &key)
{

    mStates.back()->handleKeyDown(key);
}

void Application::onSignalKeyUp(const CL_InputEvent &key)
{
    mStates.back()->handleKeyUp(key);
}


void Application::onSignalQuit()
{
    
}

void Application::requestRedraw(const State * /*pState*/)
{
    draw();
}

void Application::draw()
{
    CL_Rect dst = getDisplayRect();

    mpWindow->get_gc()->push_cliprect( dst);

    std::vector<State*>::iterator end = mStates.end();

    for(std::vector<State*>::iterator iState = mStates.begin();
        iState != end; iState++)
    {
        State * pState = *iState;
        pState->draw(dst, mpWindow->get_gc());
        
        if(pState->lastToDraw()) break; // Don't draw any further.

    }
    
    mpWindow->get_gc()->pop_cliprect();
}

void Application::run()
{
    CL_FramerateCounter frameRate;
    static int count = 0;
    State * backState = mStates.back();

    backState->start();
    unsigned int then = CL_System::get_time();
    while(!backState->isDone())
    {

        draw();
        CL_Display::flip();
        //CL_System::sleep(10);
        CL_System::keep_alive();
#ifndef NDEBUG
        if(count++ % 50 == 0)
            std::cout << "FPS " <<  frameRate.get_fps() << std::endl;
#endif  
        unsigned int now = CL_System::get_time();

        if(now - then > MS_BETWEEN_MOVES)
        {
            if ( !backState->disableMappableObjects())
            {
                mMapState.moveMappableObjects();
                mStates.back()->mappableObjectMoveHook();
            }
            then = now;
        }

        
    }


    mStates.back()->finish();
  
    mStates.pop_back();


}


int Application::main(int argc, char ** argv)
{
  
#ifndef NDEBUG

    CL_ConsoleWindow console("Stone Ring Debug",80,1000);
    console.redirect_stdio();
#endif
        
    try
    {

        setupClanLib();

        //CL_Display::get_buffer()
                
        mpResources = new CL_ResourceManager ( "Media/resources.xml" );

#ifdef NDEBUG
        std::string name = CL_String::load("Configuration/name", mpResources);
#else
        std::string name = CL_String::load("Configuration/name", mpResources) + " (DEBUG)";
#endif

        std::string startinglevel = CL_String::load("Game/StartLevel",mpResources);
        mAppUtils.loadGameItemsAndSkills("",mpResources);

        // Load special overlay for say.


        mpWindow  = new CL_OpenGLWindow(name, WINDOW_WIDTH, WINDOW_HEIGHT,false,false,3);
        //mpWindow->set_buffer_count(3);

#ifndef NDEBUG
        std::cout << "Back Buffer Depth" <<mpWindow->get_back_buffer().get_format().get_depth() << std::endl;
        std::cout << "Front Buffer Depth" << mpWindow->get_front_buffer().get_format().get_depth() << std::endl;
        std::cout << "Back Buffer Red" << std::hex << mpWindow->get_back_buffer().get_format().get_red_mask() << std::endl;
        std::cout << "Front Buffer Red" << std::hex << mpWindow->get_front_buffer().get_format().get_red_mask() << std::endl;
        std::cout << "Back Buffer Green" << std::hex << mpWindow->get_back_buffer().get_format().get_green_mask() << std::endl;
        std::cout << "Front Buffer Green" << std::hex << mpWindow->get_front_buffer().get_format().get_green_mask() << std::endl;
        std::cout << "Back Buffer Blue" << std::hex << mpWindow->get_back_buffer().get_format().get_blue_mask() << std::endl;
        std::cout << "Front Buffer Blue" << std::hex << mpWindow->get_front_buffer().get_format().get_blue_mask() << std::endl;
        std::cout << "Back Buffer Alpha" << std::hex << mpWindow->get_back_buffer().get_format().get_alpha_mask() << std::endl;
        std::cout << "Front Buffer Alpha" << std::hex << mpWindow->get_front_buffer().get_format().get_alpha_mask() << std::endl;
        std::cout << std::dec;
#endif
        
        //for(int i =0; i < mpWindow->get_buffer_count(); i++)
        //  mpWindow->get_buffer(i).to_format(CL_PixelFormat(24,0,0,0,0,false,0,pixelformat_rgba));


    
        CL_Display::clear();

        showRechargeableOnionSplash();
        showIntro();

        Level * pLevel = new Level(startinglevel, mpResources);

        mMapState.setDimensions(getDisplayRect());
        mMapState.pushLevel ( pLevel, 1,1 );  
      

        mStates.push_back( &mMapState );

        CL_Slot slot_quit = mpWindow->sig_window_close().connect(this, &Application::onSignalQuit);

        CL_Slot slot_key_down = CL_Keyboard::sig_key_down().connect(this, &Application::onSignalKeyDown);

        CL_Slot slot_key_up  = CL_Keyboard::sig_key_up().connect(this, &Application::onSignalKeyUp);

        CL_Display::clear();
        
        static int start_time = CL_System::get_time();
        static long fpscounter = 0;

        while(mStates.size())
            run();
                
#ifndef NDEBUG
        std::string foo;
        std::getline(std::cin,foo);
#endif

                
        teardownClanLib();
    }
    catch(CL_Error error)
    {
        std::cerr << "Exception Caught!!" << std::endl;
        std::cerr << error.message.c_str() << std::endl;

#ifndef NDEBUG
        std::string foo;
        std::cin >> foo;
#endif
        
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
    if(total_time >= 1000)      // One second has passed
    {
        fps_result = fps_counter + 1;
        fps_counter = total_time = 0;
    }
    fps_counter++;      // Increase fps

    return fps_result;
}




