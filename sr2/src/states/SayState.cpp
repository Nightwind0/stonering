#include "SayState.h"
#include "IApplication.h"
#include "GraphicsManager.h"

StoneRing::SayState::SayState():mSpeakerRect(16,315,783,369), 
                                mTextRect(16,388,783,580), mbDone(false),mpSayOverlay(NULL)

{

}

StoneRing::SayState::~SayState()
{
    delete mpSayOverlay;
}

bool StoneRing::SayState::isDone() const
{
    return mbDone;
}

void StoneRing::SayState::handleKeyDown(const CL_InputEvent &key)
{
}

void StoneRing::SayState::handleKeyUp(const CL_InputEvent &key)
{
    switch(key.id)
    {
    case CL_KEY_ENTER:
    case CL_KEY_SPACE:
        if(mnDrawnThisFrame + mnTotalDrawn == mText.size())
        {
            mbDone = true;
        }
        else
        {
            mnTotalDrawn += mnDrawnThisFrame;
            miText += mnDrawnThisFrame;
            mnDrawnThisFrame = 0;
            CL_System::sleep(100);
        }
        break;
    case CL_KEY_ESCAPE:
        mbDone = true;
        break;
    }
}

void StoneRing::SayState::draw(const CL_Rect &screenRect,CL_GraphicContext * pGC)
{
    CL_Rect speakerTextRect = mSpeakerRect;
    CL_Font * pSpeakerFont = GraphicsManager::getInstance()->getFont( GraphicsManager::FONT_SPEAKER );
    CL_Font * pTextFont = GraphicsManager::getInstance()->getFont( GraphicsManager::FONT_SAY_TEXT );

    speakerTextRect.top += ( mSpeakerRect.get_height() - pSpeakerFont->get_height()) /2;
    speakerTextRect.bottom += ( mSpeakerRect.get_height() - pSpeakerFont->get_height()) /2;

    pGC->fill_rect( mSpeakerRect, CL_Color(255,255,255,200) );
    pGC->fill_rect( mTextRect, CL_Color(0,0,0,128) ) ;

    mpSayOverlay->draw(0,300, pGC);
            

    if(miText != mText.end())
        mnDrawnThisFrame  =  pTextFont->draw(mTextRect, miText, mText.end(), pGC );

    pSpeakerFont->draw(speakerTextRect, mSpeaker.begin(),mSpeaker.end(), pGC );


    if(mnTotalDrawn + mnDrawnThisFrame < mText.size())
    {
        // Draw a little "Theres more" doodad
        pGC->fill_rect( CL_Rect(screenRect.get_width() - 20, screenRect.get_height() - 20, screenRect.get_width() - 10, screenRect.get_height() - 10), CL_Color::black );
    }




}



bool StoneRing::SayState::disableMappableObjects() const
{
    return true;
}


void StoneRing::SayState::mappableObjectMoveHook()
{
}

void StoneRing::SayState::start()
{

    mbDone = false;
    mnTotalDrawn = mnDrawnThisFrame = 0;

    if(!mpSayOverlay)
        mpSayOverlay = new CL_Surface("Overlays/say_overlay", IApplication::getInstance()->getResources() );
}

void StoneRing::SayState::finish()
{
}


void StoneRing::SayState::init(const std::string &speaker, const std::string &text)
{
    mSpeaker = speaker;
    mText = text;
    miText = mText.begin();
}



