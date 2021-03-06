/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  <copyright holder> <email>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef SHOPSTATE_H
#define SHOPSTATE_H

#include "State.h"
#include "StatusBox.h"
#include "Menu.h"

namespace StoneRing {
class ShopState : public State
{
public:
    ShopState();
    virtual ~ShopState();

    void Init(const Steel::SteelArray& items);
    void Init(); // sell mode
    virtual bool IsDone() const;
 
    virtual void Draw(const clan::Rect &screenRect,clan::Canvas& GC);
    virtual bool LastToDraw() const; // Should we continue drawing more states?
    virtual bool DisableMappableObjects() const; // Should the app move the MOs?
    virtual void Update(); // Do stuff right after the mappable object movement
    virtual void Start();
    virtual void SteelInit      (SteelInterpreter *);
    virtual void SteelCleanup   (SteelInterpreter *);
    virtual void Finish(); // Hook to clean up or whatever after being popped  
protected:
    // Handle joystick / key events that are processed according to mappings
    virtual void HandleButtonUp(const IApplication::Button& button);
    virtual void HandleButtonDown(const IApplication::Button& button);
    virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);
   	
private:
    class ItemMenu : public Menu {
    public:
        ItemMenu();
        virtual ~ItemMenu();
        void SetRect(const clan::Rectf& rect);
        void SetFont(Font font);
        void SetSelectedFont(Font font);
        void SetUnavailableFont(Font font);
        void SetHeightPerOption(uint height);
        void SetPriceFont(Font font);
        void EnableSelection();
        void DisableSelection();
        virtual clan::Rectf get_rect();
        virtual void draw_option(int option, bool selected, const clan::Rectf& rect, clan::Canvas& gc);
        virtual int height_for_option(clan::Canvas& gc);
        virtual void process_choice(int selection);
        virtual int get_option_count();
        void SetSellMode(bool Sell);
        Item* GetSelection() const;
        void ClearOptions();
        void AddOption(Item* pOption, int count = -1);
    private:
        struct ItemEntry {
            Item* m_pItem;
            int   m_count;
        };
        clan::Rectf m_rect;
        uint m_height_per_option;
        Font m_option_font;
        Font m_selected_font;
        Font m_price_font;
        Font m_unavailable_font;
        uint m_selection;
        bool m_enable_selection;
        bool m_bSell;
        std::vector<ItemEntry> m_options;        
    };
    
    void draw_description(clan::Canvas& gc);
    void draw_characters(clan::Canvas& gc);
    void draw_status(clan::Canvas& gc);
    void draw_gold(clan::Canvas& gc);
    
    Font  m_item_font;
    Font  m_item_selected_font;
    Font  m_item_unavailable_font;
    Font  m_stat_font;
    Font  m_stat_up_font;
    Font  m_stat_down_font;
    Font  m_stat_name_font;
    Font  m_desc_font;
    Font  m_price_font;
    Font  m_gold_font;
    Font  m_stat_header_font;
    
    clan::Rectf m_character_rect;
    clan::Rectf m_items_rect;
    clan::Rectf m_stats_rect;
    clan::Rectf m_desc_rect;
    clan::Rectf m_gold_rect;
    clan::Rectf m_stat_header_rect;
    clan::Rectf m_stat_box;
    uint     m_current_character;
    std::vector<Item*> m_items;
    StatusBox* m_pStatusBox;
    ItemMenu m_item_menu;
    bool m_bSell;
    bool m_bDone;   
    friend class ItemCollector;
};
}

#endif // SHOPSTATE_H
