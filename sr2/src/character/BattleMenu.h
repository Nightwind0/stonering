#ifndef SR_BATTLE_MENU_H
#define SR_BATTLE_MENU_H

#include <vector>
#include "Element.h"
#include "Menu.h"
#ifdef WINVER
#include "SteelInterpreter.h"
#else
#include "steel/SteelInterpreter.h"
#endif

#include "GraphicsManager.h"

namespace StoneRing
{
    class BattleMenuOption;
    class Character;

    class BattleMenu : public Element, public Menu
    {
    public:
        BattleMenu();
        virtual ~BattleMenu();
        virtual eElement WhichElement() const { return EBATTLEMENU; }

        enum eType{
            POPUP,
            SKILLS,
            SPELLS,
            ITEMS,
            CUSTOM // TODO: Way to populate this
        };

        std::vector<BattleMenuOption*>::iterator GetOptionsBegin();
        std::vector<BattleMenuOption*>::iterator GetOptionsEnd();
        
        BattleMenuOption* GetSelectedOption() const;

        eType GetType ( void ) const;
        void SetRect(CL_Rectf& rect);
        void SetEnableConditionParams(const ParameterList& params, Character* pChar);
        void Init();
    private:
        virtual CL_Rectf get_rect();
        virtual void draw_option(int option, bool selected,  float x, float y, CL_GraphicContext& gc);
        virtual int height_for_option(CL_GraphicContext& gc);
        virtual void process_choice(int selection){}
        virtual int get_option_count();
        void build_visible_list();
        
        virtual bool handle_element(eElement, Element *);
        virtual void load_attributes(CL_DomNamedNodeMap );
        virtual void load_finished();

        std::vector<BattleMenuOption*> m_options;
        std::vector<BattleMenuOption*> m_visible_options;
        eType m_eType;
        Font m_onFont;
        Font m_offFont;
        Font m_selectedFont;
        Font m_bpFont;
        Font m_mpFont;
        Character *m_pCharacter;
        CL_Rectf m_rect;
        CL_Pointf m_cost_spacing;
        int m_font_height;
        ParameterList m_params;
    };
};

#endif

