#ifndef SR2_CHOICE_STATE_H
#define SR2_CHOICE_STATE_H

#include "State.h"
#include "Menu.h"
#include "sr_defines.h"
#include "GraphicsManager.h"

namespace StoneRing
{

    class ChoiceState : public State, public Menu
    {
    public:
        ChoiceState();
        virtual ~ChoiceState();

        virtual bool IsDone() const;
        virtual void HandleKeyDown(const CL_InputEvent &key);
        virtual void HandleKeyUp(const CL_InputEvent &key);
	virtual void HandleButtonUp(const IApplication::Button& button);
	virtual void HandleButtonDown(const IApplication::Button& button);
	virtual void HandleAxisMove(const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos);
	
        virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
        virtual bool LastToDraw() const { return false; } // It'll be last anyway.... and if not, thats okay too
        virtual bool DisableMappableObjects() const; // Should the app move the MOs?
        virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void Start();
        virtual void RegisterSteelFunctions(SteelInterpreter*){}
        virtual void Finish(); // Hook to clean up or whatever after being popped

        virtual void Init(const std::string &choiceText, const std::vector<std::string> &choices);
        virtual int GetSelection() const { return m_nSelection; }

    private:
	virtual CL_Rectf get_rect();
	virtual void draw_option(int option, bool selected, float x, float y, CL_GraphicContext& gc);
	virtual int height_for_option(CL_GraphicContext& gc);
	virtual void process_choice(int selection);
	virtual int get_option_count();

        std::string m_text;
        CL_Rectf m_question_rect;
        CL_Rectf m_text_rect;
        CL_Image m_choiceOverlay;
        uint m_X;
        uint m_Y;
        bool m_bDone;
        std::vector<std::string> m_choices;
        StoneRing::Font m_choiceFont;
        StoneRing::Font m_optionFont;
        StoneRing::Font m_currentOptionFont;

        int m_nSelection;
        bool m_bDraw;
    };
}


#endif




