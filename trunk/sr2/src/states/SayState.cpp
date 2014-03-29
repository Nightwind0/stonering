#include "SayState.h"
#include "IApplication.h"
#include "GraphicsManager.h"
#include "MenuBox.h"
#include "SoundManager.h"

StoneRing::SayState::SayState()
:m_bDone(false)

{

}

StoneRing::SayState::~SayState()
{
}

bool StoneRing::SayState::IsDone() const
{
    return m_bDone;
}


void StoneRing::SayState::HandleButtonUp(const IApplication::Button& button)
{
    switch(button)
    {
	case IApplication::BUTTON_CONFIRM:
	    if(m_nDrawnThisFrame + m_nTotalDrawn >= m_text.size())
	    {
		m_bDone = true;
	    }
	    else
	    {
                if(m_ms_per_page == -1)
                    SoundManager::PlayEffect(SoundManager::EFFECT_CHANGE_OPTION);
		m_nTotalDrawn += m_nDrawnThisFrame;
		m_nDrawnThisFrame = 0;
		clan::System::sleep(100);
	    }
        break;
	case IApplication::BUTTON_CANCEL:
            SoundManager::PlayEffect(SoundManager::EFFECT_CANCEL);
	    m_bDone = true;
	    break;
    }
}

void StoneRing::SayState::HandleButtonDown(const IApplication::Button& button)
{
}

void StoneRing::SayState::HandleAxisMove(const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos)
{
}

void StoneRing::SayState::HandleKeyDown(const clan::InputEvent &key)
{
}

void StoneRing::SayState::HandleKeyUp(const clan::InputEvent &key)
{

}

void StoneRing::SayState::Draw(const clan::Rect &screenRect,clan::Canvas& GC)
{
    clan::Rect speakerTextRect = m_speaker_rect;

    Font speakerFont = m_speakerFont;
    Font textFont = m_speechFont;

    speakerTextRect.top += ( m_speaker_rect.get_height() - speakerFont.get_font_metrics(GC).get_height()) /2;
    speakerTextRect.bottom += ( m_speaker_rect.get_height() - speakerFont.get_font_metrics(GC).get_height()) /2;

    MenuBox::Draw(GC,m_rect);
   
    speakerFont.draw_text(GC,(float)speakerTextRect.left, speakerTextRect.top + speakerFont.get_font_metrics(GC).get_height(),m_speaker);


    m_nDrawnThisFrame = draw_text(GC,textFont,m_text_rect,m_text,m_nTotalDrawn);//textFont.draw_text(GC,m_text_rect.get_top_left(.x,m_text_rect.get_top_left().y, m_iText, m_text.end());
    if(m_nTotalDrawn + m_nDrawnThisFrame < m_text.size())
    {
        // Draw a little "Theres more" doodad
        GC.fill_rect(clan::Rectf(screenRect.get_width() - 20, screenRect.get_height() - 20, screenRect.get_width() - 10, screenRect.get_height() - 10), clan::Colorf::black );
    }
    
    if(m_ms_per_page != -1){
        if(clan::System::get_time() >= m_page_start_time + m_ms_per_page){
            m_page_start_time = clan::System::get_time();
            HandleButtonUp(IApplication::BUTTON_CONFIRM);
        }
    }
    

}



bool StoneRing::SayState::DisableMappableObjects() const
{
    return m_bDisableMos;
}


void StoneRing::SayState::Update()
{
}

void StoneRing::SayState::Start()
{
    m_bDone = false;
    m_nTotalDrawn = m_nDrawnThisFrame = 0;

    m_speakerFont = GraphicsManager::GetFont(GraphicsManager::GetFontName(GraphicsManager::SAY,"Speaker"));
    m_speechFont = GraphicsManager::GetFont(GraphicsManager::GetFontName(GraphicsManager::SAY,"Speech"));

 
    m_speaker_rect = GraphicsManager::GetRect(GraphicsManager::SAY,"header");
    m_text_rect = GraphicsManager::GetRect(GraphicsManager::SAY,"text");
    m_rect =      GraphicsManager::GetRect(GraphicsManager::SAY,"rect");
    //SoundManager::PlayEffect(SoundManager::EFFECT_SELECT_OPTION);
    m_page_start_time = clan::System::get_time();
}

void StoneRing::SayState::Finish()
{
}


void StoneRing::SayState::Init(const std::string &speaker, const std::string &text, int ms_per_page, bool disableMOs)
{
    m_speaker = speaker;
    m_text = text;
    m_nTotalDrawn = 0;
    m_iText = m_text.begin();
    m_ms_per_page = ms_per_page;
    m_bDisableMos = disableMOs;
}





