#ifndef SR2_MAIN_MENU_STATE_H
#define SR2_MAIN_MENU_STATE_H

#include "State.h"
#include "Menu.h"
#include "MenuOption.h"
#include "sr_defines.h"

namespace StoneRing
{

    class MainMenuState : public State, public Menu
    {
    public:
        MainMenuState();
        virtual ~MainMenuState();

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

        virtual void Init();

	void AddOption(MenuOption*);
	
    private:
	virtual CL_Rectf get_rect();
	virtual void draw_option(int option, bool selected, float x, float y, CL_GraphicContext& gc);
	virtual int height_for_option(CL_GraphicContext& gc);
	virtual void process_choice(int selection);
	virtual int get_option_count();

        std::string m_text;
	CL_Image m_overlay;
	CL_Colorf m_optionColor;
	CL_Colorf m_selectionColor;
	CL_Font m_optionFont;
	CL_Font m_selectionFont;
        CL_Rectf m_menu_rect;
	CL_Rectf m_character_rect;
	CL_Rectf m_party_rect;
	std::vector<MenuOption*> m_choices;
        bool m_bDone;
    };
}


#endif




