#include <cassert>
#include "Level.h"
#include "GraphicsManager.h"
#include "Graphic.h"
#include "SpriteRef.h"
#include "IParty.h"
#include <ClanLib/core.h>




namespace StoneRing { 


bool MappableObject::EvaluateCondition() const
{
    if(m_pCondition)
    {
        return m_pCondition->EvaluateCondition();
    }

    return true;
}

MappableObject::eSize MappableObject::GetSize() const
{
    return m_eSize;
}

void MappableObject::SetPixelPosition ( const CL_Point& pixel_pos )
{
    m_pos = pixel_pos;
}

void MappableObject::Move(Level& level)
{
    if(!m_navStack.empty()){
        m_navStack.top()->MoveObject(*this,level);
    }
}

void MappableObject::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
    std::string motype = get_required_string("type",attributes);
    std::string size = get_required_string("size",attributes);

    m_StartX= get_required_int("xpos",attributes);
    m_StartY = get_required_int("ypos",attributes);

    m_pos.x = m_StartX *32;
    m_pos.y = m_StartY *32;

    if(size == "small") m_eSize = MO_SMALL;
    else if(size == "medium") m_eSize = MO_MEDIUM;
    else if(size == "large") m_eSize = MO_LARGE;
    else if(size == "wide") m_eSize = MO_WIDE;
    else if(size == "tall") m_eSize = MO_TALL;
    else throw CL_Exception("MO size wasnt small, medium, or large.");

    if(motype == "npc") m_eType = NPC;
    else if (motype == "square") m_eType = SQUARE;
    else if (motype == "container") m_eType = CONTAINER;
    else if (motype == "door") m_eType = DOOR;
    else if (motype == "warp") m_eType = WARP;

    bool solid = get_implied_bool("solid",attributes,false);

    if(solid) cFlags |= SOLID;

}

bool MappableObject::handle_element(Element::eElement element, Element * pElement)
{
    switch(element)
    {
    case ETILEMAP:
    {
        if( m_eSize != MO_SMALL) throw CL_Exception("Mappable objects using tilemaps MUST be size small.");
        cFlags |= TILEMAP;
        m_graphic.asTilemap = dynamic_cast<Tilemap*>(pElement);
        break;
    }
    case ESPRITEREF:
    {
        SpriteRef * pRef = dynamic_cast<SpriteRef*>(pElement);

        if(pRef->GetType() >= SpriteRef::_END_MO_TYPES)
        {
            throw CL_Exception("spriteRef on mappable object must have direction type, not battle type");
        }

        m_graphic.asSpriteRef = pRef;
        cFlags |= SPRITE;

        m_sprite = GraphicsManager::CreateSprite ( pRef->GetRef() );

        int swidth = m_sprite.get_width();
        int sheight = m_sprite.get_height();

        switch( m_eSize )
        {
        case MO_SMALL:
            if( swidth != 32 && sheight !=32) throw CL_Exception("Sprite size doesn't match MO size (SMALL)");
            break;
        case MO_MEDIUM:
            if( swidth != 64 && sheight != 64) throw CL_Exception("Sprite size doesn't match MO size (MEDIUM).");
            break;
        case MO_LARGE:
            if( swidth != 128 && sheight != 128) throw CL_Exception("Sprite size doesnt match MO size (LARGE)");
            break;
        case MO_TALL:
            if( swidth != 32 && sheight != 64) throw CL_Exception("Sprite size does not match MO size(TALL)");
            break;
        case MO_WIDE:
            if( swidth != 64 && sheight != 32) throw CL_Exception("Sprite size does not match MO size(TALL)");
            break;
        }

        break;
    }
    case ECONDITIONSCRIPT:
        m_pCondition = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EEVENT:
        m_events.push_back ( dynamic_cast<Event*>(pElement));
        break;
    case EMOVEMENT:
    {
        m_pMovement = dynamic_cast<Movement*>(pElement);
        // Make sure the proper sprites are around for the movement type
        PushNavigator(new NPCNavigator());
        Prod(); // Get it going
        break;
    }
    default:
        return false;
    }

    return true;
}

void MappableObject::load_finished()
{
    CL_Size dimensions = Calc_Tile_Dimensions();
    m_nWidth = dimensions.width;
    m_nHeight = dimensions.height;
}

MappableObject::MappableObject():m_nStep(0),m_pMovement(NULL),
                                            m_pCondition(0),cFlags(0),
                                            m_nTilesMoved(0)
{
}

MappableObject::~MappableObject()
{
    for( std::list<Event*>::iterator i = m_events.begin();
         i != m_events.end();
         i++)
    {
        delete *i;
    }

    delete m_pMovement;
}



std::string MappableObject::GetName() const
{
    return m_name;
}


CL_Size MappableObject::Calc_Tile_Dimensions() const
{
    uint width,height = 0;
    int movement_x, movement_y;
    movement_x = movement_y = 0;
    
    if(m_pos.x % 32 != 0)
        movement_x = 1;
    if(m_pos.y % 32 != 0)
        movement_y = 1;

    switch ( m_eSize )
    {
    case MO_SMALL:
        width = height = 1;
        break;
    case MO_MEDIUM:
        width = height = 2;
        break;
    case MO_LARGE:
        width = height = 4;
        break;
    case MO_TALL:
        width = 1;
        height = 2;
        break;
    case MO_WIDE:
        width = 2;
        height = 1;
        break;
    }
    return CL_Size(width+movement_x,height+movement_y);
}

CL_Rect MappableObject::GetSpriteRect() const
{
    CL_Rect pixelRect;
    CL_Point myDimensions(m_nWidth,m_nHeight);
    myDimensions.x *= 32;
    myDimensions.y *= 32;

    if(IsSprite())
    {
        pixelRect.top = m_pos.y - (m_sprite.get_height() - myDimensions.y);
        pixelRect.left = m_pos.x - (m_sprite.get_width() - myDimensions.x);
        pixelRect.right = pixelRect.left + m_sprite.get_width();
        pixelRect.bottom = pixelRect.top + m_sprite.get_height();

        return pixelRect;
    }
    else if (cFlags & TILEMAP)
    {
        return CL_Rect(m_pos.x*32,m_pos.y*32, m_pos.x*32 + 32, m_pos.y *32 +32);
    }
    else
    {
        return CL_Rect(m_pos.x*32,m_pos.y*32,m_pos.x * 32 + myDimensions.x, m_pos.y* 32 + myDimensions.y);
    }
}

bool MappableObject::IsSprite() const
{
    return cFlags & SPRITE;
}

void MappableObject::Draw(CL_GraphicContext& GC, const CL_Point& offset)
{
    CL_Rect dstRect = GetSpriteRect();
    dstRect.translate(offset);
    if(IsSprite())
    {
        m_sprite.draw(GC,dstRect);
    }
    else if( cFlags & TILEMAP )
    {
#ifndef NDEBUG
        std::cout << "Mappable Object is tilemap?" << std::endl;
#endif
        CL_Rect srcRect(m_graphic.asTilemap->GetMapX() * 32, m_graphic.asTilemap->GetMapY() * 32,
                        (m_graphic.asTilemap->GetMapX() * 32), (m_graphic.asTilemap->GetMapY() * 32));

        m_graphic.asTilemap->GetTileMap().draw(GC,srcRect, dstRect);
    }
}




void MappableObject::Set_Frame_For_Direction()
{
    if(m_sprite.is_null()) return;
    if(!m_pMovement) 
        m_sprite.set_frame(0);

    switch(m_pMovement->GetMovementType())
    {
    case Movement::MOVEMENT_NONE:
        
        m_sprite.set_frame(0);

        break;
    case Movement::MOVEMENT_WANDER:
    {
        int step = m_nStep % 4;
        if(m_eFacingDirection == Direction::NORTH)
            m_sprite.set_frame(12 + step);
        else if(m_eFacingDirection == Direction::EAST)
            m_sprite.set_frame(8 + step);            
        else if(m_eFacingDirection == Direction::WEST)
            m_sprite.set_frame(4 + step);
        else if(m_eFacingDirection == Direction::SOUTH)
            m_sprite.set_frame(step);
        else assert(0);    
        break;
    }
    case Movement::MOVEMENT_PACE_NS:
    {
        int step = m_nStep % 4;
        if(m_eFacingDirection == Direction::NORTH)
            m_sprite.set_frame ( 4 + step );
        else if(m_eFacingDirection == Direction::SOUTH)
            m_sprite.set_frame ( step);
        else assert(0);
        break;
    }
    case Movement::MOVEMENT_PACE_EW:
    {
        int step = m_nStep % 4;
        if(m_eFacingDirection == Direction::EAST)
            m_sprite.set_frame ( 4 + step  );    
        else if(m_eFacingDirection == Direction::WEST)
            m_sprite.set_frame ( step );
        else assert(0);
        break;
    }
    }

    
}


void MappableObject::Update()
{

}


bool MappableObject::IsSolid() const
{
    return cFlags & MappableObject::SOLID;
}

int MappableObject::GetDirectionBlock() const
{
    if (IsSolid()) return  (BLK_NORTH | BLK_SOUTH | BLK_EAST | BLK_WEST);
    else return 0;
}

bool MappableObject::IsTile() const
{
    return false;
}

void MappableObject::Prod()
{
    if(!m_navStack.empty())
        m_navStack.top()->Prod(*this);
}


bool MappableObject::ProvokeEvents ( Event::eTriggerType trigger )
{
    bool provoked = false;
    IParty *party = IApplication::GetInstance()->GetParty();

    for(std::list<Event*>::iterator i = m_events.begin();
        i != m_events.end();
        i++)
    {
        Event * pEvent = *i;

        // If this is the correct trigger,
        // And the event is either repeatable or
        // Hasn't been done yet, invoke
        if( pEvent->GetTriggerType() == trigger
            && (pEvent->Repeatable() || !party->DidEvent ( pEvent->GetName() ))
            )
        {
            ParameterList params;
            params.push_back(ParameterListItem("$_Name",m_name));
            pEvent->Invoke(params);
            provoked = true;
        }

    }
    
    return provoked;
}

int MappableObject::ConvertDirectionToDirectionBlock(Direction dir)
{
    if(dir == Direction::NORTH)
        return BLK_SOUTH;
    else if(dir == Direction::SOUTH)
        return BLK_NORTH;
    else if(dir == Direction::EAST)
        return BLK_WEST;
    else if(dir == Direction::WEST)
        return BLK_EAST;
    else if(dir == Direction::NONE)
        return 0;
    else assert(0);

}

void MappableObject::CalculateEdgePoints(const CL_Point &topleft, Direction dir, eSize size, std::list<CL_Point> *pList)
{
    uint points = 0;
    CL_Size dimensions = Calc_Tile_Dimensions();
    pList->clear();

    if(dir == Direction::NORTH){
        points = dimensions.width;
        for(uint i=0;i<points;i++)
        {
            pList->push_back ( CL_Point(topleft.x+i,topleft.y ));
        }
    }else if(dir == Direction::SOUTH){
        points = dimensions.width;
        for(uint i=0;i<points;i++)
        {
            pList->push_back ( CL_Point(topleft.x+i,topleft.y + (dimensions.height-1) ));
        }
    }else if(dir == Direction::EAST){
        points = Calc_Tile_Dimensions().height;
        for(uint i=0;i<points;i++)
        {
            pList->push_back ( CL_Point(topleft.x+ (dimensions.width-1),topleft.y + i ));
        }
    }else if(dir == Direction::WEST){
        points = Calc_Tile_Dimensions().height;
        for(uint i=0;i<points;i++)
        {
            pList->push_back ( CL_Point(topleft.x,topleft.y + i ));
        }       
    }
}


#if 0
MappableObject::eDirection MappableObject::OppositeDirection(eDirection current_dir)
{
    switch( current_dir )
    {
    case NORTH:
        return SOUTH;
    case SOUTH:
        return NORTH;
    case EAST:
        return WEST;
    case WEST:
        return EAST;
    default:
        return current_dir;
    }
}
#endif

void MappableObject::Moved_One_Cell()
{

}
bool MappableObject::IsAligned() const
{
    return (m_pos.x % 32 == 0 ) && (m_pos.y  %32 == 0);
}


CL_Rect MappableObject::GetTileRect() const 
{
    CL_Size size = Calc_Tile_Dimensions();
    CL_Point position = GetPosition();
    if(!IsAligned()){
     //  position += m_pNavigator->GetCurrentDirection().ToScreenVector();
    }
    return CL_Rect(position,size);
}

Navigator* MappableObject::PopNavigator()
{
    if(!m_navStack.empty()){
        Navigator * pNav = m_navStack.top();
        m_navStack.pop();
        return pNav;
    }else{
        return NULL;
    }
}



MappablePlayer::MappablePlayer(uint startX, uint startY)
{
    m_eSize = MO_SMALL;
    m_StartX = startX;
    m_StartY = startY;
    m_pos.x=startX * 32;
    m_pos.y=startY * 32;
    m_name = "Player";
    m_eType = PLAYER;
    m_eFacingDirection = Direction::SOUTH; // Should come in like the startX, startY
    m_nHeight  =1;
    m_nWidth = 1;
}

MappablePlayer::~MappablePlayer()
{
}


void MappablePlayer::StopMovement()
{
    Stop();
    if(!m_navStack.empty())
        m_navStack.top()->Idle(*this);
   // Update();
}




CL_Point MappablePlayer::GetPointInFront() const
{
    CL_Point point = GetPosition();

    if(m_eFacingDirection == Direction::NORTH)
        point.y--;
    else if(m_eFacingDirection == Direction::SOUTH)
        point.y++;
    else if(m_eFacingDirection == Direction::EAST)
        point.x++;
    else if(m_eFacingDirection == Direction::WEST)
        point.x--;

    return point;
}

void MappablePlayer::Set_Frame_For_Direction()
{
    if(m_eFacingDirection == Direction::NONE)
        m_nStep = 0;
    
    if(m_eFacingDirection ==  Direction::NORTH)
        m_sprite.set_frame(12 + (m_nStep / 2)  % 4);
    else if(m_eFacingDirection == Direction::EAST)
        m_sprite.set_frame(8 + (m_nStep/2) % 4);
    else if(m_eFacingDirection == Direction::WEST)
        m_sprite.set_frame(4 + (m_nStep/2) % 4);
    else if(m_eFacingDirection == Direction::SOUTH)
        m_sprite.set_frame((m_nStep/2) % 4);

}

void MappablePlayer::SerializeState ( std::ostream& out )
{
    out.write((char*)&m_pos,sizeof(m_pos));
    out.write((char*)&m_eFacingDirection,sizeof(m_eFacingDirection));
}

void MappablePlayer::DeserializeState ( std::istream& in )
{
    in.read((char*)&m_pos,sizeof(m_pos));
    in.read((char*)&m_eFacingDirection,sizeof(m_eFacingDirection));

}


} // namespace StoneRing




