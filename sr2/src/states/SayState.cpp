#include "SayState.h"
#include "IApplication.h"
#include "GraphicsManager.h"

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
		m_nTotalDrawn += m_nDrawnThisFrame;
		m_nDrawnThisFrame = 0;
		CL_System::sleep(100);
	    }
        break;
	case IApplication::BUTTON_CANCEL:
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

void StoneRing::SayState::HandleKeyDown(const CL_InputEvent &key)
{
}

void StoneRing::SayState::HandleKeyUp(const CL_InputEvent &key)
{

}

void StoneRing::SayState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC)
{
    CL_Rect speakerTextRect = m_speaker_rect;

    CL_Font speakerFont = m_speakerFont;
    CL_Font textFont = m_speechFont;

    speakerTextRect.top += ( m_speaker_rect.get_height() - speakerFont.get_font_metrics(GC).get_height()) /2;
    speakerTextRect.bottom += ( m_speaker_rect.get_height() - speakerFont.get_font_metrics(GC).get_height()) /2;

    CL_Draw::fill(GC,m_speaker_rect, CL_Colorf(m_speaker_BGColor) );
    CL_Draw::fill(GC, m_text_rect, CL_Colorf(m_text_BGColor)) ;

    m_sayOverlay.draw(GC,static_cast<float>(m_X),static_cast<float>(m_Y));
    speakerFont.draw_text(GC,(float)speakerTextRect.left, speakerTextRect.top + speakerFont.get_font_metrics(GC).get_height(),m_speaker, m_speakerColor);


    m_nDrawnThisFrame = draw_text(GC,textFont,m_speechColor,m_text_rect,m_text,m_nTotalDrawn);//textFont.draw_text(GC,m_text_rect.get_top_left(.x,m_text_rect.get_top_left().y, m_iText, m_text.end());
    if(m_nTotalDrawn + m_nDrawnThisFrame < m_text.size())
    {
        // Draw a little "Theres more" doodad
        CL_Draw::fill(GC,CL_Rectf(screenRect.get_width() - 20, screenRect.get_height() - 20, screenRect.get_width() - 10, screenRect.get_height() - 10), CL_Colorf::black );
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
    CL_ResourceManager& resources = pApp->GetResources();

    m_bDone = false;
    m_nTotalDrawn = m_nDrawnThisFrame = 0;

    m_speakerFont = GraphicsManager::GetInstance()->GetFont(GraphicsManager::GetInstance()->GetFontName(GraphicsManager::SAY,"Speaker"));
    m_speechFont = GraphicsManager::GetInstance()->GetFont(GraphicsManager::GetInstance()->GetFontName(GraphicsManager::SAY,"Speech"));
    m_speakerColor = GraphicsManager::GetInstance()->GetFontColor(GraphicsManager::GetInstance()->GetFontName(GraphicsManager::SAY,"Speaker"));
    m_speechColor = GraphicsManager::GetInstance()->GetFontColor(GraphicsManager::GetInstance()->GetFontName(GraphicsManager::SAY,"Speech"));

    m_sayOverlay = CL_Image(GET_MAIN_GC(),resource + "overlay",&resources);

    m_speaker_rect.top = (float)resources.get_integer_resource(resource + "header/top", 0);
    m_speaker_rect.left = (float)resources.get_integer_resource(resource + "header/left", 0);
    m_speaker_rect.right = (float)resources.get_integer_resource(resource + "header/right", 0);
    m_speaker_rect.bottom = (float)resources.get_integer_resource(resource + "header/bottom", 0);

    m_text_rect.top = (float)resources.get_integer_resource(resource + "text/top", 0);
    m_text_rect.left = (float)resources.get_integer_resource(resource + "text/left", 0);
    m_text_rect.right = (float)resources.get_integer_resource(resource + "text/right", 0);
    m_text_rect.bottom = (float)resources.get_integer_resource(resource + "text/bottom", 0);

    m_speaker_BGColor.set_red (resources.get_integer_resource(resource + "header/bgcolor/r", 0));
    m_speaker_BGColor.set_green (resources.get_integer_resource(resource + "header/bgcolor/g", 0));
    m_speaker_BGColor.set_blue (resources.get_integer_resource(resource + "header/bgcolor/b", 0));
    m_speaker_BGColor.set_alpha (resources.get_integer_resource(resource + "header/bgcolor/a", 0));

    m_text_BGColor.set_red (resources.get_integer_resource(resource + "text/bgcolor/r", 0));
    m_text_BGColor.set_green (resources.get_integer_resource(resource + "text/bgcolor/g", 0));
    m_text_BGColor.set_blue (resources.get_integer_resource(resource + "text/bgcolor/b", 0));
    m_text_BGColor.set_alpha (resources.get_integer_resource(resource + "text/bgcolor/a", 0));

    m_X = resources.get_integer_resource(resource + "x", 0);
    m_Y = resources.get_integer_resource(resource + "y", 0);
}

void StoneRing::SayState::Finish()
{
}


void StoneRing::SayState::Init(const std::string &speaker, const std::string &text)
{
    m_speaker = speaker;
    m_text = text;
    m_nTotalDrawn = 0;
    m_iText = m_text.begin();
}





