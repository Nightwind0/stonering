#include <ClanLib/display.h>
#include <ClanLib/core.h>
#include <ClanLib/gl.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <fstream>
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


int Application::GetScreenWidth()const
{
    return WINDOW_WIDTH;
}

int Application::GetScreenHeight()const
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
    pLevel->Load(level, mpResources);
    pLevel->Invoke();
    mMapState.PushLevel( pLevel, static_cast<uint>(startX), static_cast<uint>(startY) );

    return SteelType();
}

void Application::Pop(bool bAll)
{
    mMapState.Pop(bAll);
}

SteelType Application::pop_(bool bAll)
{
    Pop(bAll);

    return SteelType();
}

void Application::StartBattle(const MonsterGroup &group, const std::string &backdrop)
{
#ifndef NDEBUG
    std::cout << "Encounter! Backdrop = " << backdrop << std::endl;

    const std::vector<MonsterRef*> &monsters = group.GetMonsters();

    for(std::vector<MonsterRef*>::const_iterator it =  monsters.begin();
        it != monsters.end(); it++)
    {
        MonsterRef * pRef = *it;
        std::cout << '\t' << pRef->GetName() << " x" << pRef->GetCount() << std::endl;
    }
#endif
    mBattleState.init(group,backdrop);

    mStates.push_back(&mBattleState);

    run();
}

SteelType Application::startBattle(const std::string &monster, uint count, bool isBoss, const std::string &backdrop)
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

    choiceState.Init(choiceText,choices);
    mStates.push_back ( &choiceState );
    run(); // Run pops for us.

    SteelType selection;
    selection.set( choiceState.GetSelection() );

    return selection;
}


SteelType Application::say(const std::string &speaker, const std::string &text)
{
    mSayState.Init(speaker,text);

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
    val.set ( mpParty->GetGold() );
    return val;
}

SteelType Application::hasItem(const std::string &item, uint count)
{
    ItemManager * pMgr = IApplication::GetInstance()->GetItemManager();
    assert ( pMgr );
    SteelType var;
    var.set ( mpParty->HasItem(pMgr->GetNamedItem(item),count) );

    return var;
}

SteelType Application::didEvent(const std::string &event)
{
    SteelType var;
    var.set ( mpParty->DidEvent(event ) );

    return var;
}

SteelType Application::doEvent(const std::string &event, bool bRemember)
{
    mpParty->DoEvent ( event, bRemember );
    return SteelType();
}

SteelType Application::giveNamedItem(const std::string &item, uint count)
{
    std::ostringstream os;
    ItemManager * pMgr = IApplication::GetInstance()->GetItemManager();
    assert ( pMgr );
    mpParty->GiveItem ( pMgr->GetNamedItem(item), count );

    os << "You received " << item;

    if(count > 1)
        os << " x" << count;

    say("Item Received",os.str());

    return SteelType();
}

SteelType Application::takeNamedItem(const std::string  &item, uint count)
{
    std::ostringstream os;
    ItemManager * pMgr = IApplication::GetInstance()->GetItemManager();
    SteelType val;
    val.set ( mpParty->GiveItem ( pMgr->GetNamedItem(item), count ) );

    os << "Gave up " << item;

    if(count > 1)
        os << " x" << count;

    say("Item Lost",os.str());

    return val;
}

SteelType Application::giveGold(int amount)
{
    std::ostringstream os;
    mpParty->GiveGold(amount);

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
    Character * pCharacter = mCharacterManager.GetCharacter(character);
    pCharacter->SetLevel(level);
    mpParty->AddCharacter(pCharacter);

    if(announce)
    {
        std::ostringstream os;
        os << character << " joined the party!";

        say("",os.str());
    }
    SteelType returnPointer;
    returnPointer.set(pCharacter);

    return returnPointer;
}

SteelType Application::doDamage(SteelType::Handle hICharacter, int damage)
{
    ICharacter * pCharacter = static_cast<ICharacter*>(hICharacter);

    if(!pCharacter->GetToggle(Character::CA_ALIVE)) return SteelType();

    int hp = pCharacter->GetAttribute(Character::CA_HP);
    const int maxhp = pCharacter->GetAttribute(Character::CA_MAXHP);

    if(hp - damage<=0)
    {
        damage = hp;
        // Kill him/her/it
        pCharacter->Kill();
        //TODO: Check if all characters are dead and game over
    }

    if(hp - damage >maxhp)
    {
        damage = maxhp - hp;
    }

    pCharacter->PermanentAugment(Character::CA_HP,damage);

    SteelType newhp;
    newhp.set(damage);

    return newhp;
}


SteelType Application::useItem()
{
    return SteelType();
}

SteelType Application::getCharacterName(const SteelType::Handle handle)
{
    ICharacter * pCharacter = static_cast<Character*>(handle);
    SteelType name;
    name.set(pCharacter->GetName());

    return name;
}

SteelType Application::getPartyCount(void)
{
    SteelType count;
    count.set((int)mpParty->GetCharacterCount());

    return count;
}

SteelType Application::getCharacter(uint index)
{
    SteelType pointer;
    pointer.set(static_cast<SteelType::Handle>(mpParty->GetCharacter(index)));
    return pointer;
}

SteelType Application::getCharacterLevel(const SteelType::Handle hCharacter)
{
    ICharacter *pCharacter = static_cast<ICharacter*>(hCharacter);
    SteelType level;
    level.set((int)pCharacter->GetLevel());

    return level;
}

SteelType Application::addStatusEffect(SteelType::Handle hCharacter, const std::string &effect)
{
    ICharacter *pCharacter = static_cast<ICharacter*>(hCharacter);
    pCharacter->AddStatusEffect( mAbilityManager.GetStatusEffect(effect) );

    return SteelType();
}

SteelType Application::removeStatusEffects(SteelType::Handle hCharacter, const std::string &effect)
{
    ICharacter *pCharacter = static_cast<ICharacter*>(hCharacter);
    pCharacter->RemoveEffects(effect);

    return SteelType();
}

IApplication * IApplication::GetInstance()
{
    return &sr_app;
}


IParty * Application::GetParty() const
{
    return mpParty;
}


ItemManager * Application::GetItemManager()
{
    return &mItemManager;
}

AbilityManager * Application::GetAbilityManager()
{
    return &mAbilityManager;
}


CL_ResourceManager * Application::GetResources() const
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


ICharacterGroup * Application::GetTargetCharacterGroup() const
{
    return mpParty;
}

ICharacterGroup * Application::GetActorCharacterGroup() const
{
    return mpParty;
}

CL_Rect Application::GetDisplayRect() const
{
    return CL_Rect(0,0,GetScreenWidth(), GetScreenHeight());

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

    mStates.back()->HandleKeyDown(key);
}

void Application::onSignalKeyUp(const CL_InputEvent &key)
{
    mStates.back()->HandleKeyUp(key);
}


void Application::onSignalQuit()
{
    
}

void Application::RequestRedraw(const State * /*pState*/)
{
    draw();
}

AstScript * Application::LoadScript(const std::string &name, const std::string &script)
{
    return mInterpreter.prebuildAst(name,script);
}

SteelType Application::RunScript(AstScript * pScript)
{
    // Intentionally letting steel exceptions 
    // Get caught by a higher layer
    return mInterpreter.runAst ( pScript );

}
SteelType Application::RunScript(AstScript *pScript, const ParameterList &params)
{
    return mInterpreter.runAst ( pScript, params );
}

void Application::registerSteelFunctions()
{
    static SteelFunctor2Arg<Application,const std::string&,const std::string&> fn_say(this,&Application::say);
    static SteelFunctor1Arg<Application,const std::string&> fn_playScene(this,&Application::playScene);
    static SteelFunctor1Arg<Application,const std::string&> fn_playSound(this,&Application::playSound);
    static SteelFunctor3Arg<Application,const std::string&,uint,uint> fn_loadLevel(this,&Application::loadLevel);
    static SteelFunctor4Arg<Application,const std::string &,uint,bool,const std::string&> fn_startBattle(this,&Application::startBattle);
    static SteelFunctor1Arg<Application,uint> fn_pause(this,&Application::pause);
    static SteelFunctor2Arg<Application,const std::string&, const std::vector<SteelType> &> fn_choice(this,&Application::choice);
    static SteelFunctor1Arg<Application,bool> fn_pop(this,&Application::pop_);
    static SteelFunctor2Arg<Application,const std::string &,uint> fn_giveNamedItem(this,&Application::giveNamedItem);
    static SteelFunctor2Arg<Application,const std::string &,uint> fn_takeNamedItem(this,&Application::takeNamedItem);
    static SteelFunctorNoArgs<Application> fn_getGold(this,&Application::getGold);
    static SteelFunctor2Arg<Application,const std::string &,uint> fn_hasItem (this,&Application::hasItem);
    static SteelFunctor1Arg<Application,const std::string &> fn_didEvent (this,&Application::didEvent);
    static SteelFunctor2Arg<Application,const std::string &, bool> fn_doEvent (this,&Application::doEvent);
    static SteelFunctor1Arg<Application,int> fn_giveGold(this,&Application::giveGold);
    static SteelFunctor3Arg<Application,const std::string &, int, bool> fn_addCharacter(this,&Application::addCharacter);
    static SteelFunctorNoArgs<Application> fn_getPartyCount(this, &Application::getPartyCount);
    static SteelFunctor1Arg<Application,uint> fn_getCharacter(this, &Application::getCharacter);

    static SteelFunctor1Arg<Application,const SteelType::Handle> fn_getCharacterName(this,&Application::getCharacterName);
    static SteelFunctor2Arg<Application,const SteelType::Handle,const std::string&> fn_addStatusEffect(this,&Application::addStatusEffect);
    static SteelFunctor2Arg<Application,const SteelType::Handle,const std::string&> fn_removeStatusEffects(this,&Application::removeStatusEffects);
    static SteelFunctor2Arg<Application,const SteelType::Handle,int> fn_doDamage(this,&Application::doDamage);

    mInterpreter.pushScope();

    steelConst("_HP",Character::CA_HP);
    steelConst("_MP",Character::CA_MP);
    steelConst("_STR",Character::CA_STR);
    steelConst("_DEF",Character::CA_DEF);
    steelConst("_DEX",Character::CA_DEX);
    steelConst("_EVD",Character::CA_EVD);
    steelConst("_MAG",Character::CA_MAG);
    steelConst("_RST",Character::CA_RST);
    steelConst("_LCK",Character::CA_LCK);
    steelConst("_JOY",Character::CA_JOY);

    steelConst("_DRAW_ILL",Character::CA_DRAW_ILL);
    steelConst("_DRAW_STONE",Character::CA_DRAW_STONE);
    steelConst("_DRAW_BERSERK",Character::CA_DRAW_BERSERK);
    steelConst("_DRAW_WEAK",Character::CA_DRAW_WEAK);
    steelConst("_DRAW_PARALYZED",Character::CA_DRAW_PARALYZED);
    steelConst("_DRAW_TRANSLUCENT",Character::CA_DRAW_TRANSLUCENT);
    steelConst("_DRAW_MINI",Character::CA_DRAW_MINI);
    steelConst("_CAN_ACT",Character::CA_CAN_ACT);
    steelConst("_CAN_FIGHT",Character::CA_CAN_FIGHT);
    steelConst("_CAN_CAST",Character::CA_CAN_CAST);
    steelConst("_CAN_SKILL",Character::CA_CAN_SKILL);
    steelConst("_CAN_ITEM",Character::CA_CAN_ITEM);
    steelConst("_CAN_RUN",Character::CA_CAN_RUN);
    steelConst("_ALIVE",Character::CA_ALIVE);

    steelConst("_MAXHP",Character::CA_MAXHP);
    steelConst("_MAXMP",Character::CA_MAXMP);

    mInterpreter.addFunction("say",&fn_say);
    mInterpreter.addFunction("playScene", &fn_playScene);
    mInterpreter.addFunction("playSound", &fn_playSound);
    mInterpreter.addFunction("loadLevel", &fn_loadLevel);
    mInterpreter.addFunction("startBattle", &fn_startBattle);
    mInterpreter.addFunction("pause",&fn_pause);
    mInterpreter.addFunction("choice", &fn_choice);
    mInterpreter.addFunction("pop", &fn_pop);
    mInterpreter.addFunction("giveNamedItem", &fn_giveNamedItem );
    mInterpreter.addFunction("takeNamedItem", &fn_takeNamedItem );
    mInterpreter.addFunction("getGold", &fn_getGold );
    mInterpreter.addFunction("hasItem", &fn_hasItem );
    mInterpreter.addFunction("didEvent", &fn_didEvent );
    mInterpreter.addFunction("doEvent", &fn_doEvent );
    mInterpreter.addFunction("giveGold", &fn_giveGold );
    mInterpreter.addFunction("addCharacter", &fn_addCharacter );
    mInterpreter.addFunction("getPartyCount", &fn_getPartyCount);
    mInterpreter.addFunction("getCharacter", &fn_getCharacter);

    mInterpreter.addFunction("getCharacterName", &fn_getCharacterName);
    mInterpreter.addFunction("addStatusEffect", &fn_addStatusEffect);
    mInterpreter.addFunction("removeStatusEffects", &fn_removeStatusEffects);
    mInterpreter.addFunction("doDamage",&fn_doDamage);

//        SteelType hasGeneratedWeapon(const std::string &wepclass, const std::string &webtype);
//       SteelType hasGeneratedArmor(const std::string &armclass, const std::string &armtype);

}

void Application::draw()
{
    CL_Rect dst = GetDisplayRect();

    mpWindow->get_gc()->push_cliprect( dst);

    std::vector<State*>::iterator end = mStates.end();

    for(std::vector<State*>::iterator iState = mStates.begin();
        iState != end; iState++)
    {
        State * pState = *iState;
        pState->Draw(dst, mpWindow->get_gc());
        
        if(pState->LastToDraw()) break; // Don't draw any further.

    }
    
    mpWindow->get_gc()->pop_cliprect();
}

void Application::run()
{
    CL_FramerateCounter frameRate;
    static int count = 0;
    State * backState = mStates.back();

    backState->SteelInit(&mInterpreter);
    backState->Start();
    unsigned int then = CL_System::get_time();
    while(!backState->IsDone())
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
            if ( !backState->DisableMappableObjects())
            {
                mMapState.MoveMappableObjects();
                mStates.back()->MappableObjectMoveHook();
            }
            then = now;
        }

        
    }


    mStates.back()->Finish();
    mStates.back()->SteelCleanup(&mInterpreter);
  
    mStates.pop_back();


}

void Application::loadscript(std::string &o_str, const std::string & filename)
{
    std::ifstream in;
    in.open(filename.c_str());
    char c;
    while(in >> c)
    {
        o_str += c;
    }
    in.close();
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
   
       
        // Load special overlay for say.
        mpWindow  = new CL_OpenGLWindow(name, WINDOW_WIDTH, WINDOW_HEIGHT,false,false,2);

        //for(int i =0; i < mpWindow->get_buffer_count(); i++)
        //  mpWindow->get_buffer(i).to_format(CL_PixelFormat(24,0,0,0,0,false,0,pixelformat_rgba));

        CL_Display::clear();

        mAppUtils.LoadGameplayAssets("",mpResources);
        std::string startinglevel = CL_String::load("Game/StartLevel",mpResources);
        std::string initscript;
        loadscript(initscript,CL_String::load("Game/StartupScript",mpResources));
        mInterpreter.run("Init",initscript);
            
        showRechargeableOnionSplash();
        showIntro();
            
        Level * pLevel = new Level();
        pLevel->Load(startinglevel, mpResources);
        pLevel->Invoke();
            
        mMapState.SetDimensions(GetDisplayRect());
        mMapState.PushLevel ( pLevel, 1,1 );  
            
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
        
    mInterpreter.popScope();

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










