#include <cassert>
#include "Level.h"
#include "GraphicsManager.h"
#include "Graphic.h"
#include "SpriteRef.h"
#include "Party.h"
#include <ClanLib/core.h>
#include <ClanLib-2.3/ClanLib/Core/System/cl_platform.h>





namespace StoneRing { 


bool MappableObject::IsFlying() const
{
    return m_cFlags & FLYING;
}

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

MappableObject::MappableObject():m_nStepLoop(0),m_cFlags(0),
                                            m_nTilesMoved(0),m_alpha(1.0f)
{
}

MappableObject::~MappableObject(){
}



void MappableObject::OnStep()
{
    m_nStepLoop = m_nStep++ / 4; // Change the step frame every 4 pixels of movement
    Set_Frame_For_Direction();
}


std::string MappableObject::GetName() const
{
    return m_name;
}

CL_Size MappableObject::DimensionsFromSizeType ( eSize size )
{
  switch ( size )
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
    
    CL_Size size = m_size;


    return CL_Size(size.width+movement_x,size.height+movement_y);
}

CL_Rect MappableObject::GetSpriteRect() const
{
    CL_Rect pixelRect;
    CL_Size mySize = m_size;
    CL_Size myDimensions = mySize * 32;
 
    if(IsSprite())
    {
        pixelRect.top = m_pos.y + (myDimensions.height - m_sprite.get_height());
        pixelRect.left = m_pos.x + (myDimensions.width - m_sprite.get_width());
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
        CL_Rect srcRect(m_tilemap->GetMapX() * 32, m_tilemap->GetMapY() * 32,
                        (m_tilemap->GetMapX() * 32), (m_tilemap->GetMapY() * 32));

        m_tilemap->GetTileMap().draw(GC,srcRect, dstRect);
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
    
    if(facing == Direction::NONE)
        m_nStepLoop = 0;
    
    int step = 0;
    
    if(!m_navStack.empty())
        facing = m_navStack.top()->GetFacingDirection();
    
    switch(GetMovementType())
    {
    case MOVEMENT_WANDER:
    {
        step = m_nStepLoop % (m_sprite.get_frame_count()/4 * 2 - 1);
        if(step > m_sprite.get_frame_count()/4 - 1){
            step = m_sprite.get_frame_count()/4 * 2 - step - 2;   
        }
        break;
    }
    case MOVEMENT_PACE_NS:
    {
        step = m_nStepLoop % (m_sprite.get_frame_count()/2 * 2 - 1);
        if(step > m_sprite.get_frame_count()/2 - 1){
            step = m_sprite.get_frame_count()/2 * 2 - step - 2;   
        }            
    }
    case MOVEMENT_PACE_EW:
    {
        step = m_nStepLoop % (m_sprite.get_frame_count()/2 * 2 - 1);
        if(step > m_sprite.get_frame_count()/2 - 1){
            step = m_sprite.get_frame_count()/2 * 2 - step - 2;   
        }             
    }
    break;
    }

    // TODO: Have to handle sprites that only have steps in NS or EW directions
    
    if(facing == Direction::NORTH)
        m_sprite.set_frame(step);
    else if(facing == Direction::EAST)
        m_sprite.set_frame(3 + step);            
    else if(facing == Direction::WEST)
        m_sprite.set_frame(9 + step);
    else if(facing == Direction::SOUTH)
        m_sprite.set_frame(6 + step);
    
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
    CL_Size dimensions = m_size;
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
    Party *party = IApplication::GetInstance()->GetParty();

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
    
    
    if(has_attribute("sprite",attributes)){
        m_sprite.clone( GraphicsManager::CreateSprite(get_string("sprite",attributes),true) );
        m_cFlags |= SPRITE;
#ifdef SR2_EDITOR
        m_sprite_name = get_string("sprite",attributes);
#endif
    }
    
    m_start_facing = Direction(get_implied_string("facing",attributes,"south")); 

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
        if(m_start_facing != Direction::NORTH &&
            m_start_facing != Direction::SOUTH){
            m_start_facing = Direction::SOUTH;
        }
            
    }
    else if (type == "paceEW")
    {
        m_move_type = MOVEMENT_PACE_EW;
        if(m_start_facing != Direction::EAST && 
            m_start_facing != Direction::WEST){
            m_start_facing = Direction::EAST;
        }
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

    if(size == "small") m_size = DimensionsFromSizeType(MO_SMALL);
    else if(size == "medium") m_size = DimensionsFromSizeType(MO_MEDIUM);
    else if(size == "large") m_size = DimensionsFromSizeType(MO_LARGE);
    else if(size == "wide") m_size = DimensionsFromSizeType(MO_WIDE);
    else if(size == "tall") m_size = DimensionsFromSizeType(MO_TALL);
    else {
            size_t pos = size.find_first_of('x');
            if(pos == string::npos){
                throw CL_Exception("Bad MO size");
            }
            std::string width_str = size.substr(0,pos);
            std::string height_str = size.substr(pos+1,size.size()-1);
            m_size.width = atoi(width_str.c_str());
            m_size.height = atoi(height_str.c_str());            
    }
 
    if(motype == "npc") m_eType = NPC;
    else if (motype == "square") m_eType = SQUARE;
    else if (motype == "container") m_eType = CONTAINER;
    else if (motype == "door") m_eType = DOOR;
    else if (motype == "warp") m_eType = WARP;

    bool solid = get_implied_bool("solid",attributes,false);
    
    bool flying = get_implied_bool("flying",attributes,false);

    if(solid) m_cFlags |= SOLID;
    if(flying) m_cFlags |= FLYING;

}

bool MappableObjectElement::handle_element(Element::eElement element, Element * pElement)
{
    switch(element)
    {
    case ETILEMAP:
    {
        if( m_size != DimensionsFromSizeType(MO_SMALL)) throw CL_Exception("Mappable objects using tilemaps MUST be size small.");
        m_cFlags |= TILEMAP;
        m_tilemap = dynamic_cast<Tilemap*>(pElement);
        break;
    }
    case ECONDITIONSCRIPT:
        m_pCondition = dynamic_cast<ScriptElement*>(pElement);
        break;
    case EEVENT:
        m_events.push_back ( dynamic_cast<Event*>(pElement));
        break;
    case ESPRITEREF:{
        SpriteRef* ref = dynamic_cast<SpriteRef*>(pElement);
#ifdef SR2_EDITOR
        m_sprite_name = ref->GetRef();
#endif        
        m_cFlags |= SPRITE;
        m_sprite = GraphicsManager::CreateSprite(m_sprite_name);
        break;
    }default:
        return false;
    }

    return true;
}

void MappableObjectElement::load_finished()
{
    CL_Size dimensions = Calc_Tile_Dimensions();

    PushNavigator(&m_navigator);
    Prod();
}

void MappableObjectElement::Placed()
{
    Set_Frame_For_Direction();
}


Direction MappableObjectElement::Get_Default_Facing() const
{
    return m_start_facing;
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
    m_size = DimensionsFromSizeType(size);
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
    m_size = CL_Size(1,1);
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
    MappableObject::Set_Frame_For_Direction();
}

void MappablePlayer::SerializeState ( std::ostream& out )
{
    out.write((char*)&m_pos,sizeof(m_pos));
}

void MappablePlayer::DeserializeState ( std::istream& in )
{
    in.read((char*)&m_pos,sizeof(m_pos));
}

#if SR2_EDITOR
CL_DomElement MappableObjectElement::CreateDomElement(CL_DomDocument& doc)const
{
    CL_DomElement element(doc,"mo");
    element.set_attribute( "name", m_name );
    
    std::string motype;
    std::string speed;
    std::string movetype;

    
    if(IsSprite()){
        element.set_attribute("sprite",m_sprite_name);
    }

    std::string size;
    size += IntToString(m_size.width) + 'x' + IntToString(m_size.height);


    switch ( m_eType )
    {
    case NPC:
        motype = "npc";
        break;
    case SQUARE:
        motype = "square";
        break;
    case CONTAINER:
        motype = "container";
        break;
    case DOOR:
        motype = "door";
        break;
    case WARP:
        motype = "warp";
        break;
    }
    
    switch(m_speed)
    {
        case SLOW:
            speed = "slow";
            break;
        case MEDIUM:
            speed = "medium";
            break;
        case FAST:
            speed = "fast";
            break;
    }
    
    switch(m_move_type)
    {
        case MOVEMENT_NONE:
            movetype = "none";
            break;
        case MOVEMENT_PACE_EW:
            movetype = "paceEW";
            break;
        case MOVEMENT_PACE_NS:
            movetype = "paceNS";
            break;
        case MOVEMENT_WANDER:
            movetype = "wander";
            break;
        case MOVEMENT_SCRIPT:
            movetype = "script";
            break;
    }

    element.set_attribute("movementType",movetype);
    element.set_attribute("speed",speed);
    element.set_attribute("size", size);
    element.set_attribute("type", motype );
    element.set_attribute("xpos", IntToString(m_StartX) );
    element.set_attribute("ypos", IntToString(m_StartY) );
    
    element.set_attribute("facing",(std::string)m_start_facing);
 

    if(IsSolid()) element.set_attribute("solid", "true" );

    if(m_cFlags & TILEMAP){
        element.append_child(m_tilemap->CreateDomElement(doc));
    }

    if(m_pCondition){
        element.append_child(m_pCondition->CreateDomElement(doc));
    }

    for(std::list<StoneRing::Event*>::const_iterator h = m_events.begin();
        h != m_events.end(); h++) {
        element.append_child((*h)->CreateDomElement(doc));
    }
    
    create_dom_element_hook(element);

    return element;  
}

void MappableObjectElement::create_dom_element_hook(CL_DomElement& element)const
{
        
}

#endif



} // namespace StoneRing




