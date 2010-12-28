#ifndef SR2_SAY_STATE_H
#define SR2_SAY_STATE_H

#include "State.h"
#include "sr_defines.h"

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
	virtual void HandleAxisMove(const IApplication::Axis& axis, float pos);
        virtual void HandleKeyDown(const CL_InputEvent &key);
        virtual void HandleKeyUp(const CL_InputEvent &key);
        virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
        virtual bool LastToDraw() const { return false; } // It'll be last anyway.... and if not, thats okay too
        virtual bool DisableMappableObjects() const; // Should the app move the MOs?
        virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void Start();
        virtual void Finish(); // Hook to clean up or whatever after being popped
        virtual void Init(const std::string &speaker, const std::string &text);

    private:

        std::string m_speaker;
        std::string m_text;
        CL_Font m_speakerFont;
        CL_Font m_speechFont;
	CL_Colorf m_speakerColor;
	CL_Colorf m_speechColor;
        CL_Rectf m_speaker_rect;
        CL_Rectf m_text_rect;
        CL_Color m_speaker_BGColor;
        CL_Color m_text_BGColor;
        CL_Image m_sayOverlay;
        uint m_X;
        uint m_Y;
        std::string::iterator m_iText;
        bool m_bDone;
        uint m_nDrawnThisFrame;
        uint m_nTotalDrawn;

    };
}


#endif




