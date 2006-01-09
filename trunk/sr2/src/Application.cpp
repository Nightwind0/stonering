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

}


void Application::say(const std::string &speaker, const std::string &text)
{
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
                           mLevelX(0),mLevelY(0),mbDone(false),
                           mbShowDebug(false), mbQueueKeyUps(false)
    
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


void Application::startKeyUpQueue()
{
    // Clear the queue.
    while(mKeyUpQueue.size())
        mKeyUpQueue.pop();

    mbQueueKeyUps = true;
}


void Application::stopKeyUpQueue()
{
    mbQueueKeyUps = false;
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


void Application::recalculatePlayerPosition()
{

	int X = mpPlayer->getLevelX();
    int Y = mpPlayer->getLevelY();

	if( X  > mLevelX + (WINDOW_WIDTH / 2))
	{
		// Try to scroll right
		int amount = X - (mLevelX  + (WINDOW_WIDTH/2));
            
		if(mLevelX + amount + WINDOW_WIDTH < mpLevel->getWidth() * 32)
		{
			mLevelX += amount;
		}
		else
		{
			// Scroll as far over as possible
			mLevelX = (mpLevel->getWidth() * 32) - WINDOW_WIDTH;
		}

	}
	if(X  <  mLevelX + (WINDOW_WIDTH / 2))
	{
		int amount = (mLevelX + (WINDOW_WIDTH/2)) - X;
		if(mLevelX - amount >0)
		{
			mLevelX-= amount;
		}
		else
		{
			mLevelX = 0;
		}
	}
    
    
	if(Y > mLevelY + (WINDOW_HEIGHT/2))
	{
		int amount = Y - (mLevelY + (WINDOW_HEIGHT/2));
            
		if(mLevelY + amount + WINDOW_HEIGHT < mpLevel->getHeight() * 32)
		{
			mLevelY+= amount;
		}
		else
		{
			mLevelY = (mpLevel->getHeight() * 32) - WINDOW_HEIGHT;
		}
	}

	if(Y  <  mLevelY + (WINDOW_HEIGHT / 2))
	{
		int amount = (mLevelY + (WINDOW_HEIGHT/2)) - Y;
		if(mLevelY - amount >0)
		{
			mLevelY-= amount;
		}
		else
		{
			mLevelY = 0;
		}
	}



	

}

void Application::onSignalKeyDown(const CL_InputEvent &key)
{


    switch(meState)
    {
    case MAIN:
    {
        bool running = false;
        if(CL_Keyboard::get_keycode( CL_KEY_SHIFT ) && mpLevel->allowsRunning())
        {
			mpPlayer->setRunning(true);
        }
        else
        {
            mpPlayer->setRunning(false);
        }
        
        switch(key.id)
        {
        case CL_KEY_ESCAPE:
            mbDone = true;
            break;
        case CL_KEY_DOWN:
			mpPlayer->setNextDirection(StoneRing::MappableObject::SOUTH);
            break;
        case CL_KEY_UP:
			mpPlayer->setNextDirection(StoneRing::MappableObject::NORTH);
            break;
        case CL_KEY_LEFT:
			mpPlayer->setNextDirection(StoneRing::MappableObject::WEST);
            break;
        case CL_KEY_RIGHT:
			mpPlayer->setNextDirection(StoneRing::MappableObject::EAST);
            break;

        default:
            break;
        }
        break;
    }
    case TALKING:
        break;
    } // switch(meState)
    
}

void Application::onSignalKeyUp(const CL_InputEvent &key)
{

    switch(meState)
    {
    case MAIN:
    {
        switch(key.id)
        {
        case CL_KEY_SPACE:
            doTalk();
            break;
        case CL_KEY_TAB:
            doTalk(true); // Prod!
            break;
            
#ifndef NDEBUG
        case CL_KEY_I:
            mItemManager.dumpItemList();
            break;
            
        case CL_KEY_P:
            
            break;
        
        case CL_KEY_D:
            mbShowDebug = mbShowDebug?false:true;

        case CL_KEY_M:
     
			mpLevel->dumpMappableObjects();
            break;
#endif
            
        }
    }
    default:
        // All other cases
        if(mbQueueKeyUps)
            mKeyUpQueue.push ( key.id );
    }// switch meState
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
    mpLevel->moveMappableObjects(getLevelRect());

	recalculatePlayerPosition();
    mbDraw = true;


#ifndef NDEBUG
  //  mpLevel->dumpMappableObjects();
#endif

}

void Application::drawMap()
{
    CL_Rect dst(0,0, min(WINDOW_WIDTH, (const int)mpLevel->getWidth()*32),min(WINDOW_HEIGHT, (const int)mpLevel->getHeight() * 32));
    
    CL_Rect src = getLevelRect();
    
    //      CL_Rect src = dst;
    mpWindow->get_gc()->push_cliprect( dst);
#ifdef NDEBUG
    mpLevel->draw(src,dst, mpWindow->get_gc(), false,false,false);
#else
	mpLevel->draw(src,dst,mpWindow->get_gc(), false,mbShowDebug,mbShowDebug);
#endif
    
   
    mpLevel->drawMappableObjects( src,dst, mpWindow->get_gc());
	mpLevel->drawFloaters(src,dst, mpWindow->get_gc());
    
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

		mpPlayer = new StoneRing::MappablePlayer(0,0);

		mpPlayer->setSprite(pPlayerSprite);

        loadFonts();
        showRechargeableOnionSplash();
        showIntro();
        loadStatusEffects(statusEffectDefinition);
        loadSpells(spelldefinition);
        loadItems(itemdefinition);
        mpLevel = new Level(startinglevel, mpResources);

		mpLevel->addPlayer(mpPlayer);


        CL_Slot slot_quit = mpWindow->sig_window_close().connect(this, &Application::onSignalQuit);

        CL_Slot slot_key_down = CL_Keyboard::sig_key_down().connect(this, &Application::onSignalKeyDown);

        CL_Slot slot_key_up  = CL_Keyboard::sig_key_up().connect(this, &Application::onSignalKeyUp);

        CL_Display::clear();
        
        static int start_time = CL_System::get_time();
        static long fpscounter = 0;

        meState = MAIN;

        mpMovementTimer = new CL_Timer(32);
        CL_Slot slot_mo_timer = mpMovementTimer->sig_timer().connect(this,&Application::onSignalMovementTimer);
        CL_FramerateCounter frameRate;
        mpMovementTimer->enable();

        while(!mbDone)
        {
                        
            processActionQueue();

            if(mbDraw)
            {
                drawMap();
                mbDraw = false;
            }




#ifndef NDEBUG
            int fps = frameRate.get_fps();

            if(mbShowDebug)
            {


                mpWindow->get_gc()->draw_rect( mLastTalkRect, CL_Color::aqua ) ;                

                mpfSBBlack->draw(0,0, mpLevel->getName());
                mpfSBBlack->draw(0,  mpfSBBlack->get_height(), std::string("FPS: ") +IntToString(fps) );

                
            }

                
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


void Application::processActionQueue()
{

}







void Application::doTalk(bool prod)
{
    CL_Point talkPoint = mpPlayer->getPosition();

    switch(mpPlayer->getDirection())
    {
    case MappableObject::NORTH:
        talkPoint.y--;
        break;
    case MappableObject::SOUTH:
        talkPoint.y++;
        break;
	case MappableObject::WEST:
        talkPoint.x--;
        break;
    case MappableObject::EAST:
        talkPoint.x++;
        break;
    }

    if(talkPoint.x >0 && talkPoint.x < mpLevel->getWidth() && 
       talkPoint.y >0 && talkPoint.y < mpLevel->getHeight())
        mpLevel->talk ( talkPoint, prod );

#if 0
    mLastTalkRect = talkRect;
    mLastTalkRect.left -= mLevelX;
    mLastTalkRect.right -= mLevelX;
    mLastTalkRect.top -= mLevelY;
    mLastTalkRect.bottom -= mLevelY;
#endif
}
