#include "SayState.h"
#include "IApplication.h"
#include "GraphicsManager.h"

StoneRing::SayState::SayState()
:mbDone(false),mpSayOverlay(NULL)

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

    pGC->fill_rect( mSpeakerRect, mSpeakerBGColor );
    pGC->fill_rect( mTextRect, mTextBGColor ) ;

    mpSayOverlay->draw(static_cast<float>(mX),static_cast<float>(mY), pGC);
            

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
    const std::string resource = "Overlays/Say/";
    IApplication * pApp = IApplication::getInstance();
    CL_ResourceManager *pResources = pApp->getResources();

    mbDone = false;
    mnTotalDrawn = mnDrawnThisFrame = 0;

    if(!mpSayOverlay)
        mpSayOverlay = new CL_Surface(resource + "overlay", pResources);

    mSpeakerRect.top = CL_Integer(resource + "header/top", pResources);
    mSpeakerRect.left = CL_Integer(resource + "header/left", pResources);
    mSpeakerRect.right = CL_Integer(resource + "header/right", pResources);
    mSpeakerRect.bottom = CL_Integer(resource + "header/bottom", pResources);

    mTextRect.top = CL_Integer(resource + "text/top", pResources);
    mTextRect.left = CL_Integer(resource + "text/left", pResources);
    mTextRect.right = CL_Integer(resource + "text/right", pResources);
    mTextRect.bottom = CL_Integer(resource + "text/bottom", pResources);

    mSpeakerBGColor.set_red (CL_Integer(resource + "header/bgcolor/r", pResources));
    mSpeakerBGColor.set_green (CL_Integer(resource + "header/bgcolor/g", pResources));
    mSpeakerBGColor.set_blue (CL_Integer(resource + "header/bgcolor/b", pResources));
    mSpeakerBGColor.set_alpha (CL_Integer(resource + "header/bgcolor/a", pResources));

    mTextBGColor.set_red (CL_Integer(resource + "text/bgcolor/r", pResources));
    mTextBGColor.set_green (CL_Integer(resource + "text/bgcolor/g", pResources));
    mTextBGColor.set_blue (CL_Integer(resource + "text/bgcolor/b", pResources));
    mTextBGColor.set_alpha (CL_Integer(resource + "text/bgcolor/a", pResources));

    mX = CL_Integer(resource + "x", pResources);
    mY = CL_Integer(resource + "y", pResources);
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





