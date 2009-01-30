#include "SayState.h"
#include "IApplication.h"
#include "GraphicsManager.h"

StoneRing::SayState::SayState()
:m_bDone(false),m_pSayOverlay(NULL)

{

}

StoneRing::SayState::~SayState()
{
    delete m_pSayOverlay;
}

bool StoneRing::SayState::IsDone() const
{
    return m_bDone;
}

void StoneRing::SayState::HandleKeyDown(const CL_InputEvent &key)
{
}

void StoneRing::SayState::HandleKeyUp(const CL_InputEvent &key)
{
    switch(key.id)
    {
    case CL_KEY_ENTER:
    case CL_KEY_SPACE:
        if(m_nDrawnThisFrame + m_nTotalDrawn == m_text.size())
        {
            m_bDone = true;
        }
        else
        {
            m_nTotalDrawn += m_nDrawnThisFrame;
            m_iText += m_nDrawnThisFrame;
            m_nDrawnThisFrame = 0;
            CL_System::sleep(100);
        }
        break;
    case CL_KEY_ESCAPE:
        m_bDone = true;
        break;
    }
}

void StoneRing::SayState::Draw(const CL_Rect &screenRect,CL_GraphicContext * pGC)
{
    CL_Rect speakerTextRect = m_speaker_rect;

    assert(m_pSpeakerFont && m_pSpeechFont);

    CL_Font * pSpeakerFont = m_pSpeakerFont;
    CL_Font * pTextFont = m_pSpeechFont;

    speakerTextRect.top += ( m_speaker_rect.get_height() - pSpeakerFont->get_height()) /2;
    speakerTextRect.bottom += ( m_speaker_rect.get_height() - pSpeakerFont->get_height()) /2;

    pGC->fill_rect( m_speaker_rect, m_speaker_BGColor );
    pGC->fill_rect( m_text_rect, m_text_BGColor ) ;

    m_pSayOverlay->draw(static_cast<float>(m_X),static_cast<float>(m_Y), pGC);
            

    if(m_iText != m_text.end())
        m_nDrawnThisFrame  =  pTextFont->draw(m_text_rect, m_iText, m_text.end(), pGC );

    pSpeakerFont->draw(speakerTextRect, m_speaker.begin(),m_speaker.end(), pGC );

    if(m_nTotalDrawn + m_nDrawnThisFrame < m_text.size())
    {
        // Draw a little "Theres more" doodad
        pGC->fill_rect( CL_Rect(screenRect.get_width() - 20, screenRect.get_height() - 20, screenRect.get_width() - 10, screenRect.get_height() - 10), CL_Color::black );
    }
}



bool StoneRing::SayState::DisableMappableObjects() const
{
    return true;
}


void StoneRing::SayState::MappableObjectMoveHook()
{
}

void StoneRing::SayState::Start()
{
    const std::string resource = "Overlays/Say/";
    IApplication * pApp = IApplication::GetInstance();
    CL_ResourceManager *pResources = pApp->GetResources();

    m_bDone = false;
    m_nTotalDrawn = m_nDrawnThisFrame = 0;

    m_pSpeakerFont = GraphicsManager::GetInstance()->GetFont(GraphicsManager::SAY,"Speaker");
    m_pSpeechFont = GraphicsManager::GetInstance()->GetFont(GraphicsManager::SAY,"Speech");

    if(!m_pSayOverlay)
        m_pSayOverlay = new CL_Surface(resource + "overlay", pResources);

    m_speaker_rect.top = CL_Integer(resource + "header/top", pResources);
    m_speaker_rect.left = CL_Integer(resource + "header/left", pResources);
    m_speaker_rect.right = CL_Integer(resource + "header/right", pResources);
    m_speaker_rect.bottom = CL_Integer(resource + "header/bottom", pResources);

    m_text_rect.top = CL_Integer(resource + "text/top", pResources);
    m_text_rect.left = CL_Integer(resource + "text/left", pResources);
    m_text_rect.right = CL_Integer(resource + "text/right", pResources);
    m_text_rect.bottom = CL_Integer(resource + "text/bottom", pResources);

    m_speaker_BGColor.set_red (CL_Integer(resource + "header/bgcolor/r", pResources));
    m_speaker_BGColor.set_green (CL_Integer(resource + "header/bgcolor/g", pResources));
    m_speaker_BGColor.set_blue (CL_Integer(resource + "header/bgcolor/b", pResources));
    m_speaker_BGColor.set_alpha (CL_Integer(resource + "header/bgcolor/a", pResources));

    m_text_BGColor.set_red (CL_Integer(resource + "text/bgcolor/r", pResources));
    m_text_BGColor.set_green (CL_Integer(resource + "text/bgcolor/g", pResources));
    m_text_BGColor.set_blue (CL_Integer(resource + "text/bgcolor/b", pResources));
    m_text_BGColor.set_alpha (CL_Integer(resource + "text/bgcolor/a", pResources));

    m_X = CL_Integer(resource + "x", pResources);
    m_Y = CL_Integer(resource + "y", pResources);
}

void StoneRing::SayState::Finish()
{
}


void StoneRing::SayState::Init(const std::string &speaker, const std::string &text)
{
    m_speaker = speaker;
    m_text = text;
    m_iText = m_text.begin();
}





