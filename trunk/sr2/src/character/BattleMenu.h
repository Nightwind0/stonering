#ifndef SR_BATTLE_MENU_H
#define SR_BATTLE_MENU_H

#include <vector>
#include "Element.h"
#include "Menu.h"
#include "sr_defines.h"
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
        void SetRect(clan::Rectf& rect);
        void SetEnableConditionParams(const Steel::ParameterList& params, Character* pChar);
		// Must call SetEnableConditionParams before calling this
		bool HasEnabledOptions() const;
        void Init();
		virtual std::string GetDebugId() const { return ""; }				
    private:
        virtual clan::Rectf get_rect();
        virtual void draw_option(int option, bool selected,  float x, float y, clan::Canvas& gc);
        virtual int height_for_option(clan::Canvas& gc);
        virtual void process_choice(int selection){}
        virtual int get_option_count();
        virtual bool hide_option(int selection)const;
        
        virtual bool handle_element(eElement, Element *);
        virtual void load_attributes(clan::DomNamedNodeMap );
        virtual void load_finished();

        std::vector<BattleMenuOption*> m_options;
        eType m_eType;
        Font m_onFont;
        Font m_offFont;
        Font m_selectedFont;
        Font m_bpFont;
        Font m_mpFont;
        Character *m_pCharacter;
        clan::Rectf m_rect;
        clan::Pointf m_cost_spacing;
        int m_font_height;
        Steel::ParameterList m_params;
    };
};

#endif

