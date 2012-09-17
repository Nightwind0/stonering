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


#ifndef EQUIPSTATE_H
#define EQUIPSTATE_H
#include "State.h"
#include "Menu.h"
#include "StatusBox.h"

namespace StoneRing {
    
    class EquipmentCollector;
    
class EquipState : public State
{
public:
    EquipState();
    virtual ~EquipState();
    void Init(Character *pCharacter);
    virtual bool IsDone() const;
    // Handle joystick / key events that are processed according to mappings
    virtual void HandleButtonUp(const IApplication::Button& button);
    virtual void HandleButtonDown(const IApplication::Button& button);
    virtual void HandleAxisMove(const IApplication::Axis& axis, const IApplication::AxisDirection dir, float pos);
    
    virtual void Draw(const CL_Rect &screenRect,CL_GraphicContext& GC);
    virtual bool LastToDraw() const; // Should we continue drawing more states?
    virtual bool DisableMappableObjects() const; // Should the app move the MOs?
    virtual void MappableObjectMoveHook(); // Do stuff right after the mappable object movement
    virtual void Start();
    virtual void SteelInit      (SteelInterpreter *);
    virtual void SteelCleanup   (SteelInterpreter *);
    virtual void Finish(); // Hook to clean up or whatever after being popped  
private:    
    
    
    class EquipmentMenu : public Menu {
    public:
        EquipmentMenu();
        virtual ~EquipmentMenu();
        void SetRect(const CL_Rectf& rect);
        void SetFont(Font font);
        void SetSelectedFont(Font font);
        void SetRemoveIcon(const CL_Image& icon);
        void SetHeightPerOption(uint height);
        void EnableSelection();
        void DisableSelection();
        virtual CL_Rectf get_rect();
        virtual void draw_option(int option, bool selected, float x, float y, CL_GraphicContext& gc);
        virtual int height_for_option(CL_GraphicContext& gc);
        virtual void process_choice(int selection);
        virtual int get_option_count();
        
        Equipment* GetSelection() const;
        void ClearOptions();
        void AddOption(Equipment* pOption);
    private:
        CL_Image m_remove_icon;
        CL_Rectf m_rect;
        uint m_height_per_option;
        Font m_option_font;
        Font m_selected_font;
        uint m_selection;
        bool m_enable_selection;
        std::vector<Equipment*> m_options;        
    };
    
    
    class EquipmentCollector : public StoneRing::ItemVisitor
    {
    public:
        EquipmentCollector(EquipState::EquipmentMenu& menu, Character *pChar, Equipment::eSlot slot):m_menu(menu),m_char(pChar),m_slot(slot)
        {
        }
        ~EquipmentCollector(){}
        void operator()(Item* pItem, int nCount);
    private:
        Character * m_char;
        EquipState::EquipmentMenu& m_menu;
        Equipment::eSlot m_slot;
    };

    
    
    bool offhand_available() const;
    int options_per_column() const;
    void draw_slots(CL_GraphicContext& gc);
    void draw_stats(CL_GraphicContext& gc);
    void draw_description(CL_GraphicContext& gc);
    void slot_selected();
    void equipment_selected();
    void fill_equipment_menu();
    
    enum State {
        SELECT_SLOT,
        SELECT_EQUIPMENT
    }m_eState;
    
    CL_Rectf m_slots_rect;
    CL_Rectf m_equipment_rect;
    CL_Rectf m_stats_rect;
    CL_Rectf m_desc_rect;
    CL_Rectf m_stats_header;
    CL_Rectf m_stat_box;
    CL_Sizef m_slot_size;
    Font  m_slot_name_font;
    Font  m_slot_name_selected_font;
    Font  m_slot_font;
    Font  m_equipment_font;
    Font  m_equipment_selected_font;
    Font  m_stat_font;
    Font  m_stat_up_font;
    Font  m_stat_down_font;
    Font  m_stat_name_font;
    Font  m_stat_header_font;
    Font  m_desc_font;
    CL_Image m_no_equipment_icon;
    uint m_nSlot;
    Character* m_pChar;
    std::vector<Equipment::eSlot> m_slots;
    StatusBox* m_pStatusBox;
    EquipmentMenu m_equipment_menu;
    bool m_bDone;

    friend class EquipmentCollector;
};
}
#endif // EQUIPSTATE_H