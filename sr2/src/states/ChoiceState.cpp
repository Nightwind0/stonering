#include "ChoiceState.h"
#include "IApplication.h"
#include "GraphicsManager.h"
#include "Level.h"

using std::min;
using std::max;

StoneRing::ChoiceState::ChoiceState():m_bDone(false),m_pChoiceOverlay(NULL),
                                      m_pChoiceFont(NULL),m_pOptionFont(NULL),
                                      m_pCurrentOptionFont(NULL),m_nSelection(-1)
{

}

StoneRing::ChoiceState::~ChoiceState()
{
}

bool StoneRing::ChoiceState::IsDone() const
{
    return m_bDone;
}

void StoneRing::ChoiceState::HandleKeyDown(const CL_InputEvent &key)
{
}

void StoneRing::ChoiceState::HandleKeyUp(const CL_InputEvent &key)
{
    switch(key.id)
    {
    case CL_KEY_ENTER:
    case CL_KEY_SPACE:
        // Select current option.
        m_bDraw = false;
       // mpChoice->chooseOption(mnCurrentOption);
        m_nSelection = m_nCurrentOption;
        m_bDone = true;
        break;
    case CL_KEY_DOWN:
        if(m_nCurrentOption + 1 < m_choices.size())
        {
            m_nCurrentOption++;
                
            // @todo: 4?? should be options per page but we'll have to latch it
            if(m_nCurrentOption > m_nOptionOffset + 4)
            {
                m_nOptionOffset++;
            }
        }
            
        break;
    case CL_KEY_UP:
        if(m_nCurrentOption > 0 )
        {
            m_nCurrentOption--;
        
            if(m_nCurrentOption < m_nOptionOffset)
            {
                m_nOptionOffset--;
            }
        }
        break;
        
    case CL_KEY_ESCAPE:
        break;
    }
}

void StoneRing::ChoiceState::Draw(const CL_Rect &screenRect,CL_GraphicContext * pGC)
{
    if(!m_bDraw) return;

    pGC->fill_rect( m_question_rect, m_question_BGColor );
    pGC->fill_rect( m_text_rect, m_text_BGColor ) ;

    m_pChoiceOverlay->draw(static_cast<float>(m_X),static_cast<float>(m_Y),pGC);
    m_pChoiceFont->draw(m_question_rect,m_text,pGC);

    CL_Font * pLineFont = NULL;

    uint optionsPerPage = max(1, m_text_rect.get_height() / m_pOptionFont->get_height());

    optionsPerPage = min(optionsPerPage,static_cast<uint>(m_choices.size()));

    // Draw the options
    for(uint i=0;i< optionsPerPage; i++)
    {
        int indent = 0;
        // Don't paint more options than there are
        if(i + m_nOptionOffset >= m_choices.size()) break;
    
        if(i + m_nOptionOffset == m_nCurrentOption)
        {
            indent = 10;
            pLineFont = m_pCurrentOptionFont;
        }
        else
        {
            pLineFont = m_pOptionFont;
        }
    
        pLineFont->draw( m_text_rect.left + indent,  i * (pLineFont->get_height() + pLineFont->get_height_offset()) + m_text_rect.top,
                         m_choices[i + m_nOptionOffset], pGC);
    
    }
    
}



bool StoneRing::ChoiceState::DisableMappableObjects() const
{
    return true;
}


void StoneRing::ChoiceState::MappableObjectMoveHook()
{
}

void StoneRing::ChoiceState::Start()
{

    m_bDone = false;
    m_bDraw = true;
    std::string resource = "Overlays/Choice/";
    IApplication *pApp = IApplication::GetInstance();
    CL_ResourceManager *pResources = pApp->GetResources();
    std::string choiceFont = CL_String::load(resource + "fonts/Choice",pResources);
    std::string optionFont = CL_String::load(resource + "fonts/Option",pResources);
    std::string selectionFont = CL_String::load(resource+ "fonts/Selection",pResources);

    if(!m_pChoiceFont)
        m_pChoiceFont = GraphicsManager::GetInstance()->GetFont(choiceFont);
    
    if(!m_pOptionFont)
        m_pOptionFont = GraphicsManager::GetInstance()->GetFont(optionFont);
    
    if(!m_pCurrentOptionFont)
        m_pCurrentOptionFont = GraphicsManager::GetInstance()->GetFont(selectionFont);

    if(!m_pChoiceOverlay)
        m_pChoiceOverlay = new CL_Surface("Overlays/Choice/overlay", pResources );

    m_question_rect.top = CL_Integer(resource + "header/top", pResources);
    m_question_rect.left = CL_Integer(resource + "header/left", pResources);
    m_question_rect.right = CL_Integer(resource + "header/right", pResources);
    m_question_rect.bottom = CL_Integer(resource + "header/bottom", pResources);

    m_text_rect.top = CL_Integer(resource + "text/top", pResources);
    m_text_rect.left = CL_Integer(resource + "text/left", pResources);
    m_text_rect.right = CL_Integer(resource + "text/right", pResources);
    m_text_rect.bottom = CL_Integer(resource + "text/bottom", pResources);

    m_question_BGColor.set_red (CL_Integer(resource + "header/bgcolor/r", pResources));
    m_question_BGColor.set_green (CL_Integer(resource + "header/bgcolor/g", pResources));
    m_question_BGColor.set_blue (CL_Integer(resource + "header/bgcolor/b", pResources));
    m_question_BGColor.set_alpha (CL_Integer(resource + "header/bgcolor/a", pResources));

    m_text_BGColor.set_red (CL_Integer(resource + "text/bgcolor/r", pResources));
    m_text_BGColor.set_green (CL_Integer(resource + "text/bgcolor/g", pResources));
    m_text_BGColor.set_blue (CL_Integer(resource + "text/bgcolor/b", pResources));
    m_text_BGColor.set_alpha (CL_Integer(resource + "text/bgcolor/a", pResources));

    m_X = CL_Integer(resource + "x", pResources);
    m_Y = CL_Integer(resource + "y", pResources);

    m_nCurrentOption = 0;
    m_nOptionOffset = 0;

/*
  if(!mpChoiceOverlay)
  mpChoiceOverlay = new CL_Surface("Overlays/choice_overlay", IApplication::getInstance()->getResources() );
*/
}

void StoneRing::ChoiceState::Finish()
{
}


void StoneRing::ChoiceState::Init(const std::string &choiceText, const std::vector<std::string> &choices)
{
    m_text = choiceText;
    m_choices = choices;
}





