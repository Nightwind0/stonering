#include "ChoiceState.h"
#include "IApplication.h"
#include "GraphicsManager.h"
#include "Level.h"

StoneRing::ChoiceState::ChoiceState():mbDone(false),mpChoiceOverlay(NULL),mpChoice(NULL),
				      mpChoiceFont(NULL),mpOptionFont(NULL),mpCurrentOptionFont(NULL)
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
	    mpChoice->chooseOption(mnCurrentOption);
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
    CL_Point choiceDrawPoint(screenRect.left, screenRect.top);
    CL_Size choiceDimensions = mpChoiceFont->get_size(mText,CL_Size( screenRect.get_width(), screenRect.get_height() / 2));

    if(choiceDimensions.width < screenRect.get_width())
	choiceDrawPoint.x += (screenRect.get_width() - choiceDimensions.width) /2;
    if(choiceDimensions.height < screenRect.get_height())
	choiceDrawPoint.y += (screenRect.get_height() - choiceDimensions.height) /2;

    pGC->fill_rect(screenRect,CL_Color(0,0,0,200));


    // Draw the Choice text
    mpChoiceFont->draw(choiceDrawPoint.x,choiceDrawPoint.y, mText, pGC);
    
    uint optionsPerPage = (screenRect.get_height() /2) / (mpOptionFont->get_height() + mpOptionFont->get_height_offset()) ;

    CL_Font * pLineFont = NULL;


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

	if(!mpChoiceFont)
	    mpChoiceFont = GraphicsManager::getInstance()->getFont( GraphicsManager::FONT_CHOICE );
	
	if(!mpOptionFont)
	    mpOptionFont = GraphicsManager::getInstance()->getFont( GraphicsManager::FONT_OPTION );
	
	if(!mpCurrentOptionFont)
	    mpCurrentOptionFont = GraphicsManager::getInstance()->getFont( GraphicsManager::FONT_CURRENT_OPTION );

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


void StoneRing::ChoiceState::init(const std::string &choiceText, const std::vector<std::string> &choices, Choice *pChoice)
{

    mText = choiceText;
    mChoices = choices;
    mpChoice = pChoice;

}

