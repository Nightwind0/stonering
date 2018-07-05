#ifndef SR2_CHOICE_STATE_H
#define SR2_CHOICE_STATE_H

#include "State.h"
#include "Menu.h"
#include "sr_defines.h"
#include "GraphicsManager.h"

namespace StoneRing {

class ChoiceState : public State, public Menu {
public:
	ChoiceState();
	virtual ~ChoiceState();

	virtual bool IsDone() const;
	virtual void Draw( const clan::Rect &screenRect, clan::Canvas& GC );
	virtual bool LastToDraw() const {
		return false; // It'll be last anyway.... and if not, thats okay too
	}
	virtual bool DisableMappableObjects() const; // Should the app move the MOs?
	virtual void Update(); // Do stuff right after the mappable object movement
	virtual void Start();
	virtual void RegisterSteelFunctions( SteelInterpreter* ) {}
	virtual void Finish(); // Hook to clean up or whatever after being popped

	virtual void Init( const std::string &choiceText, const std::vector<std::string> &choices );
	virtual int GetSelection() const {
		return m_nSelection;
	}
protected:
	virtual void HandleKeyDown( const clan::InputEvent &key );
	virtual void HandleKeyUp( const clan::InputEvent &key );
	virtual void HandleButtonUp( const IApplication::Button& button );
	virtual void HandleButtonDown( const IApplication::Button& button );
	virtual void HandleAxisMove( const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos );	
private:
	virtual clan::Rectf get_rect();
	virtual void draw_option( int option, bool selected, const clan::Rectf& rect, clan::Canvas& gc );
	virtual int height_for_option( clan::Canvas& gc );
	virtual void process_choice( int selection );
	virtual int get_option_count();

	std::string m_text;
	clan::Rectf m_question_rect;
	clan::Rectf m_text_rect;
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




