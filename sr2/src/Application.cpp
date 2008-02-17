#include <ClanLib/display.h>
#include <ClanLib/core.h>
#include <ClanLib/gl.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include "Application.h"
#include "Level.h"
#include "Party.h"
#include "LevelFactory.h"
#include "ItemFactory.h"
#include "CharacterFactory.h"
#include "ChoiceState.h"
#include "MonsterRef.h"
#include "Monster.h"
#ifndef _WINDOWS_
#include <steel/SteelType.h>
#else
#include <SteelType.h>
#endif
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


SteelType Application::playScene(const std::string &animation)
{
#ifndef NDEBUG
    std::cout << "Playing scene " << animation << std::endl;
#endif

    return SteelType();
}

SteelType Application::playSound(const std::string &sound)
{
#ifndef NDEBUG
    std::cout << "Playing sound " << sound << std::endl;
#endif

    return SteelType();
}
SteelType Application::loadLevel(const std::string &level, uint startX, uint startY)
{
    Level * pLevel = new Level();
    pLevel->load(level, mpResources);
    pLevel->invoke();
    mMapState.pushLevel( pLevel, static_cast<uint>(startX), static_cast<uint>(startY) );

    return SteelType();
}

void Application::pop(bool bAll)
{
    mMapState.pop(bAll);
}

SteelType Application::pop_(bool bAll)
{
    pop(bAll);

    return SteelType();
}

void Application::startBattle(const std::vector<MonsterRef*> &monsters)
{
#ifndef NDEBUG
    std::cout << "Encounter!" << std::endl;
    for(std::vector<MonsterRef*>::const_iterator it =  monsters.begin();
        it != monsters.end(); it++)
    {
        MonsterRef * pRef = *it;
        std::cout << '\t' << pRef->getName() << " x" << pRef->getCount() << std::endl;
        Monster * pMonster = mCharacterManager.getMonster(pRef->getName());
        pMonster->invoke();
    }
#endif
}

SteelType Application::startBattle(const std::string &monster, uint count, bool isBoss)
{
#ifndef NDEBUG
    std::cout << "Start battle " << monster << std::endl;
#endif

    // TODO: Return false if you lose, true if you win.
    return SteelType();
}

SteelType Application::choice(const std::string &choiceText,
                              const std::vector<SteelType> &choices_)
{
    static ChoiceState choiceState;
    std::vector<std::string> choices;
    choices.reserve ( choices_.size() );

    for(unsigned int i=0;i<choices_.size();i++)
        choices.push_back ( choices_[i] );

    choiceState.init(choiceText,choices);
    mStates.push_back ( &choiceState );
    run(); // Run pops for us.

    SteelType selection;
    selection.set( choiceState.getSelection() );

    return selection;
}


SteelType Application::say(const std::string &speaker, const std::string &text)
{
    mSayState.init(speaker,text);

    mStates.push_back(&mSayState);

    run();

    return SteelType();
}

void Application::showError(int line, const std::string &script, const std::string &message)
{
    std::ostringstream os;
    os << "Script error in " << script << " on line " << line << " (" << message << ')';

    say("Error", os.str());
}

SteelType Application::pause(uint time)
{
    CL_System::sleep(time);

    return SteelType();
}

SteelType Application::invokeShop(const std::string &shoptype)
{
    return SteelType();
}


SteelType Application::getGold()
{
    SteelType val;
    val.set ( mpParty->getGold() );
    return val;
}

SteelType Application::hasItem(const std::string &item, uint count)
{
    ItemManager * pMgr = IApplication::getInstance()->getItemManager();
    assert ( pMgr );
    SteelType var;
    var.set ( mpParty->hasItem(pMgr->getNamedItem(item),count) );

    return var;
}

SteelType Application::didEvent(const std::string &event)
{
    SteelType var;
    var.set ( mpParty->didEvent(event ) );

    return var;
}

SteelType Application::doEvent(const std::string &event, bool bRemember)
{
    mpParty->doEvent ( event, bRemember );
    return SteelType();
}

SteelType Application::giveNamedItem(const std::string &item, uint count)
{
    std::ostringstream os;
    ItemManager * pMgr = IApplication::getInstance()->getItemManager();
    assert ( pMgr );
    mpParty->giveItem ( pMgr->getNamedItem(item), count );

    os << "You received " << item;

    if(count > 1)
        os << " x" << count;

    say("Item Received",os.str());

    return SteelType();
}

SteelType Application::takeNamedItem(const std::string  &item, uint count)
{
    std::ostringstream os;
    ItemManager * pMgr = IApplication::getInstance()->getItemManager();
    SteelType val;
    val.set ( mpParty->giveItem ( pMgr->getNamedItem(item), count ) );

    os << "Gave up " << item;

    if(count > 1)
        os << " x" << count;

    say("Item Lost",os.str());

    return val;
}

SteelType Application::giveGold(int amount)
{
    std::ostringstream os;
    mpParty->giveGold(amount);

    if(amount > 0)
    {
        os << "You received " << amount << ' ' << mGold << '.';
        say(mGold, os.str());
    }
    else if ( amount > 0)
    {
        os << "Lost " << amount << ' ' << mGold << '.';
        say(mGold,os.str());
    }

    return SteelType();
}

SteelType Application::addCharacter(const std::string &character, int level, bool announce)
{
    Character * pCharacter = mCharacterManager.getCharacter(character);
    pCharacter->fixAttribute(ICharacter::CA_LEVEL,static_cast<double>(level));
    mpParty->addCharacter(pCharacter);

    if(announce)
    {
        std::ostringstream os;
        os << character << " joined the party!";

        say("",os.str());
    }

    return SteelType();
}

SteelType Application::useItem()
{
    return SteelType();
}


IApplication * IApplication::getInstance()
{
    return &sr_app;
}


IParty * Application::getParty() const
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


CL_ResourceManager * Application::getResources() const
{
    return mpResources;
}


Application::Application():mpParty(0),
                           mbDone(false)
    
{
    mpParty = new Party();
}

Application::~Application()
{

}


ICharacterGroup * Application::getTargetCharacterGroup() const
{
    return mpParty;
}

ICharacterGroup * Application::getActorCharacterGroup() const
{
    return mpParty;
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

AstScript * Application::loadScript(const std::string &name, const std::string &script)
{
    return mInterpreter.prebuildAst(name,script);
}

SteelType Application::runScript(AstScript * pScript)
{
    // Intentionally letting steel exceptions 
    // Get caught by a higher layer
    return mInterpreter.runAst ( pScript );

}
SteelType Application::runScript(AstScript *pScript, const ParameterList &params)
{
    return mInterpreter.runAst ( pScript, params );
}

void Application::registerSteelFunctions()
{
    mInterpreter.addFunction("say",
                             new SteelFunctor2Arg<Application,const std::string&,const std::string&>
                             (this,&Application::say));
    mInterpreter.addFunction("playScene",
                             new SteelFunctor1Arg<Application,const std::string&>
                             (this,&Application::playScene));
    mInterpreter.addFunction("playSound",
                             new SteelFunctor1Arg<Application,const std::string&>
                             (this,&Application::playSound));
    mInterpreter.addFunction("loadLevel",
                             new SteelFunctor3Arg<Application,const std::string&,uint,uint>
                             (this,&Application::loadLevel));
    mInterpreter.addFunction("startBattle",
                             new SteelFunctor3Arg<Application,const std::string &,uint,bool>
                             (this,&Application::startBattle));
    mInterpreter.addFunction("pause",
                             new SteelFunctor1Arg<Application,uint>(this,&Application::pause));
    mInterpreter.addFunction("choice", 
                             new SteelFunctor2Arg<Application,const std::string&, const std::vector<SteelType> &>
                             (this,&Application::choice));
    mInterpreter.addFunction("pop",
                             new SteelFunctor1Arg<Application,bool>(this,&Application::pop_));
    mInterpreter.addFunction("giveNamedItem",
                             new SteelFunctor2Arg<Application,const std::string &,uint>
                             (this,&Application::giveNamedItem));
    mInterpreter.addFunction("takeNamedItem",
                             new SteelFunctor2Arg<Application,const std::string &,uint>
                             (this,&Application::takeNamedItem));
    mInterpreter.addFunction("getGold",
                             new SteelFunctorNoArgs<Application>(this,&Application::getGold));
    mInterpreter.addFunction("hasItem",
                             new SteelFunctor2Arg<Application,const std::string &,uint>
                             (this,&Application::hasItem));
    mInterpreter.addFunction("didEvent",
                             new SteelFunctor1Arg<Application,const std::string &>
                             (this,&Application::didEvent));
    mInterpreter.addFunction("doEvent",
                             new SteelFunctor2Arg<Application,const std::string &, bool>
                             (this,&Application::doEvent));
    mInterpreter.addFunction("giveGold",
                             new SteelFunctor1Arg<Application,int>
                             (this,&Application::giveGold));
    mInterpreter.addFunction("addCharacter",
        new SteelFunctor3Arg<Application,const std::string &, int, bool>
        (this,&Application::addCharacter));


//        SteelType hasGeneratedWeapon(const std::string &wepclass, const std::string &webtype);
//       SteelType hasGeneratedArmor(const std::string &armclass, const std::string &armtype);

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
#if 0
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
 
    setupClanLib();

    //CL_Display::get_buffer()
    try
    {
        registerSteelFunctions();

        mpResources = new CL_ResourceManager ( "Media/resources.xml" );

#ifdef NDEBUG
        std::string name = CL_String::load("Configuration/name", mpResources);
#else
        std::string name = CL_String::load("Configuration/name", mpResources) + " (DEBUG)";
#endif
        mGold = CL_String::load("Game/Currency",mpResources);
        std::string startinglevel = CL_String::load("Game/StartLevel",mpResources);

        
        // Load special overlay for say.
        mpWindow  = new CL_OpenGLWindow(name, WINDOW_WIDTH, WINDOW_HEIGHT,false,false,2);

        //for(int i =0; i < mpWindow->get_buffer_count(); i++)
        //  mpWindow->get_buffer(i).to_format(CL_PixelFormat(24,0,0,0,0,false,0,pixelformat_rgba));

        CL_Display::clear();

        mAppUtils.loadGameplayAssets("",mpResources);
            
        showRechargeableOnionSplash();
        showIntro();
            
        Level * pLevel = new Level();
        pLevel->load(startinglevel, mpResources);
        pLevel->invoke();
            
        mMapState.setDimensions(getDisplayRect());
        mMapState.pushLevel ( pLevel, 1,1 );  
            
        mStates.push_back( &mMapState );
    }
    catch(CL_Error error)
    {
        std::cerr << "Exception Caught!!" << std::endl;
        std::cerr << error.message.c_str() << std::endl;
            
#ifndef NDEBUG
        console.display_close_message();
#endif
        return 1;
    }
    catch(SteelException ex)
    {
        std::cerr << "Steel Exception on line " << ex.getLine() 
                  << " of " << ex.getScript() << ':' << ex.getMessage() << std::endl;
#ifndef NDEBUG
        console.display_close_message();
#endif
        return 1;
    }

              
    CL_Slot slot_quit = mpWindow->sig_window_close().connect(this, &Application::onSignalQuit);
    CL_Slot slot_key_down = CL_Keyboard::sig_key_down().connect(this, &Application::onSignalKeyDown);
    CL_Slot slot_key_up  = CL_Keyboard::sig_key_up().connect(this, &Application::onSignalKeyUp);


    try
    {

        CL_Display::clear();
            
        static int start_time = CL_System::get_time();
        static long fpscounter = 0;
            
        while(mStates.size())
            run();
            
#ifndef NDEBUG
        console.display_close_message();
#endif
            
            
        teardownClanLib();
    }
    catch(SteelException ex)
    {
        while(mStates.size()) 
            mStates.pop_back();

        showError ( ex.getLine(), ex.getScript(), ex.getMessage() );
            

    }
    catch(CL_Error error)
    {
        std::cerr << "Exception Caught!!" << std::endl;
        std::cerr << error.message.c_str() << std::endl;
            
#ifndef NDEBUG
        console.display_close_message();
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



    while(!CL_Keyboard::get_keycode(CL_KEY_ENTER))
    {

        background.draw(0,0);
        splash.draw(static_cast<float>(displayX),
                    static_cast<float>(displayY));

        CL_Display::flip();
        CL_System::keep_alive();
    }

    // Wait for them to release the key before moving on.
    while(CL_Keyboard::get_keycode(CL_KEY_ENTER)) CL_System::keep_alive();

        

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










