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

const int WINDOW_HEIGHT = 600 ;
const int WINDOW_WIDTH = 800 ;


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


void Application::playAnimation(const std::string &animation)
{
#ifndef NDEBUG
    std::cout << "Playing animation " << animation << std::endl;
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


const ItemManager * Application::getItemManager() const
{
    return &mItemManager;
}

const AbilityManager * Application::getAbilityManager() const
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
}

Application::~Application()
{
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




void Application::loadSpells(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading spells..." << std::endl;
#endif    

    CL_InputSource_File file(filename);

    CL_DomDocument document;
        
    document.load(&file);

    mAbilityManager.loadSpellFile ( document );

}

void Application::loadSkills(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading skills..." << std::endl;
#endif    

    CL_InputSource_File file(filename);

    CL_DomDocument document;
        
    document.load(&file);

    mAbilityManager.loadSkillFile ( document );

}

void Application::loadStatusEffects(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading status effects...." << std::endl;
#endif

    CL_InputSource_File file(filename);

    CL_DomDocument document;

    document.load(&file);

    mAbilityManager.loadStatusEffectFile( document );
}

void Application::loadCharacterClasses(const std::string &filename)
{
#ifndef NDEBUG
	std::cout << "Loading character classes..." << std::endl;
#endif

	CL_InputSource_File file(filename);

	CL_DomDocument document;

	document.load(&file);

	mAbilityManager.loadCharacterClassFile( document );
}

void Application::loadItems(const std::string &filename)
{
#ifndef NDEBUG
    std::cout << "Loading items..." << std::endl;
#endif    

  
    CL_InputSource_File file(filename);

    CL_DomDocument document;
        
    document.load(&file);

    mItemManager.loadItemFile ( document );

}
void Application::onSignalMovementTimer()
{

    mMapState.moveMappableObjects();

    mStates.back()->mappableObjectMoveHook();

}

void Application::draw()
{
    CL_Rect dst = getDisplayRect();

    mpWindow->get_gc()->push_cliprect( dst);


    for(std::vector<State*>::iterator iState = mStates.begin();
        iState != mStates.end(); iState++)
    {
        (*iState)->draw(dst, mpWindow->get_gc());
        
        if((*iState)->lastToDraw()) break; // Don't draw any further.

    }
    
    mpWindow->get_gc()->pop_cliprect();

}

void Application::run()
{

    if ( mStates.back()->disableMappableObjects())
        mpMovementTimer->disable(); 

    mStates.back()->start();

    while(!mStates.back()->isDone())
    {
        draw();
        CL_System::keep_alive();
        CL_Display::flip();
    }


    mStates.back()->finish();
    if(mStates.back()->disableMappableObjects())
        mpMovementTimer->enable(); // It would have been disabled. So re-enable it.

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
		std::string itemdefinition = CL_String::load("Game/ItemDefinitions", mpResources );
        std::string statusEffectDefinition = CL_String::load("Game/StatusEffectDefinitions",mpResources);
        std::string spelldefinition = CL_String::load("Game/SpellDefinitions", mpResources);
		std::string skilldefinition = CL_String::load("Game/SkillDefinitions",mpResources);
		std::string classdefinition = CL_String::load("Game/CharacterClassDefinitions",mpResources);
        // Load special overlay for say.


        mpWindow  = new CL_DisplayWindow(name, WINDOW_WIDTH, WINDOW_HEIGHT);

	//	mpWindow->get_buffer(0).to_format(CL_PixelFormat(24,0,0,0,0,false,0,pixelformat_rgba));

                
        
        CL_Display::clear();


        showRechargeableOnionSplash();
        showIntro();
        loadStatusEffects(statusEffectDefinition);
        loadSpells(spelldefinition);
        loadItems(itemdefinition);
		loadSkills(skilldefinition);
		loadCharacterClasses(classdefinition);
        Level * pLevel = new Level(startinglevel, mpResources);

        mMapState.pushLevel ( pLevel, 1,1 );  
        mMapState.setDimensions(getDisplayRect());

        mStates.push_back( &mMapState );

        CL_Slot slot_quit = mpWindow->sig_window_close().connect(this, &Application::onSignalQuit);

        CL_Slot slot_key_down = CL_Keyboard::sig_key_down().connect(this, &Application::onSignalKeyDown);

        CL_Slot slot_key_up  = CL_Keyboard::sig_key_up().connect(this, &Application::onSignalKeyUp);

        CL_Display::clear();
        
        static int start_time = CL_System::get_time();
        static long fpscounter = 0;

        mpMovementTimer = new CL_Timer(32);
        CL_Slot slot_mo_timer = mpMovementTimer->sig_timer().connect(this,&Application::onSignalMovementTimer);
        CL_FramerateCounter frameRate;
        mpMovementTimer->enable();

        while(mStates.size())
        {
                        
        
            run();
                

        }
                

                

                
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


