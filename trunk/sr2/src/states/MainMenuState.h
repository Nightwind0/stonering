#ifndef SR2_MAIN_MENU_STATE_H
#define SR2_MAIN_MENU_STATE_H

#include "State.h"
#include "Menu.h"
#include "MenuOption.h"
#include "sr_defines.h"
#include "GraphicsManager.h"
#include "MainMenuTargetingState.h"

using StoneRing::Font;

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
	virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);
	
        virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
        virtual bool LastToDraw() const { return false; } // It'll be last anyway.... and if not, thats okay too
        virtual bool DisableMappableObjects() const; // Should the app move the MOs?
        virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
        virtual void Start();
        virtual void RegisterSteelFunctions(SteelInterpreter*){}
        virtual void Finish(); // Hook to clean up or whatever after being popped
        virtual void SteelInit      (SteelInterpreter *);
        virtual void SteelCleanup   (SteelInterpreter *);	
        virtual void Init();

	void AddOption(MenuOption*);
	// Character selection stuff
	void SelectCharacterUp();
	void SelectCharacterDown();
	void SelectAllCharacters();
	void SelectionStart();
	void SelectionFinish();
	void SelectionCancel();
        
        void SelectNextOmega();
        void SelectPrevOmega();
	
    private:
	virtual CL_Rectf get_rect();
	virtual void draw_option(int option, bool selected, float x, float y, CL_GraphicContext& gc);
	virtual int height_for_option(CL_GraphicContext& gc);
	virtual void process_choice(int selection);
	virtual int get_option_count();
	virtual void draw_party(CL_GraphicContext& gc);
        virtual void draw_party_stats(CL_GraphicContext& gc);
        virtual void draw_omegas(CL_GraphicContext& gc);
        virtual bool hide_option(int option)const;
	
        void fill_choices(std::vector<MenuOption*>::const_iterator begin,
                          std::vector<MenuOption*>::const_iterator end);
        
	CL_Pointf calc_player_position(int player)const;
	
	SteelType selectTargets(bool group);
        SteelType selectOmegaSlot();
        SteelType reload();
        
	
	int m_nSelectedChar;
	bool m_bSelectAll;
	bool m_bSelectingTarget;

        std::string m_text;
	Font m_optionFont;
	Font m_selectionFont;
	Font m_ClassFont;
	Font m_HPFont;
	Font m_MPFont;
	Font m_LevelFont;
	Font m_SPFont;
	Font m_CharacterFont;
        Font m_partyStatFont;
        CL_Rectf m_menu_rect;
	CL_Rectf m_character_rect;
	CL_Rectf m_party_rect;
        CL_Rectf m_status_rect;
        CL_Rectf m_omega_rect;
	CL_Image m_portrait_shadow;
	CL_Sprite m_target_sprite;
        MenuOption* m_option_parent;
        std::vector<MenuOption*> m_root_choices;
	std::vector<MenuOption*> m_choices;
    
	MainMenuTargetingState m_targetingState;
        bool m_bDone;
    };
}


#endif




