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
#ifndef NDEBUG
    std::cout << "Load level " << level << ':' << startX << ',' << startY << std::endl;
#endif

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
#if 0

    meState = CHOICE;
    startKeyUpQueue();

    mpMovementTimer->disable();

    static const CL_Rect choiceRect = CL_Rect(0,0, WINDOW_WIDTH, WINDOW_HEIGHT / 2);
    static const CL_Rect optionsRect = CL_Rect(0,WINDOW_HEIGHT / 2, WINDOW_WIDTH, WINDOW_HEIGHT);

    CL_Rect choiceTextRect = choiceRect;
    choiceTextRect.top += ( choiceRect.get_height() - mpfBWhite->get_height(choiceText, CL_Size(WINDOW_WIDTH, WINDOW_HEIGHT/2))) /2;

    bool done = false;

    uint optionOffset = 0;
    uint currentOption = 0;

    uint optionsPerPage = optionsRect.get_height() / (mpfBGray->get_height() + mpfBGray->get_height_offset()) ;

    CL_Font * pLineFont = NULL;

    while(!done)
    {
        drawMap();

        //      mpWindow->get_gc()->fill_rect( choiceRect, CL_Color(0,0,0,128) ) ;
        mpWindow->get_gc()->fill_rect( optionsRect, CL_Color(0,0,0,200) ) ;

        mpfBWhite->draw(choiceTextRect,choiceText,mpWindow->get_gc() );
        
        for(uint i=0;i< optionsPerPage; i++)
        {
            
            // Don't paint more options than there are
            if(i + optionOffset >= choices.size()) break;

            if(i + optionOffset == currentOption)
            {
                pLineFont = mpfBPowderBlue;

            }
            else
            {
                pLineFont = mpfBGray;
            }

            pLineFont->draw( 0,  i * (mpfBPowderBlue->get_height() + mpfBPowderBlue->get_height_offset()) + optionsRect.top,
                             choices[i + optionOffset], mpWindow->get_gc());

        }

        if(mKeyUpQueue.size())
        {
            int key = mKeyUpQueue.front();
            mKeyUpQueue.pop();

            switch(key)
            {
            case CL_KEY_ENTER:
            case CL_KEY_SPACE:
                done = true;
                break;
            case CL_KEY_DOWN:
                if(currentOption + 1< choices.size())
                {
                    currentOption++;
                    
                    if(currentOption > optionOffset + 4)
                    {
                        optionOffset++;
                    }
                }
                    
                break;
            case CL_KEY_UP:
                if(currentOption > 0 )
                {
                    currentOption--;

                    if(currentOption < optionOffset)
                    {
                        optionOffset--;
                    }
                }
                break;
            default:
                break;
                
            }
        }

        CL_Display::flip();
        
        CL_System::keep_alive();
    }

    pChoice->chooseOption ( currentOption );

    stopKeyUpQueue();

    mpMovementTimer->enable();

    meState = MAIN;
#endif
}


void Application::say(const std::string &speaker, const std::string &text)
{
#if 0
#ifndef NDEBUG
    std::cout << "Say: " << speaker << ":" << text << std::endl;
#endif

    meState = TALKING;

    mpMovementTimer->disable();
    
    // Start queueing up key up signals
    startKeyUpQueue();

    static const CL_Rect textRect(16,388, 783, 580);
    static const CL_Rect speakerRect(16,315,783,369);
   
    CL_Rect speakerTextRect = speakerRect;

    speakerTextRect.top += ( speakerRect.get_height() - mpfBGray->get_height()) /2;
    speakerTextRect.bottom += ( speakerRect.get_height() - mpfBGray->get_height()) /2;


    std::string::const_iterator textIter = text.begin();

    bool done = false;

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
            mpfBGray->draw(speakerTextRect, speaker.begin(),speaker.end(),mpWindow->get_gc() );

            // Process any KeyUps that were received. 
            if(mKeyUpQueue.size())
            {
                int key = mKeyUpQueue.front();
                mKeyUpQueue.pop();


                switch(key)
                {
                case CL_KEY_ENTER:
                case CL_KEY_SPACE:

                    if(totalDrawn + drawCount == text.size())
                    {
                        done = true;
                    }
                    
                    doneFrame = true;
                    
                    CL_System::sleep(333); // Part hack to prevent enter from being detected twice, partly for effect...
                    break;
                case CL_KEY_ESCAPE:
                    doneFrame = true;
                    done = true;
                }
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
    mpMovementTimer->enable();
    meState = MAIN;

    stopKeyUpQueue();
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


    for(std::vector<State*>::reverse_iterator iState = mStates.rbegin();
	iState != mStates.rend(); iState++)
    {
	(*iState)->draw(dst, mpWindow->get_gc());
	
	if((*iState)->lastToDraw()) break; // Don't draw any further.

    }
    
    mpWindow->get_gc()->pop_cliprect();

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
                
        mpResources = new CL_ResourceManager ( "Media/resources.xml" );

#ifdef NDEBUG
        std::string name = CL_String::load("Configuration/name", mpResources);
#else
        std::string name = CL_String::load("Configuration/name", mpResources) + " (DEBUG)";
#endif
        std::string startinglevel = CL_String::load("Game/StartLevel",mpResources);
        std::string defaultplayersprite = CL_String::load("Game/DefaultPlayerSprite",mpResources );
        std::string itemdefinition = CL_String::load("Game/ItemDefinitions", mpResources );
        std::string statusEffectDefinition = CL_String::load("Game/StatusEffectDefinitions",mpResources);
        std::string spelldefinition = CL_String::load("Game/SpellDefinitions", mpResources);
        // Load special overlay for say.


        mpWindow  = new CL_DisplayWindow(name, WINDOW_WIDTH, WINDOW_HEIGHT);
                
        
        CL_Display::clear();

        mpSayOverlay = new CL_Surface("Overlays/say_overlay", mpResources );

        CL_Sprite * pPlayerSprite = new CL_Sprite(defaultplayersprite, mpResources );

	MappablePlayer *pPlayer = new StoneRing::MappablePlayer(0,0);

	pPlayer->setSprite(pPlayerSprite);



        loadFonts();
        showRechargeableOnionSplash();
        showIntro();
        loadStatusEffects(statusEffectDefinition);
        loadSpells(spelldefinition);
        loadItems(itemdefinition);
        Level * pLevel = new Level(startinglevel, mpResources);

	mMapState.setLevel ( pLevel );	
	mMapState.setPlayer(pPlayer);
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

        while(!mbDone)
        {
                        
        

	    draw();



#ifndef NDEBUG
            int fps = frameRate.get_fps();

                
#endif
            
#if 0
            if(delta_time < 16)
            {
                CL_System::keep_alive(16 - delta_time);
            }
            else
            {
                CL_System::keep_alive();
            }
#else
            CL_System::keep_alive();
#endif

            CL_Display::flip();


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


