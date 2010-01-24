#include <cassert>
#include "Level.h"
#include "GraphicsManager.h"
#include "Graphic.h"
#include "SpriteRef.h"
#include "IParty.h"

class StoneRing::Tilemap;
class StoneRing::Element;
class StoneRing::Level;

using StoneRing::IParty;

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

void StoneRing::MappableObject::Move()
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
        GraphicsManager *GM = GraphicsManager::GetInstance();
        SpriteRef * pRef = dynamic_cast<SpriteRef*>(pElement);

        if(pRef->GetType() >= SpriteRef::_END_MO_TYPES)
        {
            throw CL_Exception("spriteRef on mappable object must have direction type, not battle type");
        }

        m_graphic.asSpriteRef = pRef;
        cFlags |= SPRITE;

        m_sprite = GM->CreateSprite ( pRef->GetRef() );

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
    Set_Frame_For_Direction();
}

StoneRing::MappableObject::MappableObject():m_eDirection(NONE),m_pMovement(NULL),
                                            m_pCondition(0),cFlags(0),m_nCellsMoved(0),
                                            m_nFrameMarks(0),m_nStepsUntilChange(0)
{
}


uint StoneRing::MappableObject::GetCellHeight() const
{
    return calcCellDimensions(m_eSize).x;
}

uint StoneRing::MappableObject::GetCellWidth() const
{
    return calcCellDimensions(m_eSize).y;
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




StoneRing::Movement* StoneRing::MappableObject::GetMovement() const
{
    return m_pMovement;
}




std::string StoneRing::MappableObject::GetName() const
{
    return m_name;
}


CL_Point StoneRing::MappableObject::calcCellDimensions(eSize size)
{
    uint width,height = 0;

    switch ( size )
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
    return CL_Point(width,height);
}

CL_Rect StoneRing::MappableObject::GetPixelRect() const
{
    CL_Rect pixelRect;
    CL_Point myDimensions = calcCellDimensions(m_eSize);
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

void StoneRing::MappableObject::Draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext& GC)
{
    if(IsSprite())
    {
        Set_Frame_For_Direction();
        m_sprite.draw(GC,dst);
    }
    else if( cFlags & TILEMAP )
    {
#ifndef NDEBUG
        std::cout << "Mappable Object is tilemap?" << std::endl;
#endif

        CL_Rect srcRect(m_graphic.asTilemap->GetMapX() * 32 + src.left, m_graphic.asTilemap->GetMapY() * 32 + src.top,
                        (m_graphic.asTilemap->GetMapX() * 32) + src.right, (m_graphic.asTilemap->GetMapY() * 32) + src.bottom);

        m_graphic.asTilemap->GetTileMap().draw(GC,srcRect, dst);
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


void StoneRing::MappableObject::RandomNewDirection()
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

    m_nCellsMoved = 0;
    m_nStepsUntilChange = rand() % 20;

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
        if(m_bStep) m_sprite.set_frame(1);
        else m_sprite.set_frame(0);

        break;
    case Movement::MOVEMENT_WANDER:
    {
        switch( m_eDirection )
        {
        case NORTH:
            m_sprite.set_frame(m_bStep? 6 : 7);
            break;
        case EAST:
            m_sprite.set_frame(m_bStep? 0 : 1);
            break;
        case WEST:
            m_sprite.set_frame(m_bStep? 2 : 3);
            break;
        case SOUTH:
            m_sprite.set_frame(m_bStep? 4 : 5);
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
            m_sprite.set_frame ( m_bStep? 2 : 3);
            break;
        case SOUTH:
            m_sprite.set_frame ( m_bStep? 0 : 1);
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
            m_sprite.set_frame ( m_bStep? 0 : 1 );
            break;
        case WEST:
            m_sprite.set_frame ( m_bStep? 2 : 3 );
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
        m_bStep = m_bStep? false:true;
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
    RandomNewDirection();
}



void StoneRing::MappableObject::SetOccupiedPoints(Level * pLevel,LevelPointMethod method)
{
    CL_Point dimensions = calcCellDimensions(m_eSize);
    CL_Point position = GetPosition();

    uint effectiveWidth = dimensions.x ;
    uint effectiveHeight = dimensions.y ;

    if(m_X % 32) ++effectiveWidth;
    if(m_Y % 32) ++effectiveHeight;

    for(uint x = position.x; x < position.x + effectiveWidth; x++)
    {
        for(uint y = position.y; y<position.y + effectiveHeight; y++)
        {
            (pLevel->*method)(CL_Point(x,y),this);
        }
    }

}

void StoneRing::MappableObject::ProvokeEvents ( Event::eTriggerType trigger )
{
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
            && (pEvent->Repeatable()
			|| ! party->DidEvent ( pEvent->GetName() ))
            )
        {
            pEvent->Invoke();
        }

    }
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
    CL_Point dimensions = calcCellDimensions(size);
    pList->clear();

    switch(dir)
    {
    case NONE:
        return;
    case NORTH:
        points = dimensions.x;
        for(uint i=0;i<points;i++)
        {
            pList->push_back ( CL_Point(topleft.x+i,topleft.y ));
        }

        return;
    case SOUTH:
        points = dimensions.x;
        for(uint i=0;i<points;i++)
        {
            pList->push_back ( CL_Point(topleft.x+i,topleft.y + (dimensions.y-1) ));
        }

        return;
    case EAST:
        points = calcCellDimensions(size).y;
        for(uint i=0;i<points;i++)
        {
            pList->push_back ( CL_Point(topleft.x+ (dimensions.x-1),topleft.y + i ));
        }

        break;
    case WEST:
        points = calcCellDimensions(size).y;
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

void StoneRing::MappableObject::MovedOneCell()
{
    if(++m_nCellsMoved)
    {
        if(m_nCellsMoved == m_nStepsUntilChange) ///@todo: get from somewhere
        {
            RandomNewDirection();
        }
    }

    Update();

}
bool StoneRing::MappableObject::IsAligned() const
{
    return (m_X % 32 == 0 )&& (m_Y  %32 == 0);
}

uint StoneRing::MappableObject::GetMovesPerDraw() const
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

CL_Point StoneRing::MappableObject::GetPositionAfterMove() const
{
    CL_Point point = GetPosition();

    switch(m_eDirection)
    {
    case SOUTH:
        point.y += 1;
        break;
    case NORTH:
        point.y -= 1;
        break;
    case EAST:
        point.x +=1;
        break;
    case WEST:
        point.x -=1;
        break;
    default:
        break;
    }

    return point;
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
    m_pMovement = new PlayerMovement;
}

StoneRing::MappablePlayer::~MappablePlayer()
{
}

uint StoneRing::MappablePlayer::GetMovesPerDraw() const
{
    if(m_bRunning) return 6;
    else return 3;
}





void StoneRing::MappablePlayer::SetNextDirection(eDirection newDir)
{
    m_bHasNextDirection = true;
    m_eNextDirection = newDir;
}

void StoneRing::MappablePlayer::RandomNewDirection()
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
        m_bHasNextDirection = false;
    }
    else
    {
        m_eDirection = NONE;
        m_bRunning = false;
    }
}

void StoneRing::MappablePlayer::MovedOneCell()
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
    switch( m_eFacingDirection )
    {
    case NORTH:
        m_sprite.set_frame(m_bStep? 6 : 7);
        break;
    case EAST:
        m_sprite.set_frame(m_bStep? 0 : 1);
        break;
    case WEST:
        m_sprite.set_frame(m_bStep? 2 : 3);
        break;
    case SOUTH:
        m_sprite.set_frame(m_bStep? 4 : 5);
        break;
    case NONE:
        m_sprite.set_frame(0);
        break;
    }
}




