#include "BattleState.h"
#include "MonsterRef.h"
#include "CharacterManager.h"
#include "GraphicsManager.h"
#include "IApplication.h"
#include "MonsterGroup.h"
#include "Monster.h"
#include "Party.h"
#include "BattleMenu.h"
#include "AnimationState.h"
#include "BattleConfig.h"
#include "MenuBox.h"
#include <sstream>
#include <iomanip>
#include <algorithm>



using namespace StoneRing;


namespace StoneRing{
class StatusEffectPainter : public Visitor<StatusEffect*>
{
public:
    StatusEffectPainter(CL_GraphicContext& gc):m_i(0),m_gc(gc){}
    virtual ~StatusEffectPainter(){}
    
    void SetShadow(CL_Colorf shadow){ m_shadow = shadow; }
    void SetSpacing(CL_Pointf spacing) { m_spacing = spacing; }
    void SetStart(CL_Pointf point) { m_start = point; }
    void SetHeight(int height) { m_height = height; }
    void SetMultiplier(int m) { m_mult = m; }
    
    virtual void Visit(StatusEffect* pEffect) {
        CL_Sprite sprite = pEffect->GetIcon();
        sprite.update();
        sprite.set_alpha(0.5f);
        int sprites_per_col = m_height / (sprite.get_height() + m_spacing.y);
        int col = m_i / sprites_per_col;
        int row = m_i % sprites_per_col;
        CL_Pointf origin(m_start.x + col * (m_spacing.x + sprite.get_width()) * m_mult,
                     m_start.y + row * (m_spacing.y + sprite.get_height()));
        CL_Sizef size(sprite.get_width(),sprite.get_height());
        CL_Rectf box(origin,size);
        CL_Rectf shadowbox = box;
        shadowbox.translate(CL_Pointf(2,2));

       // CL_Draw::fill(m_gc, shadowbox, m_shadow);
        sprite.draw(m_gc,origin.x,origin.y);
        m_i++;
    }
    
private:
    int m_i;
    int m_height;
    int m_mult;
    CL_Colorf m_shadow;
    CL_Pointf m_start;
    CL_Pointf m_spacing;
    CL_GraphicContext & m_gc;
};
}

const BattleState::SpriteTicket BattleState::UNDEFINED_SPRITE_TICKET=-1;

void BattleState::SetConfig(BattleConfig* config)
{
    m_config = config;
}

void BattleState::init(const std::vector<MonsterRef*>& monsters, int cellRows, int cellColumns, bool isBoss, const std::string & backdrop)
{
    CharacterManager * pCharManager = IApplication::GetInstance()->GetCharacterManager();

    m_monsters = new MonsterParty();

    m_nRows = cellRows;
    m_nColumns = cellColumns;

    m_cur_char = 0;
    m_nRound = 0;
    
    m_bDoneAfterRound = false;

    m_bBossBattle = isBoss;
    // uint bottomrow = m_nRows - 1;
    
    for (std::vector<MonsterRef*>::const_iterator it = monsters.begin();
            it != monsters.end(); it++)
    {
        MonsterRef *pRef= *it;
        uint count = pRef->GetCount();

        for (int y=0;y<pRef->GetRows();y++)
        {
            for (int x=0;x<pRef->GetColumns();x++)
            {
                if (count>0)
                {
                    Monster * pMonster = pCharManager->CreateMonster(pRef->GetName());
                    pMonster->SetCellX( pRef->GetCellX() + x );
                    pMonster->SetCellY( pRef->GetCellY() + y );

                    pMonster->SetCurrentSprite(GraphicsManager::CreateMonsterSprite(pMonster->GetName(),
                                               "idle")); // fuck, dude

                    m_monsters->AddMonster(pMonster);
                    pMonster->Invoke();
                }

                if (--count == 0) break;
            }
            if(count == 0) break;
        }

        if (count > 0) throw CL_Exception("Couldn't fit all monsters in their rows and columns");
    }
    m_backdrop = GraphicsManager::GetBackdrop(backdrop);
    
    
    m_ndarkMode = DISPLAY_ORDER_NO_DISPLAY;
    
}

void BattleState::init(const MonsterGroup &group, const std::string &backdrop)
{
    m_bBossBattle = false;
    const std::vector<MonsterRef*> & monsters = group.GetMonsters();

    init(monsters,group.GetCellRows(),group.GetCellColumns(),false,backdrop);
}

void BattleState::set_positions_to_loci()
{
    for(std::deque<ICharacter*>::iterator iter = m_initiative.begin();
    iter != m_initiative.end(); iter++)
    {
        CL_Pointf pos = get_character_locus_rect(*iter).get_center();
        (*iter)->SetBattlePos(pos);
    }
}

void BattleState::next_turn()
{
    if(m_bDoneAfterRound) 
    {
	m_bDone = true;
	return;
    }
    
    while(true)
    {
        ICharacter * iCharacter = m_initiative[m_cur_char];
        Character * pCharacter = dynamic_cast<Character*>(iCharacter);
        
        if(!iCharacter->GetToggle(ICharacter::CA_CAN_ACT)
           || !iCharacter->GetToggle(ICharacter::CA_ALIVE)){
            pick_next_character();
            continue;
        }
        
        set_positions_to_loci();
        iCharacter->StatusEffectRound();

        if (pCharacter != NULL)
        {
            ParameterList params;
            // Supply a handle to the character in question
            Character *pChar = dynamic_cast<Character*>(m_initiative[m_cur_char]);
            params.push_back ( ParameterListItem("$Character", pChar ) );
            params.push_back ( ParameterListItem("$Round", static_cast<int>(m_nRound) ) );

            pCharacter->GetBattleMenu()->SetEnableConditionParams(params, pChar);
            pCharacter->GetBattleMenu()->Init();

            // TODO: This check is wrong... maybe I should just add a 
            // "Skip" option that just calls finish...
            if(pCharacter->GetBattleMenu()->GetOptionsBegin() == 
                pCharacter->GetBattleMenu()->GetOptionsEnd())
            {
                // No options.. skip this guy
                pick_next_character();
                continue;
            }
            m_combat_state = BATTLE_MENU;
            m_menu_stack.push ( pCharacter->GetBattleMenu() );
            break;
        }
        else
        {
            Monster * pMonster = dynamic_cast<Monster*>(m_initiative[m_cur_char]);
            assert(pMonster != NULL); // has to be a monster...
            // Figure out what the monster will do here.
            m_combat_state = DISPLAY_ACTION;
            ParameterList params;
            // Supply a handle to the character in question;
            params.push_back ( ParameterListItem("$Character", pMonster ) );
            params.push_back ( ParameterListItem("$Round", static_cast<int>(m_nRound) ) );
            pMonster->Round(params);
            break;
        }
            
    }
    

}

void BattleState::pick_next_character()
{
    if (++m_cur_char >= m_initiative.size())
    {
        m_cur_char = 0;
        m_nRound++;
    }
}

bool characterInitiativeSort(const ICharacter* pChar1, const ICharacter* pChar2)
{
    return pChar1->GetInitiative() > pChar2->GetInitiative();
}

void BattleState::roll_initiative()
{
    IParty * party = IApplication::GetInstance()->GetParty();

    for (uint i=0;i<party->GetCharacterCount();i++)
    {
        ICharacter * pChar = party->GetCharacter(i);
        pChar->RollInitiative();
        m_initiative.push_back(pChar);
    }

    for (uint i=0;i<m_monsters->GetCharacterCount();i++)
    {
        m_monsters->GetCharacter(i)->RollInitiative();
        m_initiative.push_back(m_monsters->GetCharacter(i));
    }

    // Okay they're all in the deque, now we just need to sort
    // it's ass
    std::sort(m_initiative.begin(),m_initiative.end(),characterInitiativeSort);

#ifndef NDEBUG
    std::cerr << "Initiative Order:" << std::endl;
    for (std::deque<ICharacter*>::const_iterator iter = m_initiative.begin(); iter != m_initiative.end();
            iter++)
    {
        std::cerr << '\t' <<(*iter)->GetName() << " @ " << (*iter)->GetInitiative() << std::endl;
    }
#endif
}

bool BattleState::IsDone() const
{
    return m_bDone;
}

void BattleState::HandleButtonUp(const IApplication::Button& button)
{
    switch(button)
    {
	case IApplication::BUTTON_CONFIRM:
	 if (m_combat_state == BATTLE_MENU)
        {

                BattleMenuOption * pOption = m_menu_stack.top()->GetSelectedOption();
                if(pOption){
                    ParameterList params;
                    // Supply a handle to the character in question
                    Character *pChar = dynamic_cast<Character*>(m_initiative[m_cur_char]);
                    params.push_back ( ParameterListItem("$Character", pChar ) );
                    params.push_back ( ParameterListItem("$Round", static_cast<int>(m_nRound) ) );

                    if (pOption->Enabled(params,pChar))
                    {
                        pOption->Select(m_menu_stack,params,pChar);
                    }
                    else
                    {
                    // Play bbzt sound
                    }
                }
            }
            else 
            {
                // Play bbzt sound
            }
            break;
        case IApplication::BUTTON_CANCEL:
            if(m_combat_state == BATTLE_MENU)
            {
                if(m_menu_stack.size() > 1)
                {
                    m_menu_stack.pop();
                    // TODO: Should I "Deselect" here??
                }
            }
            break;
	    
	
    }
}

void BattleState::HandleButtonDown(const IApplication::Button& button)
{
    
}

void BattleState::HandleAxisMove(const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos)
{
    if(axis == IApplication::AXIS_VERTICAL)
    {
	if(dir == IApplication::AXIS_DOWN)
	{
	    if (m_combat_state == BATTLE_MENU)
		m_menu_stack.top()->SelectDown();
	}
	else if(dir == IApplication::AXIS_UP)
	{
	    if (m_combat_state == BATTLE_MENU)
		m_menu_stack.top()->SelectUp();
	}
    }
}

void BattleState::HandleKeyDown(const CL_InputEvent &key)
{

}

void BattleState::HandleKeyUp(const CL_InputEvent &key)
{

}

void BattleState::Draw(const CL_Rect &screenRect,CL_GraphicContext& GC)
{
    assert(m_draw_method != NULL);

    if(m_eState == TRANSITION_IN){
        float passed = (float)(CL_System::get_time() - m_startup_time) / 1000.0f;
        if(passed >= 1.0f) {
            m_eState = COMBAT;
            next_turn();
            (this->*m_draw_method)(screenRect,GC);            
        }else {
            if(m_transition == FLIP_ZOOM || 
                m_transition == FLIP_ZOOM_SPIN ||
                m_transition == FADE_IN
            ){
            //  CL_Sizef screenSize = screenRect.get_size() * passed;
                CL_Texture texture(GC,screenRect.get_width(),screenRect.get_height(),cl_rgba);
                CL_FrameBuffer framebuffer(GC);
                framebuffer.attach_color_buffer(0,texture);
                GC.set_frame_buffer(framebuffer);
                (this->*m_draw_method)(screenRect,GC);
                GC.reset_frame_buffer();
                
                CL_SpriteDescription desc;
                desc.add_frame(texture);
                CL_Sprite sprite(GC,desc);
                if(m_transition == FLIP_ZOOM || m_transition == FLIP_ZOOM_SPIN){
                    CL_Rectf destRect((screenRect.get_width() - (screenRect.get_width() * passed))/2.0,
                                    (screenRect.get_height() - (screenRect.get_height() * passed))/2.0,
                                    (screenRect.get_width() *passed),(screenRect.get_height() * passed));
                    //image.set_scale(passed,passed);
                    if(m_transition == FLIP_ZOOM_SPIN)
                        sprite.set_angle(CL_Angle::from_degrees(passed * 360.0f));
                    sprite.draw(GC,destRect);
                }else if(m_transition == FADE_IN){
                    sprite.set_alpha(passed);
                    sprite.draw(GC,0,0);
                }
            }
        }
    }else {
        (this->*m_draw_method)(screenRect,GC);
    }
}

bool BattleState::LastToDraw() const
{
    return false;
}

bool BattleState::DisableMappableObjects() const
{
    return true;
}

void BattleState::MappableObjectMoveHook()
{
}

void BattleState::StartTargeting()
{
    m_targets.selected.m_pTarget = NULL;
    m_targets.m_bSelectedGroup = false;
    m_combat_state = TARGETING;
}

void BattleState::FinishTargeting()
{
    m_combat_state = DISPLAY_ACTION;
}

void BattleState::Start()
{
    m_draw_method = &BattleState::draw_battle;
    m_eState = TRANSITION_IN;
  
    float r = ranf();
    if(r > 0.66f) 
        m_transition = FLIP_ZOOM;
    else if(r > 0.33f)
        m_transition = FLIP_ZOOM_SPIN;
    else
        m_transition = FADE_IN;

    m_bDone = false;

#if 0
    m_pStatusHPFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::HP);
    m_pStatusMPFont =  pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::MP);
    m_pStatusBPFont =  pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::BP);
    m_pStatusGeneralFont =  pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::GENERAL);
    m_pStatusBadFont = pGraphicsManager->GetFont(GraphicsManager::BATTLE_STATUS, GraphicsManager::BAD);
#endif

    m_generalFont = GraphicsManager::GetFont(GraphicsManager::BATTLE_STATUS,"general");
    m_hpFont = GraphicsManager::GetFont(GraphicsManager::BATTLE_STATUS,"hp");
    m_mpFont = GraphicsManager::GetFont(GraphicsManager::BATTLE_STATUS,"mp");
    m_bpFont = GraphicsManager::GetFont(GraphicsManager::BATTLE_STATUS,"bp");
    m_charNameFont = GraphicsManager::GetFont(GraphicsManager::BATTLE_STATUS,"name");
    
    m_bp_gradient = GraphicsManager::GetGradient(GraphicsManager::BATTLE_STATUS,"bp_bar");
    m_bp_box = GraphicsManager::GetRect(GraphicsManager::BATTLE_STATUS,"bp_box");

    m_bp_border = GraphicsManager::GetColor(GraphicsManager::BATTLE_STATUS,"bp_border");
;

    m_player_rect = GraphicsManager::GetRect(GraphicsManager::BATTLE_STATUS,"player");
    m_monster_rect = GraphicsManager::GetRect(GraphicsManager::BATTLE_STATUS,"monster");
    m_statusRect = GraphicsManager::GetRect(GraphicsManager::BATTLE_STATUS,"status");
    m_popupRect = GraphicsManager::GetRect(GraphicsManager::BATTLE_POPUP_MENU,"popup");
    m_status_text_rect = GraphicsManager::GetRect(GraphicsManager::BATTLE_STATUS, "text");


    m_status_effect_shadow_color = GraphicsManager::GetColor(GraphicsManager::BATTLE_STATUS, "status_effect_shadow");
    m_status_effect_spacing = GraphicsManager::GetPoint(GraphicsManager::BATTLE_STATUS,"status_effect_spacing");
    m_bp_gradient = GraphicsManager::GetGradient(GraphicsManager::BATTLE_STATUS,"bp_bar");
    init_or_release_players(false);

    m_bDone = false;
    roll_initiative();
    set_positions_to_loci();
    m_startup_time = CL_System::get_time();
    //next_turn();
}

void BattleState::init_or_release_players(bool bRelease)
{
    IParty * pParty = IApplication::GetInstance()->GetParty();

    uint count = pParty->GetCharacterCount();
    for (uint nPlayer = 0;nPlayer < count; nPlayer++)
    {
        Character * pCharacter = dynamic_cast<Character*>(pParty->GetCharacter(nPlayer));
        assert(pCharacter);
        std::string name = pCharacter->GetName();
        if (!bRelease) // TODO: If they're dead, their sprite should be dead.
            pCharacter->SetCurrentSprite(GraphicsManager::CreateCharacterSprite(name,"idle")); // bullshit
    }
}


void BattleState::Finish()
{

    for (uint i =0; i < m_monsters->GetCharacterCount();i++)
        delete m_monsters->GetCharacter(i);

    delete m_monsters;

    m_displays.clear();
    
    m_sprites.clear();

    m_initiative.clear();

    init_or_release_players(true);
}

void BattleState::draw_darkness(const CL_Rectf &screenRect, CL_GraphicContext& GC)
{
    CL_Draw::fill(GC,screenRect,m_darkColor);
}

void BattleState::draw_transition_in(const CL_Rectf &screenRect, CL_GraphicContext& GC)
{
}

void BattleState::draw_start(const CL_Rectf &screenRect, CL_GraphicContext& GC)
{
}

void BattleState::draw_status(const CL_Rectf &screenRect, CL_GraphicContext& GC)
{
    IParty * pParty = IApplication::GetInstance()->GetParty();
    
    MenuBox::Draw(GC,m_statusRect);
    
    CL_Rectf textRect = m_status_text_rect;
    
    //CL_Draw::box(GC,m_status_rect,CL_Colorf::red);
    int height_per_character = textRect.get_height() / pParty->GetCharacterCount();

    for (uint p = 0; p < pParty->GetCharacterCount(); p++)
    {
        ICharacter * pChar = pParty->GetCharacter(p);
        std::ostringstream name;
        name << std::setw(16) << std::left << pChar->GetName();
        std::ostringstream hp;
        hp << std::setw(6) << pChar->GetAttribute(ICharacter::CA_HP) << '/'
        << pChar->GetAttribute(ICharacter::CA_MAXHP);
        
        m_generalFont.draw_text(GC,textRect.left,
                              textRect.top +(p * height_per_character),
                              name.str(),Font::TOP_LEFT);
        m_hpFont.draw_text(GC,textRect.get_width() / 3 + textRect.left,
                         textRect.top +(p* height_per_character)
                         ,hp.str(),Font::TOP_LEFT);
        std::ostringstream mp;
        mp << std::setw(6) << pChar->GetAttribute(ICharacter::CA_MP) << '/'
        << pChar->GetAttribute(ICharacter::CA_MAXMP);
        m_mpFont.draw_text(GC,(textRect.get_width() / 3) * 2 + textRect.left,
                         textRect.top +  (p*height_per_character),mp.str(),Font::TOP_LEFT
			);
        
        CL_Rectf bp_box = m_bp_box;
        bp_box.top += textRect.top + (p*height_per_character);
        bp_box.bottom += textRect.top + (p*height_per_character);
        CL_Rectf bp_fill = bp_box;
        float percentage = pChar->GetAttribute(ICharacter::CA_BP) / pChar->GetAttribute(ICharacter::CA_MAXBP);
        bp_fill.right = bp_fill.left + (percentage * bp_fill.get_width());
        CL_Draw::gradient_fill(GC,bp_fill,m_bp_gradient);
        CL_Draw::box(GC,bp_box,m_bp_border);
    }
}

void BattleState::draw_battle(const CL_Rectf &screenRect, CL_GraphicContext& GC)
{

    m_backdrop.draw(GC,screenRect);

    if(m_ndarkMode == DISPLAY_ORDER_PRE_PLAYERS)
	draw_darkness(screenRect,GC);
    draw_players(m_player_rect,GC);
    if(m_ndarkMode == DISPLAY_ORDER_POST_PLAYERS)
	draw_darkness(screenRect,GC);
    
    if(m_ndarkMode == DISPLAY_ORDER_PREMONSTERS)
	draw_darkness(screenRect,GC);
    draw_monsters(m_monster_rect,GC);
    if(m_ndarkMode == DISPLAY_ORDER_POSTMONSTERS)
	draw_darkness(screenRect,GC);

    draw_status(screenRect,GC);
    
    if(m_ndarkMode == DISPLAY_ORDER_PRE_SPRITES)
	draw_darkness(screenRect,GC);
    draw_sprites(0,GC);
    draw_status_effects(GC);
    if(m_ndarkMode == DISPLAY_ORDER_POST_SPRITES)
	draw_darkness(screenRect,GC);
    
    if(m_ndarkMode == DISPLAY_ORDER_PRE_DISPLAYS)
	draw_darkness(screenRect,GC);
    draw_displays(GC);
    if(m_ndarkMode == DISPLAY_ORDER_POST_DISPLAYS)
	draw_darkness(screenRect,GC);
    

    if(m_ndarkMode == DISPLAY_ORDER_PRE_MENUS)
	draw_darkness(screenRect,GC);
    if (m_eState == COMBAT &&  m_combat_state == BATTLE_MENU)
    {
        draw_menus(screenRect,GC);
    }
    
    if(m_ndarkMode == DISPLAY_ORDER_POST_MENUS)
	draw_darkness(screenRect,GC);
    
    if(m_ndarkMode == DISPLAY_ORDER_FINAL) 
	draw_darkness(screenRect,GC);

}

void BattleState::draw_status_effects ( CL_GraphicContext& GC )
{
    for(std::deque<ICharacter*>::iterator iter = m_initiative.begin();
        iter != m_initiative.end(); iter++)
        {
            ICharacter * pICharacter = *iter;
            Character * pCharacter = dynamic_cast<Character*>(pICharacter);
            
            StatusEffectPainter painter(GC);
            painter.SetShadow(m_status_effect_shadow_color);
            painter.SetSpacing(m_status_effect_spacing);
            if(pCharacter != NULL)
            {
                CL_Rectf rectf = get_character_rect(pICharacter);
            
                painter.SetStart ( rectf.get_top_right() );
                painter.SetHeight(rectf.get_height());
                painter.SetMultiplier(1);
               
            }
            else
            {
                // Monster
                if(!pICharacter->GetToggle(ICharacter::CA_ALIVE) || 
                    !pICharacter->GetToggle(ICharacter::CA_VISIBLE))
                    continue;
                CL_Rectf rectf = get_character_rect(pICharacter);
                painter.SetHeight(rectf.get_height());
                painter.SetStart ( rectf.get_top_left());
                painter.SetMultiplier(-1);
            }
            
            pICharacter->IterateStatusEffects(painter);          
            
        }
}


BattleState::Display::Display(BattleState& parent,BattleState::Display::eDisplayType type,int damage,ICharacter* pICharacter)
        :m_parent(parent),m_eDisplayType(type),m_amount(-damage)
{
    m_pTarget = dynamic_cast<ICharacter*>(pICharacter);
}
BattleState::Display::~Display()
{
}

void BattleState::Display::start()
{
    m_start_time = CL_System::get_time();
    m_complete = 0.0f;
}

bool BattleState::Display::expired() const
{
    return m_complete >= 1.0f;
}

void BattleState::Display::update()
{
    int elapsed = CL_System::get_time()-m_start_time;

    m_complete = (double)elapsed / 2000.0;
}
void BattleState::Display::draw(CL_GraphicContext& GC)
{
    std::ostringstream stream;
    Font font;
    Font shadow_font;

    GraphicsManager::DisplayFont displayFont;
    

    switch (m_eDisplayType)
    {
    case DISPLAY_CRITICAL:
        stream << "CRITICAL ";
        // fallthrough
    case DISPLAY_DAMAGE:
        if (m_amount >= 0)
        {
            displayFont = GraphicsManager::DISPLAY_HP_POSITIVE;
            stream << m_amount;
        }
        else
        {
            displayFont = GraphicsManager::DISPLAY_HP_NEGATIVE;
            stream << -m_amount;
        }
        break;
    case DISPLAY_MP:
        if (m_amount >= 0)
        {
            displayFont = GraphicsManager::DISPLAY_MP_POSITIVE;
            stream << m_amount;
        }
        else
        {
            displayFont = GraphicsManager::DISPLAY_MP_NEGATIVE;
            stream << -m_amount;
        }
        break;
    case DISPLAY_MISS:
        displayFont = GraphicsManager::DISPLAY_MISS;
        stream << "MISS";
        break;

    }

    font = GraphicsManager::GetFont(GraphicsManager::GetFontName(displayFont));
    shadow_font = GraphicsManager::GetFont(GraphicsManager::GetFontName(GraphicsManager::DISPLAY_FONT_SHADOW));

    font.set_alpha(1.0f - m_complete);
    shadow_font.set_alpha(1.0f-m_complete);

    CL_Rect rect;
    Monster *pMonster = dynamic_cast<Monster*>(m_pTarget);

    if (pMonster)
    {
        rect = m_parent.get_character_rect(pMonster);
    }
    else
    {
        IParty * pParty = IApplication::GetInstance()->GetParty();
        for (uint n=0;n<pParty->GetCharacterCount();n++)
        {
            if (m_pTarget == pParty->GetCharacter(n))
            {
                rect = m_parent.get_character_rect(pParty->GetCharacter(n));
                break;
            }
        }
    }

    CL_String value(stream.str());

    CL_Point pos = rect.get_top_right();

    pos.y += font.get_font_metrics(GC).get_height();
    pos.x -= font.get_text_size(GC,value).width;

    shadow_font.draw_text(GC,pos.x-2,pos.y-2,value);
    font.draw_text(GC,pos.x,pos.y,value);


}

BattleState::Sprite::Sprite(CL_Sprite sprite):m_sprite(sprite)
{
}
 
BattleState::Sprite::~Sprite()
{
}
 
void BattleState::Sprite::SetPosition(const CL_Pointf& pos)
{
    m_pos = pos;
}
 
CL_Pointf BattleState::Sprite::Position()const
{
    return m_pos;
}
void BattleState::Sprite::Draw(CL_GraphicContext& gc)
{
    m_sprite.set_alignment(origin_center);
    m_sprite.draw(gc,m_pos.x,m_pos.y);
}

int BattleState::Sprite::ZOrder() const
{
    return m_zorder;
}

void BattleState::Sprite::SetZOrder(int z)
{
    m_zorder = z;
}

bool BattleState::Sprite::Enabled() const
{
	return m_enabled;
}

void BattleState::Sprite::SetEnabled(bool enabled)
{
	m_enabled = enabled;
}

CL_Sprite BattleState::Sprite::GetSprite() const
{
    return m_sprite;
}

int BattleState::add_sprite(CL_Sprite sprite)
{
    int index = m_sprites.size();
    sprite.set_alignment(origin_center);
    Sprite mysprite(sprite);

    
    for(int i=0;i<m_sprites.size();i++)
    {
	if(!m_sprites[i].Enabled())
	{
	    m_sprites[i] = mysprite;
	    m_sprites[i].SetEnabled(true);
	    return i;
	}
    }
    m_sprites.push_back(mysprite);
    m_sprites.back().SetEnabled(true);
    return index;
}

void BattleState::set_sprite_pos(int nSprite, CL_Pointf pos)
{
    m_sprites[nSprite].SetPosition(pos);
}


CL_Sprite BattleState::get_sprite(int nSprite) const
{
    return m_sprites[nSprite].GetSprite();
}

void BattleState::remove_sprite(int nSprite)
{
    if(nSprite <  m_sprites.size())
    {
	m_sprites[nSprite].SetEnabled(false);
    }
}

void BattleState::draw_sprites(int z, CL_GraphicContext& GC)
{
    for(int i=0;i<m_sprites.size();i++)
    {
	if(m_sprites[i].Enabled())
	{
	    m_sprites[i].GetSprite().update();
	    m_sprites[i].Draw(GC);
	}
    }
}

bool SortByBattlePos(const ICharacter* d1, const ICharacter* d2)
{

    CL_Pointf pos1,pos2;
    pos1 = d1->GetBattlePos();
    pos2 = d2->GetBattlePos();

    
  return pos1.y * IApplication::GetInstance()->GetDisplayRect().get_width() + pos1.x <
	pos2.y * IApplication::GetInstance()->GetDisplayRect().get_width() + pos2.x;
}


void BattleState::draw_monsters(const CL_Rectf &monsterRect, CL_GraphicContext& GC)
{
    std::vector<Monster*> sortedList;
    sortedList.reserve(m_monsters->GetCharacterCount());
    for(uint i=0;i<m_monsters->GetCharacterCount();i++)
    {
	Monster *pMonster = dynamic_cast<Monster*>(m_monsters->GetCharacter(i));
	sortedList.push_back(pMonster);	
    }
    
   std::sort(sortedList.begin(), sortedList.end(), SortByBattlePos);

    

    for (uint i = 0; i < sortedList.size(); i++)
    {
        Monster *pMonster = sortedList[i];

       // ICharacter *iCharacter = m_monsters->GetCharacter(i);
        if (!pMonster->GetToggle(ICharacter::CA_VISIBLE))
            continue;

       // CL_Rectf rect = get_character_rect(pMonster);
	CL_Pointf center = pMonster->GetBattlePos();
	//std::cout << pMonster->GetName() << '@' << rect.top << ',' << rect.left << std::endl;
	//rect.translate ( rect.get_width() / 2.0f,  rect.get_height() / 2.0f);

        CL_Sprite  sprite = current_sprite(pMonster);
        
        if(!pMonster->GetToggle(ICharacter::CA_DRAW_STILL) &&  pMonster->GetToggle(ICharacter::CA_ALIVE))
            sprite.update();

        if (m_combat_state == TARGETING)
        {
            if ((m_targets.m_bSelectedGroup && m_targets.selected.m_pGroup == m_monsters)
                    || (!m_targets.m_bSelectedGroup && pMonster == m_targets.selected.m_pTarget))
            {
                CL_FontMetrics metrics = m_charNameFont.get_font_metrics(GC);
		CL_Sizef textSize = m_charNameFont.get_text_size(GC,pMonster->GetName());
                CL_Pointf textPoint = CL_Pointf(center.x - textSize.width/2, get_character_rect(pMonster).get_top_left().y);
                
                if(textPoint.y - metrics.get_height() < 0){
                    // Gonna have to draw below the monster
                    textPoint = CL_Pointf(center.x - textSize.width/2, 
                                          get_character_rect(pMonster).get_bottom_left().y + metrics.get_height());
                }
                
                m_charNameFont.draw_text(GC, textPoint, pMonster->GetName(), Font::DEFAULT);
                sprite.draw(GC,center.x,center.y);
            }
            else
            {
                //sprite.set_alpha(0.7f);
                sprite.draw(GC,center.x,center.y);
            }
        }
        else
        {
            sprite.draw(GC,center.x,center.y);
        }
        

    }
}

CL_Sprite BattleState::current_sprite ( ICharacter* iCharacter )
{
        CL_Sprite  sprite = iCharacter->GetCurrentSprite(true);
        return sprite;
}


void BattleState::draw_players(const CL_Rectf &playerRect, CL_GraphicContext& GC)
{
    IParty * pParty = IApplication::GetInstance()->GetParty();

    uint playercount = pParty->GetCharacterCount();
    for (uint nPlayer = 0; nPlayer < playercount; nPlayer++)
    {
        Character * pCharacter = dynamic_cast<Character*>(pParty->GetCharacter(nPlayer));
        ICharacter * iCharacter = pParty->GetCharacter(nPlayer);
        
        CL_Sprite sprite = current_sprite(iCharacter);
       // CL_Rect rect = get_character_rect(iCharacter);
	CL_Pointf center = pCharacter->GetBattlePos();
	//rect.translate (  rect.get_width() / 2.0f,  rect.get_height() / 2.0f );

        if(!pCharacter->GetToggle(ICharacter::CA_DRAW_STILL))
            sprite.update();

        // Need to get the spacing from game config
        if (!pCharacter->GetToggle(ICharacter::CA_VISIBLE))
            continue;

        if (m_combat_state == TARGETING)
        {
            if ((m_targets.m_bSelectedGroup && m_targets.selected.m_pGroup == m_monsters)
                    || (!m_targets.m_bSelectedGroup && iCharacter == m_targets.selected.m_pTarget))
            {
		//sprite.set_alpha(1.0f);
                sprite.draw(GC,center.x,center.y);
            }
            else
            {
                //sprite.set_alpha(0.7f);
                sprite.draw(GC,center.x,center.y);

            }
        }
        else
        {
            sprite.draw(GC,center.x,center.y);
        }

    }
}

CL_Rectf  BattleState::get_group_rect(ICharacterGroup* group) const
{
    MonsterParty * monsterParty = dynamic_cast<MonsterParty*>(group);

    if(monsterParty != NULL){
        return m_monster_rect;
    }else{
        return m_player_rect;
    }
}

CL_Pointf BattleState::get_monster_locus(const Monster * pMonster)const
{
    CL_Pointf point;
    const uint cellWidth = m_monster_rect.get_width() / m_nColumns;
    const uint cellHeight = m_monster_rect.get_height() / m_nRows;
    CL_Sprite  sprite = const_cast<Monster*>(pMonster)->GetCurrentSprite();


    point.x = m_monster_rect.left + pMonster->GetCellX() * cellWidth + (cellWidth  /2);
    point.y = m_monster_rect.top + pMonster->GetCellY() * cellHeight + (cellHeight /2);
    return point;
}

CL_Pointf BattleState::get_player_locus(uint nPlayer)const
{
    CL_Pointf point;
    const uint partySize = IApplication::GetInstance()->GetParty()->GetCharacterCount();
    double each_player = m_player_rect.get_width() / partySize;
    // TODO: Get the spacing from config
    point.x = m_player_rect.left + (nPlayer) * each_player + (each_player/2.0);
    point.y = m_player_rect.top + (nPlayer) * 64;
    return point;
}

CL_Pointf BattleState::get_character_locus(const ICharacter* pCharacter) const
{
    CL_Point pointf;
    const Monster * pMonster = dynamic_cast<const Monster*>(pCharacter);
    if(pMonster != NULL){
        return get_monster_locus(pMonster);
    }else{
        IParty * pParty = IApplication::GetInstance()->GetParty();
        uint playercount = pParty->GetCharacterCount();
        for(uint i=0;i<playercount;i++){
            ICharacter * iCharacter = pParty->GetCharacter(i);

            if(pCharacter == iCharacter){
                return get_player_locus(i);
            }
        }
    }
    assert(0);
    return CL_Pointf(0.0,0.0);
}

CL_Sizef  BattleState::get_character_size(const ICharacter* pCharacter) const
{
    const Monster * pMonster = dynamic_cast<const Monster*>(pCharacter);
    if(pMonster != NULL){
        return const_cast<Monster*>(pMonster)->GetCurrentSprite().get_size();
    }else{
        return CL_Sizef(60,120);
    }
}


CL_Rectf BattleState::get_character_rect(const ICharacter* pCharacter)const
{
    CL_Rectf rect;
    CL_Pointf point = pCharacter->GetBattlePos();
    CL_Sizef size = get_character_size(pCharacter);
    
    rect.set_top_left(CL_Pointf(point.x - size.width / 2.0f,point.y - size.height / 2.0f));
   /* rect.top = point.x - size.width / 2.0f;
    rect.left = point.y - size.height / 2.0f;
    rect.bottom = rect.top + size.height;
    rect.right = rect.left + size.width; */
    rect.set_size(size);
  
   
    return rect;
}

CL_Rectf BattleState::get_character_locus_rect(const ICharacter* pCharacter)const
{
    CL_Rectf rect;
    CL_Pointf point = get_character_locus(pCharacter);
    CL_Sizef size = get_character_size(pCharacter);
    rect.set_top_left(CL_Pointf(point.x - size.width / 2.0f, point.y - size.height / 2.0f));
    rect.set_size(get_character_size(pCharacter));
    
    
    /*
    std::cout << pCharacter->GetName() <<" locus " << '(' << rect.left << ',' << rect.top << ')' << '(' 
    << rect.right << ',' << rect.bottom <<')' << 'c'
    << rect.get_center().x << ',' << rect.get_center().y << std::endl;
    */
    return rect;
}


void BattleState::draw_displays(CL_GraphicContext& GC)
{
    for (std::list<Display>::iterator iter = m_displays.begin();iter != m_displays.end();iter++)
    {
        iter->update();
        iter->draw(GC);
    }

    // m_displays.remove_if(Display::has_expired);
    m_displays.remove_if(std::mem_fun_ref(&Display::expired));
}


void BattleState::draw_menus(const CL_Rectf &screenrect, CL_GraphicContext& GC)
{
    BattleMenu * pMenu = m_menu_stack.top();
    CL_Rectf menu_rect = m_popupRect;
    menu_rect.translate(GraphicsManager::GetMenuInset());
    menu_rect.shrink(GraphicsManager::GetMenuInset().x,GraphicsManager::GetMenuInset().y);

    pMenu->SetRect(menu_rect);

    if (pMenu->GetType() == BattleMenu::POPUP)
    {
        MenuBox::Draw(GC,m_popupRect,false);
        pMenu->Draw(GC);
    }
}
void BattleState::SteelInit(SteelInterpreter* pInterpreter)
{
    pInterpreter->pushScope();

    static SteelFunctor3Arg<BattleState,bool,bool,bool> fn_selectTargets(this,&BattleState::selectTargets);
    static SteelFunctorNoArgs<BattleState> fn_finishTurn(this,&BattleState::finishTurn);
    static SteelFunctorNoArgs<BattleState> fn_cancelOption(this,&BattleState::cancelOption);
    static SteelFunctor3Arg<BattleState,SteelType::Handle,SteelType::Handle,SteelType::Handle> fn_doTargetedAnimation(this,&BattleState::doTargetedAnimation);
    static SteelFunctor2Arg<BattleState,SteelType::Handle,SteelType::Handle> fn_doCharacterAnimation(this,&BattleState::doCharacterAnimation);
    static SteelFunctor3Arg<BattleState,int,SteelType::Handle,int> fn_createDisplay(this,&BattleState::createDisplay);
    static SteelFunctor1Arg<BattleState,bool> fn_getCharacterGroup(this,&BattleState::getCharacterGroup);
    static SteelFunctorNoArgs<BattleState> fn_getAllCharacters(this,&BattleState::getAllCharacters);
    static SteelFunctor1Arg<BattleState,SteelType::Handle> fn_getMonsterDamageCategory(this,&BattleState::getMonsterDamageCategory);

    static SteelFunctorNoArgs<BattleState> fn_flee(this,&BattleState::flee);
    static SteelFunctorNoArgs<BattleState> fn_isBossBattle(this,&BattleState::isBossBattle);
    static SteelFunctor1Arg<BattleState,int> fn_darkMode(this, &BattleState::darkMode);
    static SteelFunctor4Arg<BattleState,double,double,double,double> fn_darkColor(this,&BattleState::darkColor);

    SteelConst(pInterpreter,"$_DISP_DAMAGE", Display::DISPLAY_DAMAGE);
    SteelConst(pInterpreter,"$_DISP_MP", Display::DISPLAY_MP);
    SteelConst(pInterpreter,"$_DISP_MISS", Display::DISPLAY_MISS);
    SteelConst(pInterpreter,"$_DISP_CRITICAL",Display::DISPLAY_CRITICAL);
    SteelConst(pInterpreter,"$_POST_BACKDROP", DISPLAY_ORDER_POSTBACKDROP);
    SteelConst(pInterpreter,"$_PRE_MONSTERS", DISPLAY_ORDER_PREMONSTERS);
    SteelConst(pInterpreter,"$_POST_MONSTERS", DISPLAY_ORDER_POSTMONSTERS);
    SteelConst(pInterpreter,"$_PRE_PLAYERS", DISPLAY_ORDER_PRE_PLAYERS);
    SteelConst(pInterpreter,"$_POST_PLAYERS", DISPLAY_ORDER_POST_PLAYERS);
    SteelConst(pInterpreter,"$_PRE_SPRITES", DISPLAY_ORDER_PRE_SPRITES);
    SteelConst(pInterpreter,"$_POST_SPRITES", DISPLAY_ORDER_POST_SPRITES);
    SteelConst(pInterpreter,"$_PRE_DISPLAYS", DISPLAY_ORDER_PRE_DISPLAYS);
    SteelConst(pInterpreter,"$_POST_DISPLAYS",DISPLAY_ORDER_POST_DISPLAYS);
    SteelConst(pInterpreter, "$_PRE_MENUS", DISPLAY_ORDER_PRE_MENUS);
    SteelConst(pInterpreter, "$_POST_MENUS", DISPLAY_ORDER_POST_MENUS);
    SteelConst(pInterpreter, "$_FINAL_DRAW", DISPLAY_ORDER_FINAL);

    pInterpreter->addFunction("selectTargets","battle",new SteelFunctor3Arg<BattleState,bool,bool,bool>(this,&BattleState::selectTargets));
    pInterpreter->addFunction("finishTurn","battle",new SteelFunctorNoArgs<BattleState>(this,&BattleState::finishTurn));
    pInterpreter->addFunction("cancelOption","battle",new SteelFunctorNoArgs<BattleState>(this,&BattleState::cancelOption));
    pInterpreter->addFunction("doTargetedAnimation","battle",new SteelFunctor3Arg<BattleState,SteelType::Handle,SteelType::Handle,SteelType::Handle>(this,&BattleState::doTargetedAnimation));
    pInterpreter->addFunction("doCharacterAnimation","battle",new SteelFunctor2Arg<BattleState,SteelType::Handle,SteelType::Handle>(this,&BattleState::doCharacterAnimation));
    pInterpreter->addFunction("createDisplay","battle",new SteelFunctor3Arg<BattleState,int,SteelType::Handle,int>(this,&BattleState::createDisplay));
    pInterpreter->addFunction("getCharacterGroup","battle",new SteelFunctor1Arg<BattleState,bool>(this,&BattleState::getCharacterGroup));
    pInterpreter->addFunction("getAllCharacters","battle",new SteelFunctorNoArgs<BattleState>(this,&BattleState::getAllCharacters));
    pInterpreter->addFunction("getMonsterDamageCategory","battle",new SteelFunctor1Arg<BattleState,SteelType::Handle>(this,&BattleState::getMonsterDamageCategory));
    //pInterpreter->addFunction("getSkill","battle",new SteelFunctor2Arg<BattleState,SteelType::Handle,const std::string&>(this,&BattleState::getSkill));
    pInterpreter->addFunction("flee","battle",new SteelFunctorNoArgs<BattleState>(this,&BattleState::flee));
    pInterpreter->addFunction("isBossBattle","battle",new SteelFunctorNoArgs<BattleState>(this,&BattleState::isBossBattle));
    pInterpreter->addFunction("darkMode","battle",new SteelFunctor1Arg<BattleState,int>(this,&BattleState::darkMode));
    pInterpreter->addFunction("darkColor","battle",new SteelFunctor4Arg<BattleState,double,double,double,double>(this,&BattleState::darkColor));
    m_config->SetupForBattle();

}

void BattleState::SteelCleanup   (SteelInterpreter* pInterpreter)
{
    pInterpreter->removeFunctions("battle");
    pInterpreter->popScope();
    m_config->TeardownForBattle();
}

ICharacter* BattleState::get_next_character(const ICharacterGroup* pParty, const ICharacter* pCharacter) const
{
    const int party_count = pParty->GetCharacterCount();
    for (int i = 0;i<party_count;i++)
    {
        if (pParty->GetCharacter(i) == pCharacter)
        {
            if (i +1 == party_count)
            {
                return pParty->GetCharacter(0);
            }
            else
            {
                return pParty->GetCharacter(i+1);
            }
        }
    }

    assert(0);
    return NULL;
}

ICharacter* BattleState::get_prev_character(const ICharacterGroup* pParty, const ICharacter* pCharacter) const
{
    const int party_count = pParty->GetCharacterCount();
    for (int i = 0;i<party_count;i++)
    {
        if (pParty->GetCharacter(i) == pCharacter)
        {
            if (i - 1 < 0)
            {
                return pParty->GetCharacter(party_count-1);
            }
            else
            {
                return pParty->GetCharacter(i-1);
            }
        }

    }

    assert(0);
    return NULL;
}

#if 0
void BattleState::SelectNextTarget()
{
    Monster * pMonster = dynamic_cast<Monster*>(m_targets.selected.m_pTarget);
    if (pMonster != NULL)
    {
        m_targets.selected.m_pTarget = get_next_character(m_monsters,pMonster);
    }
    else
    {
        Character * pCharacter = dynamic_cast<Character*>(m_targets.selected.m_pTarget);
        const Party * pParty = dynamic_cast<Party*>(IApplication::GetInstance()->GetParty());
        m_targets.selected.m_pTarget = get_next_character(pParty,pCharacter);
    }
}

void BattleState::SelectPreviousTarget()
{
    Monster * pMonster = dynamic_cast<Monster*>(m_targets.selected.m_pTarget);
    if (pMonster != NULL)
    {
        // We have a monster
        m_targets.selected.m_pTarget = get_prev_character(m_monsters,pMonster);
    }
    else
    {
        Character * pCharacter = dynamic_cast<Character*>(m_targets.selected.m_pTarget);
        const Party * pParty = dynamic_cast<Party*>(IApplication::GetInstance()->GetParty());
        m_targets.selected.m_pTarget = get_prev_character(pParty,pCharacter);
    }
}

void BattleState::SelectLeftTarget()
{
// 
}

void BattleState::SelectRightTarget()
{

}


void BattleState::SelectUpTarget()
{

}
void BattleState::SelectDownTarget()
{


}

#endif		

ICharacterGroup* BattleState::group_for_character(ICharacter* pICharacter)
{
	if(pICharacter->IsMonster()) return m_monsters;
	else return IApplication::GetInstance()->GetParty();
}

bool BattleState::MonstersOnLeft()
{
    return true;
}

void BattleState::FinishTurn()
{
    // Clear the menu stack
    while(!m_menu_stack.empty())
        m_menu_stack.pop();
    // TODO: Good time to check if either side is wiped out, etc
    // And, any dead monsters need to have ->Remove called on them
    ParameterList params;
    m_config->OnTurn(params);
    check_for_death();
    if(!end_conditions()){
	pick_next_character();
	next_turn();
    }
}


// Go back to menu, they decided not to proceed with this option
void BattleState::CancelTargeting()
{
    m_targets.m_bSelectedGroup = false;
    m_targets.selected.m_pTarget = NULL;
}

bool BattleState::end_conditions()
{
    IParty * party = IApplication::GetInstance()->GetParty();
       // Now, see if the monsters are all dead
    bool anyAlive = false;
    for(int i=0;i<m_monsters->GetCharacterCount();i++)
    {
	if(m_monsters->GetCharacter(i)->GetToggle(ICharacter::CA_ALIVE))
	{
	    anyAlive= true;
	    break;
	}
    }
    
    if(!anyAlive)
    {
	// You win!
	// TODO: Win
	win();
	// return so you don't lose after you win
	return true;
    }
    
    anyAlive = false;
    
    for(int i=0;i<party->GetCharacterCount();i++)
    {
	if(party->GetCharacter(i)->GetToggle(ICharacter::CA_ALIVE) )
	{
	    anyAlive = true;
	    break;
	}
    }
    
    if(!anyAlive)
    {
	lose();
	return true;
    }
    
    return false;
}


void BattleState::check_for_death()
{

    for (std::deque<ICharacter*>::const_iterator iter = m_initiative.begin();
            iter != m_initiative.end(); iter++)
    {
        Monster * pMonster = dynamic_cast<Monster*>(*iter);

        if (pMonster)
        {
            if (!pMonster->GetToggle(ICharacter::CA_ALIVE) && !pMonster->DeathAnimated())
            {
                pMonster->Die();
                death_animation(pMonster);
                pMonster->SetToggle(ICharacter::CA_VISIBLE,false);
            }
        }
        else
        {
            // Change PC sprite to dead one?
        }
    }
    // BRING OUT YOUR DEAD!
    // Ok. We don't do this because we need to leave the dead actually in the initiative, to do status effect rounds on them
    /*
    m_initiative.erase(std::remove_if(m_initiative.begin(), m_initiative.end(),
				      std::not1(std::bind2nd(std::mem_fun(&ICharacter::GetToggle),ICharacter::CA_ALIVE)))
				    ,m_initiative.end());
    */
}

void BattleState::death_animation(Monster* pMonster)
{
    pMonster->MarkDeathAnimated();
}

void BattleState::lose()
{
    m_bDone = true;


    ParameterList params;
    m_config->OnBattleLost(params);    
}

void BattleState::win()
{
    m_bDone = true;
    ParameterList params;
    // All battle methods remain valid here
    m_config->OnBattleWon(params);
    IParty * pParty = IApplication::GetInstance()->GetParty();
    
    // TODO: Maybe this should just be steel in OnBattleWon? 
    for(int i=0;i<pParty->GetCharacterCount(); i++)
        dynamic_cast<Character*>(pParty->GetCharacter(i))->RemoveBattleStatusEffects();
}


/***** BIFS *****/
SteelType BattleState::selectTargets(bool single, bool group, bool defaultMonsters)
{
    m_combat_state = TARGETING;
    SteelType::Container targets;
    TargetingState::Targetable targetable;
    if (single && group)
        targetable = TargetingState::EITHER;
    else if (group)
        targetable = TargetingState::GROUP;
    else
        targetable = TargetingState::SINGLE;

    m_targeting_state.Init(this,targetable,defaultMonsters);
    IApplication::GetInstance()->RunState(&m_targeting_state);


    if (m_targets.m_bSelectedGroup)
    {

        for (uint i=0;i<m_targets.selected.m_pGroup->GetCharacterCount();i++)
        {
            SteelType ref;
            ref.set(m_targets.selected.m_pGroup->GetCharacter(i));
            targets.push_back(ref);
        }

    }
    else
    {
        SteelType ref;
	if(m_targets.selected.m_pTarget){
	    ref.set(m_targets.selected.m_pTarget);
	    targets.push_back(ref);
	}
    }

    SteelType val;
    val.set(targets);
    return val;
}

SteelType BattleState::finishTurn()
{
    FinishTurn();
    return SteelType();
}
// if they back out and want to go back to the battle menu
SteelType BattleState::cancelOption()
{
    m_combat_state = BATTLE_MENU;
    return SteelType();
}

SteelType BattleState::darkMode(int nOrder)
{
    m_ndarkMode = nOrder;
    return SteelType();
}

SteelType BattleState::darkColor(double r, double g, double b, double a)
{
    m_darkColor = CL_Colorf((float)r,(float)g,(float)b,(float)a);
    return SteelType();
}


SteelType BattleState::doTargetedAnimation(SteelType::Handle pICharacter, SteelType::Handle pITarget,SteelType::Handle hAnim)
{
    ICharacter * character = GrabHandle<ICharacter*>(pICharacter);
    ICharacter * target = GrabHandle<ICharacter*>(pITarget);
    Animation * anim = GrabHandle<Animation*>(hAnim);
    if(anim == NULL)
    {
	throw TypeMismatch();
    }
    if(!target) throw TypeMismatch();
    if(!character) throw TypeMismatch();
    StoneRing::AnimationState state(*this, group_for_character(character), group_for_character(target), character, target);
    state.Init(anim);

    IApplication::GetInstance()->RunState (&state);
    
    return SteelType();
}




SteelType BattleState::doCharacterAnimation(SteelType::Handle pICharacter,SteelType::Handle hAnim)
{
	ICharacter * character = GrabHandle<ICharacter*>(pICharacter);
	Animation * anim = GrabHandle<Animation*>(hAnim);
	if(anim == NULL)
	{
		throw TypeMismatch();
	}
	if(!character) throw TypeMismatch();
	StoneRing::AnimationState state(*this, group_for_character(character), NULL, character, NULL);
	state.Init(anim);

	IApplication::GetInstance()->RunState (&state);
	
    return SteelType();
}

SteelType BattleState::createDisplay(int damage,SteelType::Handle hICharacter,int display_type)
{
    //            Display(BattleState& parent,eDisplayType type,int damage,SteelType::Handle pICharacter);
    ICharacter* iChar = GrabHandle<ICharacter*>(hICharacter);
    if(!iChar) throw TypeMismatch();
    Display display(*this, static_cast<Display::eDisplayType>(display_type),damage,iChar);
    display.start();
    m_displays.push_back(display);

    return SteelType();
}

SteelType BattleState::getCharacterGroup(bool monsters)
{
    SteelType::Container vector;
    ICharacterGroup * group;

    if (monsters)
    {
        group = m_monsters;
    }
    else
    {
        group = IApplication::GetInstance()->GetParty();
    }

    for (uint i=0;i<group->GetCharacterCount();i++)
    {
        SteelType handle;
        handle.set( group->GetCharacter(i) );
        vector.push_back(handle);
    }
    SteelType array;
    array.set(vector);
    return array;
}

SteelType BattleState::getAllCharacters()
{
    SteelType::Container vector;

    for (uint i=0;i<m_initiative.size();i++)
    {
        SteelType handle;
        handle.set ( m_initiative[i] );
        vector.push_back(handle);
    }

    SteelType array;
    array.set(vector);
    return array;
}

SteelType BattleState::getMonsterDamageCategory(SteelType::Handle hMonster)
{
    SteelType result;
    Monster * pMonster = GrabHandle<Monster*>(hMonster);
    if(!pMonster) throw TypeMismatch();

    result.set ( pMonster->GetDefaultDamageCategory() );

    return result;
}

SteelType BattleState::flee()
{
    m_bDoneAfterRound = true;
    return SteelType();
}

SteelType BattleState::isBossBattle()
{
    SteelType val;
    val.set(m_bBossBattle);
    
    return val;
}


