#include "ChoiceState.h"
#include "IApplication.h"
#include "GraphicsManager.h"
#include "Level.h"


StoneRing::ChoiceState::ChoiceState():mbDone(false),mpChoiceOverlay(NULL),
                                      mpChoiceFont(NULL),mpOptionFont(NULL),
                                      mpCurrentOptionFont(NULL),mnSelection(-1)
{

}

StoneRing::ChoiceState::~ChoiceState()
{
}

bool StoneRing::ChoiceState::isDone() const
{
    return mbDone;
}

void StoneRing::ChoiceState::handleKeyDown(const CL_InputEvent &key)
{
}

void StoneRing::ChoiceState::handleKeyUp(const CL_InputEvent &key)
{
    switch(key.id)
    {
    case CL_KEY_ENTER:
    case CL_KEY_SPACE:
        // Select current option.
        mbDraw = false;
       // mpChoice->chooseOption(mnCurrentOption);
        mbDone = true;
        break;
    case CL_KEY_DOWN:
        if(mnCurrentOption + 1 < mChoices.size())
        {
            mnCurrentOption++;
                
            // @todo: 4?? should be options per page but we'll have to latch it
            if(mnCurrentOption > mnOptionOffset + 4)
            {
                mnOptionOffset++;
            }
        }
            
        break;
    case CL_KEY_UP:
        if(mnCurrentOption > 0 )
        {
            mnCurrentOption--;
        
            if(mnCurrentOption < mnOptionOffset)
            {
                mnOptionOffset--;
            }
        }
        break;
        
    case CL_KEY_ESCAPE:
        break;
    }
}

void StoneRing::ChoiceState::draw(const CL_Rect &screenRect,CL_GraphicContext * pGC)
{
    if(!mbDraw) return;

    pGC->fill_rect( mSpeakerRect, mSpeakerBGColor );
    pGC->fill_rect( mTextRect, mTextBGColor ) ;

    mpChoiceOverlay->draw(mX,mY,pGC);
    mpChoiceFont->draw(mSpeakerRect,mText,pGC);

    CL_Font * pLineFont = NULL;

    uint optionsPerPage = 0;

    // Draw the options
    for(uint i=0;i< optionsPerPage; i++)
    {
        // Don't paint more options than there are
        if(i + mnOptionOffset >= mChoices.size()) break;
    
        if(i + mnOptionOffset == mnCurrentOption)
        {
            pLineFont = mpCurrentOptionFont;
        }
        else
        {
            pLineFont = mpOptionFont;
        }
    
        pLineFont->draw( 0,  i * (pLineFont->get_height() + pLineFont->get_height_offset()) + (screenRect.get_height() /2),
                         mChoices[i + mnOptionOffset], pGC);
    
    }
    
}



bool StoneRing::ChoiceState::disableMappableObjects() const
{
    return true;
}


void StoneRing::ChoiceState::mappableObjectMoveHook()
{
}

void StoneRing::ChoiceState::start()
{

    mbDone = false;
    mbDraw = true;
    std::string resource = "Overlays/Choice/";
    IApplication *pApp = IApplication::getInstance();
    CL_ResourceManager *pResources = pApp->getResources();

    if(!mpChoiceFont)
        mpChoiceFont = GraphicsManager::getInstance()->getFont( GraphicsManager::FONT_CHOICE );
    
    if(!mpOptionFont)
        mpOptionFont = GraphicsManager::getInstance()->getFont( GraphicsManager::FONT_OPTION );
    
    if(!mpCurrentOptionFont)
        mpCurrentOptionFont = GraphicsManager::getInstance()->getFont( GraphicsManager::FONT_CURRENT_OPTION );

    if(!mpChoiceOverlay)
        mpChoiceOverlay = new CL_Surface("Overlays/Choice/overlay", pResources );

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

    mnCurrentOption = 0;
    mnOptionOffset = 0;

/*
  if(!mpChoiceOverlay)
  mpChoiceOverlay = new CL_Surface("Overlays/choice_overlay", IApplication::getInstance()->getResources() );
*/
}

void StoneRing::ChoiceState::finish()
{
}


void StoneRing::ChoiceState::init(const std::string &choiceText, const std::vector<std::string> &choices)
{
    mText = choiceText;
    mChoices = choices;
}



