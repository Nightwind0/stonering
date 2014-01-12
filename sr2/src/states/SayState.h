#ifndef SR2_SAY_STATE_H
#define SR2_SAY_STATE_H

#include "State.h"
#include "sr_defines.h"
#include "GraphicsManager.h"

namespace StoneRing
{

    class SayState : public State
    {
    public:
        SayState();
        virtual ~SayState();

        virtual bool IsDone() const;
		virtual void HandleButtonUp(const IApplication::Button& button);
		virtual void HandleButtonDown(const IApplication::Button& button);
		virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);
        virtual void HandleKeyDown(const clan::InputEvent &key);
        virtual void HandleKeyUp(const clan::InputEvent &key);
        virtual void Draw(const clan::Rect &screenRect,clan::Canvas& GC);
        virtual bool LastToDraw() const { return false; } // It'll be last anyway.... and if not, thats okay too
        virtual bool DisableMappableObjects() const; // Should the app move the MOs?
        virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void Start();
        virtual void Finish(); // Hook to clean up or whatever after being popped
        virtual void Init(const std::string &speaker, const std::string &text, int ms_per_page=-1,bool disable_mos=true);

    private:

        std::string m_speaker;
        std::string m_text;
        StoneRing::Font m_speakerFont;
        StoneRing::Font m_speechFont;
        clan::Rectf m_speaker_rect;
        clan::Rectf m_text_rect;
        clan::Rectf m_rect;
        std::string::iterator m_iText;
        bool m_bDone;
        uint m_nDrawnThisFrame;
        uint m_nTotalDrawn;
        int m_ms_per_page;
        uint m_page_start_time;
        bool m_bDisableMos;
    };
}


#endif




