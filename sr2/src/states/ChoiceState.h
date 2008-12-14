#ifndef SR2_CHOICE_STATE_H
#define SR2_CHOICE_STATE_H

#include "State.h"
#include "sr_defines.h"

namespace StoneRing
{

    class ChoiceState : public State
    {
    public:
        ChoiceState();
        virtual ~ChoiceState();

        virtual bool IsDone() const;
        virtual void HandleKeyDown(const CL_InputEvent &key);
        virtual void HandleKeyUp(const CL_InputEvent &key);
        virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext * pGC);
        virtual bool LastToDraw() const { return false; } // It'll be last anyway.... and if not, thats okay too
        virtual bool DisableMappableObjects() const; // Should the app move the MOs? 
        virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void Start(); 
        virtual void RegisterSteelFunctions(SteelInterpreter*){}
        virtual void Finish(); // Hook to clean up or whatever after being popped

        virtual void Init(const std::string &choiceText, const std::vector<std::string> &choices);
        virtual int GetSelection() const { return m_nSelection; }

    private:

        std::string m_text;
        CL_Rect m_question_rect;
        CL_Rect m_text_rect;
        CL_Color m_question_BGColor;
        CL_Color m_text_BGColor;
        CL_Surface *m_pChoiceOverlay;
        uint m_X;
        uint m_Y;
        bool m_bDone;
        std::vector<std::string> m_choices;
        CL_Font *m_pChoiceFont;
        CL_Font *m_pOptionFont;
        CL_Font *m_pCurrentOptionFont;
        uint m_nCurrentOption;
        uint m_nOptionOffset;
        int m_nSelection;
        bool m_bDraw;
    };
};


#endif




