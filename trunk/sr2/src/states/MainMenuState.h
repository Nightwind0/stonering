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
    virtual void HandleKeyDown ( const clan::InputEvent &key );
    virtual void HandleKeyUp ( const clan::InputEvent &key );


    virtual void Draw ( const clan::Rect &screenRect,clan::Canvas& GC );
    virtual bool LastToDraw() const
    {
        return false;    // It'll be last anyway.... and if not, thats okay too
    }
    virtual bool DisableMappableObjects() const; // Should the app move the MOs?
    virtual void Update(); // Do stuff right after the mappable object movement
    virtual void Start();
    virtual void RegisterSteelFunctions ( SteelInterpreter* ) {}
    virtual void Finish(); // Hook to clean up or whatever after being popped
    virtual void SteelInit ( SteelInterpreter * );
    virtual void SteelCleanup ( SteelInterpreter * );
    virtual void Init();

    void AddOption ( MenuOption* );
    // Character selection stuff
    void SelectCharacterUp();
    void SelectCharacterDown();
    void SelectAllCharacters();
    void SelectionStart();
    void SelectionFinish();
    void SelectionCancel();

    void SelectNextOmega();
    void SelectPrevOmega();
protected:
    virtual void HandleButtonUp ( const IApplication::Button& button );
    virtual void HandleButtonDown ( const IApplication::Button& button );
    virtual void HandleAxisMove ( const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos );

private:
    virtual clan::Rectf get_rect();
    virtual void draw_option ( int option, bool selected, const clan::Rectf& rect, clan::Canvas& gc );
    virtual int height_for_option ( clan::Canvas& gc );
    virtual void process_choice ( int selection );
    virtual int get_option_count();
    virtual void draw_party ( clan::Canvas& gc );
    virtual void draw_party_stats ( clan::Canvas& gc );
    virtual void draw_omegas ( clan::Canvas& gc );
    virtual bool hide_option ( int option ) const;

    void fill_choices ( std::vector<MenuOption*>::const_iterator begin,
                        std::vector<MenuOption*>::const_iterator end );

    clan::Pointf calc_player_position ( int player ) const;

    SteelType selectTargets ( bool group );
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
    clan::Rectf m_menu_rect;
    clan::Rectf m_character_rect;
    clan::Rectf m_party_rect;
    clan::Rectf m_status_rect;
    clan::Rectf m_omega_rect;
    clan::Image m_portrait_shadow;
    clan::Sprite m_target_sprite;
    MenuOption* m_option_parent;
    std::vector<MenuOption*> m_root_choices;
    std::vector<MenuOption*> m_choices;

    MainMenuTargetingState m_targetingState;
    bool m_bDone;
};
}


#endif




