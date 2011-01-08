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
namespace StoneRing
{
    class BattleMenuOption;

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
	void SetEnableConditionParams(const ParameterList& params);
	void Init();
    private:
	virtual CL_Rectf get_rect();
	virtual void draw_option(int option, bool selected,  float x, float y, CL_GraphicContext& gc);
	virtual int height_for_option(CL_GraphicContext& gc);
	virtual void process_choice(int selection){}
	virtual int get_option_count();
	
        virtual bool handle_element(eElement, Element *);
        virtual void load_attributes(CL_DomNamedNodeMap );
        virtual void load_finished();

        std::vector<BattleMenuOption*> m_options;
        eType m_eType;
	CL_Font m_onFont;
	CL_Font m_offFont;
	CL_Font m_selectedFont;
	CL_Colorf m_onColor;
	CL_Colorf m_offColor;
	CL_Colorf m_selectedColor;
	CL_Rectf m_rect;
	int m_font_height;
	ParameterList m_params;
    };
};

#endif

