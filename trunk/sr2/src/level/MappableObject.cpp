#include <cassert>
#include "Level.h"
#include "GraphicsManager.h"
#include "Graphic.h"
#include "SpriteRef.h"
#include "IParty.h"
#include <ClanLib/core.h>




namespace StoneRing { 




void MappableObject::SetPixelPosition ( const CL_Point& pixel_pos )
{
    m_pos = pixel_pos;
}

void MappableObject::Move(Level& level)
{
    if(!m_navStack.empty()){
        //uint speed = m_navStack.top()->GetSpeed();
        for(uint i=0;i<m_navStack.top()->GetSpeed();i++){
            if(IsAligned()){
                // TODO: Should probably make the new direction erturn from OnMove
                m_navStack.top()->OnMove(level);
                m_direction = m_navStack.top()->GetCurrentDirection();
                CL_Rect rect = GetTileRect();
                rect.translate ( m_direction.ToScreenVector() );                
                if(!level.Move(this,GetTileRect(),rect)){
                    m_navStack.top()->Blocked();
                    break;
                }

            }            
            CL_Point pt = GetPixelPosition();
            if(m_direction == Direction::NORTH)
                pt.y--;
            else if(m_direction == Direction::SOUTH)
                pt.y++;
            else if(m_direction == Direction::EAST)
                pt.x++;
            else if(m_direction == Direction::WEST)
                pt.x--;
            
            if(m_direction != Direction::NONE){
                SetPixelPosition(pt);
            }
        }
        if(m_direction == Direction::NONE)
            Stop();
        else
            OnStep();

    }
    Set_Frame_For_Direction();
}

MappableObject::MappableObject():m_nStep(0),m_cFlags(0),
                                            m_nTilesMoved(0),m_alpha(1.0f)
{
}

MappableObject::~MappableObject(){
}




std::string MappableObject::GetName() const
{
    return m_name;
}

CL_Size MappableObject::DimensionsFromSizeType ( ) const
{
  switch ( m_eSize )
    {
    case MO_SMALL:
        return CL_Size(1,1);
    case MO_MEDIUM:
        return CL_Size(2,2);
    case MO_LARGE:
        return CL_Size(3,3);
    case MO_TALL:
        return CL_Size(1,2);
    case MO_WIDE:
        return CL_Size(2,1);
    }
	assert(0);
	return CL_Size(0,0);
}


CL_Size MappableObject::Calc_Tile_Dimensions() const
{
    int movement_x, movement_y;
    movement_x = movement_y = 0;
    
    if(m_pos.x % 32 != 0)
        movement_x = 1;
    if(m_pos.y % 32 != 0)
        movement_y = 1;
    
    CL_Size size = DimensionsFromSizeType();


    return CL_Size(size.width+movement_x,size.height+movement_y);
}

CL_Rect MappableObject::GetSpriteRect() const
{
    CL_Rect pixelRect;
    CL_Size mySize = DimensionsFromSizeType();
    CL_Size myDimensions = mySize * 32;
 
    if(IsSprite())
    {
        pixelRect.top = m_pos.y - (m_sprite.get_height() - myDimensions.height);
        pixelRect.left = m_pos.x - (m_sprite.get_width() - myDimensions.width);
        pixelRect.right = pixelRect.left + m_sprite.get_width();
        pixelRect.bottom = pixelRect.top + m_sprite.get_height();

        return pixelRect;
    }
    else if (m_cFlags & TILEMAP)
    {
        return CL_Rect(m_pos.x*32,m_pos.y*32, m_pos.x*32 + 32, m_pos.y *32 +32);
    }
    else
    {
        return CL_Rect(m_pos.x*32,m_pos.y*32,m_pos.x * 32 + myDimensions.width, m_pos.y* 32 + myDimensions.height);
    }
}

bool MappableObject::IsSprite() const
{
    return m_cFlags & SPRITE;
}

void MappableObject::Draw(CL_GraphicContext& GC, const CL_Point& offset)
{
    CL_Rect dstRect = GetSpriteRect();
    dstRect.translate(offset);
    if(IsSprite())
    {
        float alpha = m_sprite.get_alpha();
        m_sprite.set_alpha(m_alpha);
        m_sprite.draw(GC,dstRect);
        m_sprite.set_alpha(alpha);
    }
    else if( m_cFlags & TILEMAP )
    {
#ifndef NDEBUG
        std::cout << "Mappable Object is tilemap?" << std::endl;
#endif
        CL_Rect srcRect(m_graphic.asTilemap->GetMapX() * 32, m_graphic.asTilemap->GetMapY() * 32,
                        (m_graphic.asTilemap->GetMapX() * 32), (m_graphic.asTilemap->GetMapY() * 32));

        m_graphic.asTilemap->GetTileMap().draw(GC,srcRect, dstRect);
    }
}

Direction MappableObject::Get_Default_Facing() const
{
    return Direction::SOUTH;
}



void MappableObject::Set_Frame_For_Direction()
{
    if(m_sprite.is_null()) return;

    m_sprite.set_frame(0);
    
    Direction facing = Get_Default_Facing();
    if(!m_navStack.empty())
        facing = m_navStack.top()->GetFacingDirection();

        switch(GetMovementType())
        {
        case MOVEMENT_NONE:
            
            m_sprite.set_frame(0);

            break;
        case MOVEMENT_WANDER:
        {
            int step = m_nStep % 4;
            if(facing == Direction::NORTH)
                m_sprite.set_frame(12 + step);
            else if(facing == Direction::EAST)
                m_sprite.set_frame(8 + step);            
            else if(facing == Direction::WEST)
                m_sprite.set_frame(4 + step);
            else if(facing == Direction::SOUTH)
                m_sprite.set_frame(step);
            break;
        }
        case MOVEMENT_PACE_NS:
        {
            int step = m_nStep % 4;
            if(facing == Direction::NORTH)
                m_sprite.set_frame ( 4 + step );
            else if(facing == Direction::SOUTH)
                m_sprite.set_frame ( step);
            break;
        }
        case MOVEMENT_PACE_EW:
        {
            int step = m_nStep % 4;
            if(facing == Direction::EAST)
                m_sprite.set_frame ( 4 + step  );    
            else if(facing == Direction::WEST)
                m_sprite.set_frame ( step );
            break;
        }
        
    }
    
}


void MappableObject::Update()
{

}


bool MappableObject::IsSolid() const
{
    return m_cFlags & MappableObject::SOLID;
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
        m_navStack.top()->Prod();
}


bool MappableObject::EvaluateCondition() const
{
    return true;
}

bool MappableObject::ProvokeEvents ( Event::eTriggerType trigger )
{
    return false;
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
	return 0;
}

void MappableObject::CalculateEdgePoints(const CL_Point &topleft, Direction dir, std::list<CL_Point> *pList)
{
    uint points = 0;
    //CL_Size dimensions = Calc_Tile_Dimensions();
    // We don't count the movement dimensions, only their normal dimensions
    CL_Size dimensions = DimensionsFromSizeType();
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
        points = dimensions.height;
        for(uint i=0;i<points;i++)
        {
            pList->push_back ( CL_Point(topleft.x+ (dimensions.width-1),topleft.y + i ));
        }
    }else if(dir == Direction::WEST){
        points = dimensions.height;
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
        if(!m_navStack.empty())
            m_navStack.top()->Prod();
        return pNav;
    }else{
        return NULL;
    }
}



MappableObjectElement::MappableObjectElement():m_pCondition(NULL),m_navigator(*this)
{

}


MappableObjectElement::~MappableObjectElement()
{
    for( std::list<Event*>::iterator i = m_events.begin();
         i != m_events.end();
         i++)
    {
        delete *i;
    }
}




bool MappableObjectElement::EvaluateCondition() const
{
    if(m_pCondition)
    {
        return m_pCondition->EvaluateCondition();
    }

    return true;
}




bool MappableObjectElement::ProvokeEvents ( Event::eTriggerType trigger )
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



void MappableObjectElement::load_attributes(CL_DomNamedNodeMap attributes)
{
    m_name = get_required_string("name",attributes);
    std::string motype = get_required_string("type",attributes);
    std::string size = get_required_string("size",attributes);
    

    m_speed = SLOW;
    if (has_attribute("speed",attributes))
    {
        std::string speed = get_string("speed",attributes);

        if (speed == "medium")
        {
            m_speed = MEDIUM;
        }
        else if (speed == "slow")
        {
            m_speed = SLOW;
        }
        else if (speed == "fast")
        {
            m_speed = FAST;
        }
        else throw CL_Exception("Error, movement speed must be fast, medium or slow.");

    }

    std::string type = get_implied_string("movementType",attributes,"none");

    if (type == "wander")
    {
        m_move_type = MOVEMENT_WANDER;
    }
    else if (type == "paceNS")
    {
        m_move_type = MOVEMENT_PACE_NS;
    }
    else if (type == "paceEW")
    {
        m_move_type = MOVEMENT_PACE_EW;
    }
    else if (type == "script")
    {
        m_move_type = MOVEMENT_SCRIPT;
    }
    else if (type == "none")
    {
        m_move_type = MOVEMENT_NONE;
    }
  
    

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

    if(solid) m_cFlags |= SOLID;

}

bool MappableObjectElement::handle_element(Element::eElement element, Element * pElement)
{
    switch(element)
    {
    case ETILEMAP:
    {
        if( m_eSize != MO_SMALL) throw CL_Exception("Mappable objects using tilemaps MUST be size small.");
        m_cFlags |= TILEMAP;
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
        m_cFlags |= SPRITE;

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
    default:
        return false;
    }

    return true;
}

void MappableObjectElement::load_finished()
{
    CL_Size dimensions = Calc_Tile_Dimensions();

    PushNavigator(&m_navigator);
}

MappableObjectDynamic::MappableObjectDynamic ( MappableObject::eMappableObjectType type, MappableObject::eMovementType move_type, MappableObject::eMovementSpeed speed )
:m_move_type(move_type),m_move_speed(speed)
{
    m_eType = type;
}

MappableObjectDynamic::~MappableObjectDynamic()
{

}

Direction MappableObjectDynamic::Get_Default_Facing() const
{
    return m_default_facing;
}


void MappableObjectDynamic::SetSolid ( bool solid )
{
    m_cFlags |= SOLID;
}

void MappableObjectDynamic::SetSprite ( CL_Sprite sprite, MappableObject::eSize size )
{
    m_sprite = sprite;
    m_eSize = size;
    m_cFlags |= SPRITE;
    Calc_Tile_Dimensions();
}

void MappableObjectDynamic::Draw ( CL_GraphicContext& GC, const CL_Point& offset )
{
    CL_Rect dstRect = GetSpriteRect();
    dstRect.translate(offset);
    m_sprite.draw(GC,dstRect);
}

void MappableObjectDynamic::Set_Frame_For_Direction()
{
    
    StoneRing::MappableObject::Set_Frame_For_Direction();
}




MappablePlayer::MappablePlayer(uint startX, uint startY):m_navigator(*this)
{
    m_eSize = MO_SMALL;
    m_StartX = startX;
    m_StartY = startY;
    m_pos.x=startX * 32;
    m_pos.y=startY * 32;
    m_name = "Player";
    m_eType = PLAYER;
    PushNavigator(&m_navigator);
}

MappablePlayer::~MappablePlayer()
{
}


void MappablePlayer::StopMovement()
{
    Stop();
    if(!m_navStack.empty())
        m_navStack.top()->Idle();
   // Update();
}




CL_Point MappablePlayer::GetPointInFront() const
{
    CL_Point point = GetPosition();

    
    Direction facing;
    if(!m_navStack.empty())      
       facing = m_navStack.top()->GetFacingDirection();
    else
        facing = Direction::SOUTH;
    
    if(facing == Direction::NORTH)
        point.y--;
    else if(facing == Direction::SOUTH)
        point.y++;
    else if(facing == Direction::EAST)
        point.x++;
    else if(facing == Direction::WEST)
        point.x--;

    return point;
}

void MappablePlayer::Set_Frame_For_Direction()
{    
    Direction facing = Get_Default_Facing();
    if(!m_navStack.empty())      
        facing = m_navStack.top()->GetFacingDirection();
    
    if(facing == Direction::NONE)
        m_nStep = 0;
    
    if(facing ==  Direction::NORTH)
        m_sprite.set_frame(12 + (m_nStep / 2)  % 4);
    else if(facing == Direction::EAST)
        m_sprite.set_frame(8 + (m_nStep/2) % 4);
    else if(facing == Direction::WEST)
        m_sprite.set_frame(4 + (m_nStep/2) % 4);
    else if(facing == Direction::SOUTH)
        m_sprite.set_frame((m_nStep/2) % 4);

}

void MappablePlayer::SerializeState ( std::ostream& out )
{
    out.write((char*)&m_pos,sizeof(m_pos));
}

void MappablePlayer::DeserializeState ( std::istream& in )
{
    in.read((char*)&m_pos,sizeof(m_pos));
}


} // namespace StoneRing




