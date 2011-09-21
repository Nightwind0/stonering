#include <cassert>
#include "Level.h"
#include "GraphicsManager.h"
#include "Graphic.h"
#include "SpriteRef.h"
#include "IParty.h"
#include <ClanLib/core.h>


using StoneRing::IParty;


CL_Vec2<int> StoneRing::MappableObject::DirectionToVector ( MappableObject::eDirection dir )
{
    switch(dir){
        case EAST:
            return CL_Vec2<int>(CL_Point(1,0));
        case WEST:
            return CL_Vec2<int>(CL_Point(-1,0));
        case NORTH:
            return CL_Vec2<int>(CL_Point(0,-1));
        case SOUTH:
            return CL_Vec2<int>(CL_Point(0,1));
        default:
            return CL_Vec2<int>(CL_Point(0,0));
    }
}


bool StoneRing::MappableObject::EvaluateCondition() const
{
    if(m_pCondition)
    {
        return m_pCondition->EvaluateCondition();
    }

    return true;
}

StoneRing::MappableObject::eSize StoneRing::MappableObject::GetSize() const
{
    return m_eSize;
}


bool StoneRing::MappableObject::Single_Move ( StoneRing::Level& level )
{
    switch(m_eDirection)
    {
    case NORTH:
        m_Y--;
        break;
    case SOUTH:
        m_Y++;
        break;
    case EAST:
        m_X++;
        break;
    case WEST:
        m_X--;
        break;
    default:
        break;
    };

    return true;
}

void StoneRing::MappableObject::Stop_Movement()
{

}



void StoneRing::MappableObject::Move(Level& level)
{
    for(int i=0;i<Get_Moves_Per_Draw(); i++){
        if(IsAligned()){
            Moved_One_Cell();
            CL_Rect rect = GetTileRect();
            rect.translate ( DirectionToVector(m_eDirection) );
            
            while(m_eDirection != NONE && !level.Move(this,GetTileRect(),rect)){
                Random_New_Direction();
                CL_Rect rect = GetTileRect();
                rect.translate ( DirectionToVector(m_eDirection) );            
            }

        }
        

            
        if(m_pMovement){ 
            Single_Move(level);
        }
    }

}

void StoneRing::MappableObject::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
    std::string motype = get_required_string("type",attributes);
    std::string size = get_required_string("size",attributes);

    m_StartX= get_required_int("xpos",attributes);
    m_StartY = get_required_int("ypos",attributes);

    m_X = m_StartX *32;
    m_Y = m_StartY *32;

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

bool StoneRing::MappableObject::handle_element(Element::eElement element, Element * pElement)
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
        switch(m_pMovement->GetMovementType())
        {
        case Movement::MOVEMENT_WANDER:
            if(m_graphic.asSpriteRef->GetType() != SpriteRef::SPR_FOUR_WAY)
                throw CL_Exception("Wandering MO needs a four way sprite ref.");
            m_eDirection = SOUTH;
            break;
        case Movement::MOVEMENT_PACE_NS:
            m_eDirection = SOUTH;
            if(m_graphic.asSpriteRef->GetType() != SpriteRef::SPR_TWO_WAY)
                throw CL_Exception("Pacing MO needs a two way sprite ref.");
            break;
        case Movement::MOVEMENT_PACE_EW:
            if(m_graphic.asSpriteRef->GetType() != SpriteRef::SPR_TWO_WAY)
                throw CL_Exception("Pacing MO needs a two way sprite ref.");
            m_eDirection = EAST;
            break;
        default:
            break;
        }

        break;
    }
    default:
        return false;
    }

    return true;
}

void StoneRing::MappableObject::load_finished()
{
    CL_Size dimensions = Calc_Tile_Dimensions();
    m_nWidth = dimensions.width;
    m_nHeight = dimensions.height;
}

StoneRing::MappableObject::MappableObject():m_eDirection(NONE),m_nStep(0),m_pMovement(NULL),
                                            m_pCondition(0),cFlags(0),
                                            m_nStepsUntilChange(0),m_nTilesMoved(0)
{
}

StoneRing::MappableObject::~MappableObject()
{
    for( std::list<Event*>::iterator i = m_events.begin();
         i != m_events.end();
         i++)
    {
        delete *i;
    }

    delete m_pMovement;
}



std::string StoneRing::MappableObject::GetName() const
{
    return m_name;
}


CL_Size StoneRing::MappableObject::Calc_Tile_Dimensions() const
{
    uint width,height = 0;
    int movement_x, movement_y;
    movement_x = movement_y = 0;
    
    if(m_X % 32 != 0)
        movement_x = 1;
    if(m_Y % 32 != 0)
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

CL_Rect StoneRing::MappableObject::GetSpriteRect() const
{
    CL_Rect pixelRect;
    CL_Point myDimensions(m_nWidth,m_nHeight);
    myDimensions.x *= 32;
    myDimensions.y *= 32;

    if(IsSprite())
    {
        pixelRect.top = m_Y - (m_sprite.get_height() - myDimensions.y);
        pixelRect.left = m_X - (m_sprite.get_width() - myDimensions.x);
        pixelRect.right = pixelRect.left + m_sprite.get_width();
        pixelRect.bottom = pixelRect.top + m_sprite.get_height();

        return pixelRect;
    }
    else if (cFlags & TILEMAP)
    {
        return CL_Rect(m_X*32,m_Y*32, m_X*32 + 32, m_Y *32 +32);
    }
    else
    {
        return CL_Rect(m_X*32,m_Y*32,m_X * 32 + myDimensions.x, m_Y* 32 + myDimensions.y);
    }
}

bool StoneRing::MappableObject::IsSprite() const
{
    return cFlags & SPRITE;
}

void StoneRing::MappableObject::Draw(CL_GraphicContext& GC, const CL_Point& offset)
{
    CL_Rect dstRect = GetSpriteRect();
    dstRect.translate(offset);
    if(IsSprite())
    {
        Set_Frame_For_Direction();
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

void StoneRing::MappableObject::Pick_Opposite_Direction()
{
    switch(m_eDirection)
    {
    case WEST:
        m_eDirection = EAST;
        break;
    case EAST:
        m_eDirection = WEST;
        break;
    case NORTH:
        m_eDirection = SOUTH;
        break;
    case SOUTH:
        m_eDirection = NORTH;
        break;
    default:
        break;
    }

}


void StoneRing::MappableObject::Random_New_Direction()
{
    if(!m_pMovement) return;

    eDirection current = m_eDirection;

    while(m_eDirection == current)
    {
        int r= rand() % 5;

        switch ( m_pMovement->GetMovementType())
        {
        case Movement::MOVEMENT_NONE:
            break;
        case Movement::MOVEMENT_WANDER:
            if(r == 0)
                m_eDirection = NORTH;
            else if(r == 1)
                m_eDirection = SOUTH;
            else if(r == 2)
                m_eDirection = EAST;
            else if(r == 3)
                m_eDirection = WEST;
            else if(r == 4)
                m_eDirection = NONE;
            break;
        case Movement::MOVEMENT_PACE_NS:
        case Movement::MOVEMENT_PACE_EW:
            if(r > 2)
                Pick_Opposite_Direction();
            break;
        default:
            break;

        }
    }

    m_nStepsUntilChange = rand() % 20;
    m_nTilesMoved = 0;

    Set_Frame_For_Direction();
}

void StoneRing::MappableObject::Idle()
{
    m_nStep = 0; // Set to what will be idle if the sprite has it.
    Set_Frame_For_Direction();
}

void StoneRing::MappableObject::Set_Frame_For_Direction()
{
    if(m_sprite.is_null()) return;

    if(!m_pMovement) m_sprite.set_frame(0);
    else if (!m_pMovement) return;

    switch(m_pMovement->GetMovementType())
    {
    case Movement::MOVEMENT_NONE:
        m_sprite.set_frame(m_nStep);

        break;
    case Movement::MOVEMENT_WANDER:
    {
        switch( m_eDirection ) //SWEN
        {
        case NORTH:
            m_sprite.set_frame(12 + m_nStep);
            break;
        case EAST:
            m_sprite.set_frame(8 + m_nStep);
            break;
        case WEST:
            m_sprite.set_frame(4 + m_nStep);
            break;
        case SOUTH:
            m_sprite.set_frame(m_nStep);
            break;
        case NONE:
            break;
        }
        break;
    }
    case Movement::MOVEMENT_PACE_NS:
    {
        switch(m_eDirection)
        {
        case NORTH:
            m_sprite.set_frame ( 4 + m_nStep );
            break;
        case SOUTH:
            m_sprite.set_frame ( m_nStep);
            break;
        default:
            assert(0);
        }
        break;
    }
    case Movement::MOVEMENT_PACE_EW:
    {
        switch(m_eDirection)
        {
        case EAST:
            m_sprite.set_frame ( 4 + m_nStep  );
            break;
        case WEST:
            m_sprite.set_frame ( m_nStep );
            break;
        default:
            assert(0);
        }
        break;
    }
    default:
        break;
    }
}


void StoneRing::MappableObject::Update()
{
    // Don't bother with disabled MOs
    if(!EvaluateCondition()) return;

    if(IsSprite())
    {
       if(++m_nStep > 3)
	   m_nStep = 0;
    }
}


bool StoneRing::MappableObject::IsSolid() const
{
    return cFlags & StoneRing::MappableObject::SOLID;
}

int StoneRing::MappableObject::GetDirectionBlock() const
{
    if (IsSolid()) return  (DIR_NORTH | DIR_SOUTH | DIR_EAST | DIR_WEST);
    else return 0;
}

bool StoneRing::MappableObject::IsTile() const
{
    return false;
}

void StoneRing::MappableObject::Prod()
{
    Random_New_Direction();
}


bool StoneRing::MappableObject::ProvokeEvents ( Event::eTriggerType trigger )
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
            pEvent->Invoke();
            provoked = true;
        }

    }
    
    return provoked;
}

int StoneRing::MappableObject::ConvertDirectionToDirectionBlock(eDirection dir)
{
    switch(dir)
    {
    case NORTH:
        return DIR_SOUTH;
    case SOUTH:
        return DIR_NORTH;
    case EAST:
        return DIR_WEST;
    case WEST:
        return DIR_EAST;
    case NONE:
        return 0;

    }

    assert( 0 );
    return 0;
}

void StoneRing::MappableObject::CalculateEdgePoints(const CL_Point &topleft, eDirection dir, eSize size, std::list<CL_Point> *pList)
{
    uint points = 0;
    CL_Size dimensions = Calc_Tile_Dimensions();
    pList->clear();

    switch(dir)
    {
    case NONE:
        return;
    case NORTH:
        points = dimensions.width;
        for(uint i=0;i<points;i++)
        {
            pList->push_back ( CL_Point(topleft.x+i,topleft.y ));
        }

        return;
    case SOUTH:
        points = dimensions.width;
        for(uint i=0;i<points;i++)
        {
            pList->push_back ( CL_Point(topleft.x+i,topleft.y + (dimensions.height-1) ));
        }

        return;
    case EAST:
        points = Calc_Tile_Dimensions().height;
        for(uint i=0;i<points;i++)
        {
            pList->push_back ( CL_Point(topleft.x+ (dimensions.width-1),topleft.y + i ));
        }

        break;
    case WEST:
        points = Calc_Tile_Dimensions().height;
        for(uint i=0;i<points;i++)
        {
            pList->push_back ( CL_Point(topleft.x,topleft.y + i ));
        }
        break;

    }
}


#if 0
StoneRing::MappableObject::eDirection StoneRing::MappableObject::OppositeDirection(eDirection current_dir)
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

void StoneRing::MappableObject::Moved_One_Cell()
{
    if(++m_nTilesMoved)
    {
        if(m_nTilesMoved == m_nStepsUntilChange) ///@todo: get from somewhere
        {
            Random_New_Direction();
        }
    }

}
bool StoneRing::MappableObject::IsAligned() const
{
    return (m_X % 32 == 0 )&& (m_Y  %32 == 0);
}

uint StoneRing::MappableObject::Get_Moves_Per_Draw() const
{
    if(m_pMovement)
    {
        switch( m_pMovement->GetMovementSpeed() )
        {
        case Movement::SLOW:
            return 1;
        case Movement::MEDIUM:
            return 2;
        case Movement::FAST:
            return 3;
        default:
            return 0;
        }
    }
    else return 0;
}
StoneRing::MappableObject::eDirection StoneRing::MappableObject::OppositeDirection ( StoneRing::MappableObject::eDirection dir )
{
    switch(dir){
        case EAST:
            return WEST;
        case WEST:
            return EAST;
        case NORTH:
            return SOUTH;
        case SOUTH:
            return NORTH;
        default:
            return dir;
    }
}



CL_Rect StoneRing::MappableObject::GetTileRect() const 
{
    CL_Size size = Calc_Tile_Dimensions();
    CL_Point position = GetPosition();
    if(!IsAligned()){
        position += DirectionToVector(m_eDirection);
    }
    return CL_Rect(position,size);
}

StoneRing::MappablePlayer::MappablePlayer(uint startX, uint startY):m_bHasNextDirection(false)
{
    m_eSize = MO_SMALL;
    m_StartX = startX;
    m_StartY = startY;
    m_X=startX * 32;
    m_Y=startY * 32;
    m_name = "Player";
    m_eType = PLAYER;
    m_eDirection = NONE;
    m_eFacingDirection = SOUTH; // Should come in like the startX, startY
    m_nHeight  =1;
    m_nWidth = 1;
    m_pMovement = new PlayerMovement();
}

StoneRing::MappablePlayer::~MappablePlayer()
{
}

uint StoneRing::MappablePlayer::Get_Moves_Per_Draw() const
{
    if(m_bRunning) return 6;
    else return 3;
}





void StoneRing::MappablePlayer::SetNextDirection(eDirection newDir)
{
    m_bHasNextDirection = true;
    m_eNextDirection = newDir;
}

void StoneRing::MappablePlayer::ClearNextDirection ()
{ 
    m_bHasNextDirection = false; 
    m_eDirection = NONE; 
}

void StoneRing::MappablePlayer::Random_New_Direction()
{
    m_eDirection = NONE;
    m_bHasNextDirection = false;
}
void StoneRing::MappablePlayer::Idle()
{
    if(m_bHasNextDirection)
    {
        m_eDirection = m_eNextDirection;
        m_eFacingDirection = m_eDirection;
       // m_bHasNextDirection = false;
	set_frame_for_direction();
    }
    else
    {
        m_eDirection = NONE;
    }
}

void StoneRing::MappablePlayer::StopMovement()
{
    m_nStep = 0;
    Set_Frame_For_Direction();
    m_bHasNextDirection = false;
   // Update();
}

void StoneRing::MappablePlayer::Moved_One_Cell()
{
    Idle();
    Update();
}

void StoneRing::MappablePlayer::SetRunning(bool running)
{
    m_bRunning = running;
}



CL_Point StoneRing::MappablePlayer::GetPointInFront() const
{
    CL_Point point = GetPosition();

    switch(m_eFacingDirection)
    {
    case NORTH:
        point.y--;
        break;
    case SOUTH:
        point.y++;
        break;
    case EAST:
        point.x++;
        break;
    case WEST:
        point.x--;
        break;
    default: break;

    }

    return point;
}

void StoneRing::MappablePlayer::set_frame_for_direction()
{
    switch( m_eFacingDirection ) // SWEN
    {
    case NORTH:
        m_sprite.set_frame(12 + m_nStep);
        break;
    case EAST:
        m_sprite.set_frame(8 + m_nStep);
        break;
    case WEST:
        m_sprite.set_frame(4 + m_nStep);
        break;
    case SOUTH:
        m_sprite.set_frame(m_nStep);
        break;
    case NONE:
        m_sprite.set_frame(0);
        break;
    }
}

void StoneRing::MappablePlayer::SerializeState ( std::ostream& out )
{
    out.write((char*)&m_X,sizeof(m_X));
    out.write((char*)&m_Y,sizeof(m_Y));
    out.write((char*)&m_eDirection,sizeof(eDirection));
    out.write((char*)&m_eFacingDirection,sizeof(m_eFacingDirection));
    out.write((char*)&m_eNextDirection,sizeof(m_eNextDirection));
    out.write((char*)&m_bHasNextDirection,sizeof(bool));
}

void StoneRing::MappablePlayer::DeserializeState ( std::istream& in )
{
    in.read((char*)&m_X,sizeof(m_X));
    in.read((char*)&m_Y,sizeof(m_Y));
    in.read((char*)&m_eDirection,sizeof(eDirection));
    in.read((char*)&m_eFacingDirection,sizeof(m_eFacingDirection));
    in.read((char*)&m_eNextDirection,sizeof(m_eNextDirection));
    in.read((char*)&m_bHasNextDirection,sizeof(bool));
}

StoneRing::MappableObject::eDirection StoneRing::MappablePlayer::GetDirection()
{
    return m_eDirection;
}






