#include "ItemSelectState.h"
#include "GraphicsManager.h"
#include "Item.h"
#include "RegularItem.h"
#include "MenuBox.h"
#include "SoundManager.h"
#include "Party.h"
#include <iomanip>

using StoneRing::ItemSelectState;
using StoneRing::Item;
using StoneRing::Font;
using StoneRing::IApplication;
using std::abs;


class ItemCollector : public StoneRing::ItemVisitor {
public:
    ItemCollector( ItemSelectState& state ): m_state( state ) {
    }
    ~ItemCollector() {}
    void operator()( Item* pItem, int nCount ) {
        m_state.addItem( pItem, nCount );
    }
private:
    ItemSelectState &m_state;
};



void ItemSelectState::Init( bool battle, int type_mask ) {
    m_typemask = type_mask;
    m_battle = battle;
}

bool ItemSelectState::IsDone() const {
    return m_bDone;
}
// Handle joystick / key events that are processed according to mappings
void ItemSelectState::HandleButtonUp( const IApplication::Button& button ) {
    static const Item::eItemType order[] = {Item::REGULAR_ITEM, Item::WEAPON, Item::ARMOR, Item::OMEGA, Item::SPECIAL};
    int cur_index = 0;
    int order_count = sizeof( order ) / sizeof( Item::eItemType );
    for( int i = 0; i < order_count; i++ ) {
        if( m_itemType == order[i] ) {
            cur_index = i;
            break;
        }
    }
    switch( button ) {
    case IApplication::BUTTON_CANCEL:
        SoundManager::PlayEffect( SoundManager::EFFECT_CANCEL );
        m_bDone = true;
        break;
    case IApplication::BUTTON_CONFIRM:
        Choose();
        if( selection_applies( m_selected_item ) ) {
            SoundManager::PlayEffect( SoundManager::EFFECT_SELECT_OPTION );
            m_bDone = true;
        } else {
            // Play sound
            SoundManager::PlayEffect( SoundManager::EFFECT_BAD_OPTION );
        }
        break;
    case IApplication::BUTTON_R:
        if( cur_index == order_count - 1 ) {
            cur_index = 0;
        } else {
            cur_index++;
        }
        m_itemType = order[cur_index];
        reset_menu();
        break;
    case IApplication::BUTTON_L:
        if( cur_index == 0 ) {
            cur_index = order_count - 1;
        } else {
            cur_index--;
        }
        m_itemType = order[cur_index];
        reset_menu();
        break;

    }
}

void ItemSelectState::HandleButtonDown( const IApplication::Button& button ) {
    switch( button ) {
    }
}

void ItemSelectState::HandleAxisMove( const IApplication::Axis& axis, IApplication::AxisDirection dir, float pos ) {
    if( axis == IApplication::AXIS_VERTICAL ) {
        if( dir == IApplication::AXIS_UP ) {
            SelectUp();
        } else if( dir == IApplication::AXIS_DOWN ) {
            SelectDown();
        }
    } else {
        if( pos == 0.0f ) {
            if( m_eArrowState == ARROW_LEFT_DOWN ) {
                HandleButtonUp( IApplication::BUTTON_L );
            } else if( m_eArrowState == ARROW_RIGHT_DOWN ) {
                HandleButtonUp( IApplication::BUTTON_R );
            }
        }
        if( dir == IApplication::AXIS_LEFT ) {
            if( abs( pos ) == 1.0f )
                m_eArrowState = ARROW_LEFT_DOWN;

        } else if( dir == IApplication::AXIS_RIGHT ) {
            if( abs( pos ) == 1.0f )
                m_eArrowState = ARROW_RIGHT_DOWN;
        }


    }
}


void ItemSelectState::Draw( const clan::Rect &screenRect, clan::Canvas& GC ) {
//m_overlay.draw(GC,0.0f,0.0f);
    clan::Rectf bgRect = screenRect;
//bgRect.shrink(2);
    MenuBox::Draw( GC, bgRect );
    Menu::Draw( GC );
    float header_width = m_header_rect.get_width();
    float each_width = header_width / m_type_icons.size();

    int i = 0;
    for( std::map<Item::eItemType, clan::Image>::iterator iter = m_type_icons.begin();
         iter != m_type_icons.end(); iter++ ) {
        float offset = ( each_width - iter->second.get_width() ) / 2.0f;
        if( m_itemType == iter->first ) {
            iter->second.set_alpha( 1.0f );
        } else {
            iter->second.set_alpha( 0.5f );
        }
        iter->second.draw( GC, m_header_rect.left + i * each_width + offset, m_header_rect.top );
        ++i;
    }


}

bool ItemSelectState::LastToDraw() const { // Should we continue drawing more states?
    return true;
}

bool ItemSelectState::DisableMappableObjects() const { // Should the app move the MOs?
    return false;
}

void ItemSelectState::Update() { // Do stuff right after the mappable object movement
}

void ItemSelectState::Start() {
    m_items.clear();
    m_selected_item = NULL;
    m_eArrowState = ARROWS_IDLE;
    Menu::Init();
    m_bDone = false;
    m_header_rect = GraphicsManager::GetRect( GraphicsManager::ITEMS, "header" );
    m_rect = GraphicsManager::GetRect( GraphicsManager::ITEMS, "list" );
    m_text_rect = GraphicsManager::GetRect( GraphicsManager::ITEMS, "text" );


    m_optionFont = GraphicsManager::GetFont( GraphicsManager::ITEMS, "Option" );
    m_currentOptionFont = GraphicsManager::GetFont( GraphicsManager::ITEMS, "Selection" );
    m_unavailableOption = GraphicsManager::GetFont( GraphicsManager::ITEMS, "Unavailable" );
    m_descriptionFont = GraphicsManager::GetFont( GraphicsManager::ITEMS, "Description" );


    m_type_icons[ Item::REGULAR_ITEM ] = GraphicsManager::GetIcon( "regular_items" );
    m_type_icons[ Item::SPECIAL ] = GraphicsManager::GetIcon( "special_items" );

    m_type_icons [ Item::WEAPON ] = GraphicsManager::GetIcon( "weapons" );
    m_type_icons [ Item::ARMOR ] = GraphicsManager::GetIcon( "armor" );
    m_type_icons[ Item::OMEGA ] = GraphicsManager::GetIcon( "omegas" );

    static const Item::eItemType order[] = {Item::REGULAR_ITEM, Item::WEAPON, Item::ARMOR, Item::OMEGA, Item::SPECIAL};
    m_itemType = Item::REGULAR_ITEM;
    for( uint i = 0; i < sizeof( order ) / sizeof( Item::eItemType ); i++ ) {
        if( order[i] & m_typemask ) {
            m_itemType = order[i];
            break;
        }
    }


    Party * party = IApplication::GetInstance()->GetParty();
    ItemCollector collector( *this );

    party->IterateItems( collector );

}

bool ItemSelectState::selection_applies( Item *pItem ) {
    if( !pItem ) return false;
    if( pItem->GetItemType() & m_typemask == 0 ) return false;

    switch( pItem->GetItemType() ) {
    case Item::REGULAR_ITEM: {
        RegularItem* pRegularItem = dynamic_cast<RegularItem*>( pItem );
        if( m_battle && pRegularItem->GetUseType() == RegularItem::WORLD ||
            !m_battle && pRegularItem->GetUseType() == RegularItem::BATTLE )
            return false;
        break;
    }

    }
    return true;
}

void ItemSelectState::Finish() { // Hook to clean up or whatever after being popped
}

clan::Rectf ItemSelectState::get_rect() {
    return m_rect;
}

void ItemSelectState::draw_option( int option, bool selected, const clan::Rectf& rect, clan::Canvas& gc ) {
    const std::pair<Item*, int>& pair  = m_items[ m_itemType ][option];

    const float x = rect.get_top_left().x;
    const float y = rect.get_top_left().y;
    
    Item * pItem = pair.first;
    const int count = pair.second;
    bool available = selection_applies( pItem );
    clan::Image icon = pItem->GetIcon();
//icon.set_alignment(origin_top_left);
    if( available )
        icon.set_alpha( 1.0f );
    else
        icon.set_alpha( 0.5f );
    icon.draw( gc, x, y );
    const float icon_width = icon.get_width();
    std::ostringstream text;
    if( count > 1 )
        text << std::setw( 40 ) << std::left << pItem->GetName() << ' ' << std::setw( 3 ) << std::right << count;
    else
        text << std::setw( 40 ) << std::left << pItem->GetName();

    if( selected )
        m_currentOptionFont.draw_text( gc, x + icon_width + 12, y, text.str(), Font::TOP_LEFT );
    else {
        if( !available ) {
            m_unavailableOption.draw_text( gc, x + icon_width + 12, y, text.str(), Font::TOP_LEFT );
        } else {
            m_optionFont.draw_text( gc, x + icon_width + 12, y, text.str(), Font::TOP_LEFT );
        }
    }

    if( selected )
        draw_text( gc, m_descriptionFont, m_text_rect, pItem->GetDescription() );

}

int ItemSelectState::height_for_option( clan::Canvas& gc ) {
    return std::max( m_optionFont.get_font_metrics( gc ).get_height(), m_currentOptionFont.get_font_metrics( gc ).get_height() );
}

void ItemSelectState::process_choice( int selection ) {
    if( !m_items.empty() && m_items[m_itemType].size() > selection ) {
        m_selected_item = m_items[m_itemType][selection].first;
    }
}

int ItemSelectState::get_option_count() {
    return m_items [ m_itemType ].size();
}

bool ItemSelectState::roll_over() {
    return false;
}


void ItemSelectState::draw_categories() {

}


void ItemSelectState::addItem( Item* pItem, int count ) {
    m_items [ pItem->GetItemType() ].push_back( std::pair<Item*, int>( pItem, count ) );
}
