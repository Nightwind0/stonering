#include "MapState.h"
#include "Level.h"

using std::min;


StoneRing::MapState::MapState():mbDone(false),mLevelX(0),
                                mLevelY(0),
                                mpPlayer(NULL)
{
#ifndef NDEBUG
    mbShowDebug = false;
#endif
}

StoneRing::MapState::~MapState()
{
}

void StoneRing::MapState::setDimensions(const CL_Rect &screenRect)
{
    mScreenRect = screenRect;
}

bool StoneRing::MapState::isDone() const
{
    return mbDone;
}

void StoneRing::MapState::handleKeyDown(const CL_InputEvent &key)
{
    if(CL_Keyboard::get_keycode(CL_KEY_SHIFT) && mpLevel->allowsRunning())
        mpPlayer->setRunning(true);
    
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
    
}

void StoneRing::MapState::handleKeyUp(const CL_InputEvent &key)
{
    switch(key.id)
    {
    case CL_KEY_SPACE:
        doTalk();
        break;
    case CL_KEY_TAB:
        doTalk(true); // Prod!
        break;
    case CL_KEY_SHIFT:
        mpPlayer->setRunning(false);
        break;
            
#ifndef NDEBUG
    case CL_KEY_P:
            
        break;
        
    case CL_KEY_D:
        mbShowDebug = mbShowDebug?false:true;

    case CL_KEY_M:
     
        mpLevel->dumpMappableObjects();
        break;
    case CL_KEY_S:
        gbDebugStop = true;
        break;

#endif
            
    }

}

void StoneRing::MapState::draw(const CL_Rect &screenRect,CL_GraphicContext * pGC)
{
	pGC->clear();
	uint width = min( (unsigned int)screenRect.get_width(), mpLevel->getWidth() * 32);
	uint height = min((unsigned int)screenRect.get_height(), mpLevel->getHeight() * 32);

    CL_Rect src = CL_Rect(mLevelX, mLevelY, mLevelX +width,
                          mLevelY + height);
	CL_Rect dst = screenRect;
	// Center
	if(screenRect.get_width() > src.get_width())
	{
		uint amount = (screenRect.get_width() - src.get_width()) /2;
		dst.left += amount;
		dst.right += amount;
	}

	if(screenRect.get_height() > src.get_height())
	{
		uint amount = (screenRect.get_height() - src.get_height()) /2;
		dst.top += amount;
		dst.bottom += amount;
	}

    mpLevel->draw(src,dst, pGC, false,mbShowDebug,mbShowDebug);
    mpLevel->drawMappableObjects( src,dst, pGC);
    mpLevel->drawFloaters(src,dst, pGC);
}


bool StoneRing::MapState::disableMappableObjects() const // Should the app move the MOs?
{
    return false;
}

bool StoneRing::MapState::drawMappableObjects() const // Should the app draw the MOs, including the player
{
    return true;
}

void StoneRing::MapState::mappableObjectMoveHook() // Do stuff right after the mappable object movemen
{
    recalculatePlayerPosition();
}

void StoneRing::MapState::start()
{
}

void StoneRing::MapState::finish() // Hook to clean up or whatever after being poppe
{
}


void StoneRing::MapState::pushLevel(Level * pLevel, uint x, uint y)
{
	CL_ResourceManager * pResources = IApplication::getInstance()->getResources();
    std::string defaultplayersprite = CL_String::load("Game/DefaultPlayerSprite",pResources );
	mLevels.push( pLevel );
	mpLevel = mLevels.top();



	MappablePlayer * pPlayer = new MappablePlayer(x,y);
	if(mpPlayer)
		pPlayer->matchFacingDirection(mpPlayer);
	mpPlayer = pPlayer;

	// gets deleted in the mappableobject destructor, which is called by Level, which deletes itself after a pop
	CL_Sprite * pPlayerSprite = new CL_Sprite(defaultplayersprite, pResources );
        
	setPlayerSprite(pPlayerSprite);
	mpPlayer->setSprite(mpPlayerSprite);

	mpLevel->addPlayer(mpPlayer);

	recalculatePlayerPosition();

	if(mpPlayer)switchFromPlayer(mpPlayer);
}

void StoneRing::MapState::setPlayerSprite(CL_Sprite * pPlayer)
{
	mpPlayerSprite = pPlayer;
}

void StoneRing::MapState::pop(bool bAll)
{
	MappableObject::eDirection oldDir = mpPlayer->getDirection();

	switchFromPlayer(mpPlayer);
	if(bAll)
	{
		while(mLevels.size() > 1)
		{
			mLevels.top()->markForDeath();
			mLevels.pop();
		}
	}
	else
	{

		if(mLevels.size()) 
		{
			mLevels.top()->markForDeath();
			mLevels.pop();
		}
	}

	mpLevel = mLevels.top();
	mpLevel->getPlayer()->matchFacingDirection(mpPlayer);
	mpPlayer = mpLevel->getPlayer();
	mpPlayer->setNextDirection( oldDir );
}



void StoneRing::MapState::recalculatePlayerPosition()
{
    int X = mpPlayer->getLevelX();
    int Y = mpPlayer->getLevelY();
    
    if( X  > mLevelX + (mScreenRect.get_width() / 2))
    {
        // Try to scroll right
        int amount = X - (mLevelX  + (mScreenRect.get_width()/2));
            
        if(mLevelX + amount + mScreenRect.get_width() < mpLevel->getWidth() * 32)
        {
            mLevelX += amount;
        }
        else
        {
            // Scroll as far over as possible
            mLevelX = (mpLevel->getWidth() * 32) - mScreenRect.get_width();
        }

    }
    if(X  <  mLevelX + (mScreenRect.get_width() / 2))
    {
        int amount = (mLevelX + (mScreenRect.get_width()/2)) - X;
        if(mLevelX - amount >0)
        {
            mLevelX-= amount;
        }
        else
        {
            mLevelX = 0;
        }
    }
    
    
    if(Y > mLevelY + (mScreenRect.get_height()/2))
    {
        int amount = Y - (mLevelY + (mScreenRect.get_height()/2));
            
        if(mLevelY + amount + mScreenRect.get_height() < mpLevel->getHeight() * 32)
        {
            mLevelY+= amount;
        }
        else
        {
            mLevelY = (mpLevel->getHeight() * 32) - mScreenRect.get_height();
        }
    }

    if(Y  <  mLevelY + (mScreenRect.get_height() / 2))
    {
        int amount = (mLevelY + (mScreenRect.get_height()/2)) - Y;
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

void StoneRing::MapState::moveMappableObjects()
{
    mpLevel->moveMappableObjects(CL_Rect(mLevelX, mLevelY, 
					 mLevelX + mScreenRect.get_width(),
					 mLevelY + mScreenRect.get_height()));
}

void StoneRing::MapState::doTalk(bool prod)
{
	CL_Point talkPoint = mpPlayer->getPointInFront();

    if(talkPoint.x >=0 && talkPoint.x < mpLevel->getWidth() && 
       talkPoint.y >=0 && talkPoint.y < mpLevel->getHeight())
        mpLevel->talk ( talkPoint, prod );
}

void StoneRing::MapState::switchFromPlayer(MappablePlayer * pPlayer)
{
	pPlayer->clearNextDirection();
}


