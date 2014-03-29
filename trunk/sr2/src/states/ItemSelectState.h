#ifndef SR2_ITEMSELECTSTATE_H
#define SR2_ITEMSELECTSTATE_H

#include <ClanLib/core.h>
#include <ClanLib/display.h>

#ifndef _WINDOWS_
#include "steel/SteelInterpreter.h"
#include "steel/SteelType.h"
#else
#include "SteelInterpreter.h"
#include "SteelType.h"
#endif
#include "IApplication.h"
#include "State.h"
#include "Menu.h"
#include <map>

namespace StoneRing
{

    class ItemSelectState : public State, public Menu
    {
    public:
        void Init(bool battle, int type_mask);
        virtual bool IsDone() const;
        virtual void Draw(const clan::Rect &screenRect,clan::Canvas& GC);
        virtual bool LastToDraw() const; // Should we continue drawing more states?
        virtual bool DisableMappableObjects() const; // Should the app move the MOs?
        virtual void Update(); // Do stuff right after the mappable object movement
        virtual void Start();
        virtual void Finish(); // Hook to clean up or whatever after being popped
        
        virtual clan::Rectf get_rect();
        virtual void draw_option(int option, bool selected, float x, float y, clan::Canvas& gc);
        virtual int height_for_option(clan::Canvas& gc);
        virtual void process_choice(int selection);
        virtual int get_option_count();
        virtual bool roll_over();
        void addItem(Item*,int count);
        Item * GetSelectedItem()const { return m_selected_item; }
	protected:
        virtual void HandleButtonUp( const IApplication::Button& button );
        virtual void HandleButtonDown( const IApplication::Button& button );
        virtual void HandleAxisMove( const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos );        
    private:
        void draw_categories();
        bool selection_applies(Item *pItem);
        enum eArrowState
        {
            ARROWS_IDLE,
            ARROW_LEFT_DOWN,
            ARROW_RIGHT_DOWN
        };
        eArrowState m_eArrowState;
        clan::Rectf m_rect;
        clan::Rectf m_header_rect;
        clan::Rectf m_text_rect;
        int m_typemask;
        bool m_battle;
        Item* m_selected_item;
        Item::eItemType m_itemType;
        std::map<Item::eItemType,clan::Image> m_type_icons;
        std::map<Item::eItemType, std::vector<std::pair<Item*,int> > > m_items;
        StoneRing::Font m_optionFont;
        StoneRing::Font m_currentOptionFont;	
        StoneRing::Font m_unavailableOption;
        StoneRing::Font m_descriptionFont;
        bool m_bDone;
    };


}


#endif




