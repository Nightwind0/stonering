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

        virtual bool isDone() const;
        virtual void handleKeyDown(const CL_InputEvent &key);
        virtual void handleKeyUp(const CL_InputEvent &key);
        virtual void draw(const CL_Rect &screenRect,CL_GraphicContext * pGC);
        virtual bool lastToDraw() const { return false; } // It'll be last anyway.... and if not, thats okay too
        virtual bool disableMappableObjects() const; // Should the app move the MOs? 
        virtual void mappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void start(); 
        virtual void finish(); // Hook to clean up or whatever after being popped

        virtual void init(const std::string &choiceText, const std::vector<std::string> &choices);
        virtual int getSelection() const { return mnSelection; }

    private:

        std::string mText;
        CL_Rect mQuestionRect;
        CL_Rect mTextRect;
        CL_Color mQuestionBGColor;
        CL_Color mTextBGColor;
        CL_Surface *mpChoiceOverlay;
        uint mX;
        uint mY;
        bool mbDone;
        std::vector<std::string> mChoices;
        CL_Font *mpChoiceFont;
        CL_Font *mpOptionFont;
        CL_Font *mpCurrentOptionFont;
        uint mnCurrentOption;
        uint mnOptionOffset;
        int mnSelection;
        bool mbDraw;

    };
};


#endif




