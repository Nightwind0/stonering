
#include "Level.h"
#include "GraphicsManager.h"


CL_DomElement  StoneRing::MappableObject::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"mo");

    element.set_attribute( "name", mName );
    
    std::string motype;
    std::string size;

    switch( meSize )
    {
    case MO_SMALL:
        size = "small";
        break;
    case MO_MEDIUM:
        size = "medium";
        break;
    case MO_LARGE:
        size = "large";
        break;
    case MO_TALL:
        size = "tall";
        break;
    case MO_WIDE:
        size = "wide";
        break;
    
    }


    switch ( meType )
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

    element.set_attribute("size", size);
    element.set_attribute("type", motype );
    element.set_attribute("xpos", IntToString(mStartX) );
    element.set_attribute("ypos", IntToString(mStartY) );
 

    if(isSolid()) element.set_attribute("solid", "true" );

    if(cFlags & TILEMAP)
    {
        CL_DomElement  tilemapEl = mGraphic.asTilemap->createDomElement(doc);

        element.append_child (  tilemapEl );

    }


    if(isSprite())
    {
    
        CL_DomElement  spriteRefEl = mGraphic.asSpriteRef->createDomElement(doc);


        element.append_child ( spriteRefEl );

    }

    if(mpCondition)
    {
        CL_DomElement condition = mpCondition->createDomElement(doc);

        element.append_child( condition );
    }

    for(std::list<Event*>::const_iterator h = mEvents.begin();
        h != mEvents.end(); h++)
    {
        CL_DomElement  evEl= (*h)->createDomElement(doc);

        element.append_child( evEl );


    }

    if(mpMovement)
    {
        CL_DomElement  moveEl = mpMovement->createDomElement(doc);

        element.append_child ( moveEl );


    }


    return element;
    
}

bool StoneRing::MappableObject::evaluateCondition() const
{
    if(mpCondition)
    {
        return mpCondition->evaluate();
    }

    return true;
}

StoneRing::MappableObject::eSize StoneRing::MappableObject::getSize() const
{
    return meSize;
} 

void StoneRing::MappableObject::move()
{
    switch(meDirection)
    {
    case NORTH:
        mY--;
        break;
    case SOUTH:
        mY++;
        break;
    case EAST:
        mX++;
        break;
    case WEST:
        mX--;
        break;

    }
}

void StoneRing::MappableObject::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mName = getRequiredString("name",pAttributes);
    std::string motype = getRequiredString("type",pAttributes);
    std::string size = getRequiredString("size",pAttributes);

    mStartX= getRequiredInt("xpos",pAttributes);
    mStartY = getRequiredInt("ypos",pAttributes);

    mX = mStartX *32;
    mY = mStartY *32;

    if(size == "small") meSize = MO_SMALL;
    else if(size == "medium") meSize = MO_MEDIUM;
    else if(size == "large") meSize = MO_LARGE;
    else if(size == "wide") meSize = MO_WIDE;
    else if(size == "tall") meSize = MO_TALL;
    else throw CL_Error("MO size wasnt small, medium, or large.");

    if(motype == "npc") meType = NPC;
    else if (motype == "square") meType = SQUARE;
    else if (motype == "container") meType = CONTAINER;
    else if (motype == "door") meType = DOOR;
    else if (motype == "warp") meType = WARP;

    bool solid = getImpliedBool("solid",pAttributes,false);

    if(solid) cFlags |= SOLID;

}

void StoneRing::MappableObject::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case ETILEMAP:
    {
        if( meSize != MO_SMALL) throw CL_Error("Mappable objects using tilemaps MUST be size small.");
        cFlags |= TILEMAP;
        mGraphic.asTilemap = dynamic_cast<Tilemap*>(pElement);
        break;
    }
    case ESPRITEREF:
    {
        GraphicsManager *GM = GraphicsManager::getInstance();

        SpriteRef * pRef = dynamic_cast<SpriteRef*>(pElement);

        mGraphic.asSpriteRef = pRef;
        cFlags |= SPRITE;

        mpSprite = GM->createSprite ( pRef->getRef() );
        

        int swidth = mpSprite->get_width();
        int sheight = mpSprite->get_height();

        switch( meSize )
        {
        case MO_SMALL:
            if( swidth != 32 && sheight !=32) throw CL_Error("Sprite size doesn't match MO size (SMALL)");
            break;
        case MO_MEDIUM:
            if( swidth != 64 && sheight != 64) throw CL_Error("Sprite size doesn't match MO size (MEDIUM).");
            break;
        case MO_LARGE:
            if( swidth != 128 && sheight != 128) throw CL_Error("Sprite size doesnt match MO size (LARGE)");
            break;
        case MO_TALL:
            if( swidth != 32 && sheight != 64) throw CL_Error("Sprite size does not match MO size(TALL)");
            break;
        case MO_WIDE:
            if( swidth != 64 && sheight != 32) throw CL_Error("Sprite size does not match MO size(TALL)");
            break;
        }
            
        break;
    }
    case ECONDITION:
        mpCondition = dynamic_cast<Condition*>(pElement);
        break;
    case EEVENT:
        mEvents.push_back ( dynamic_cast<Event*>(pElement));
        break;
    case EMOVEMENT:
    {
        mpMovement = dynamic_cast<Movement*>(pElement);

        // Make sure the proper sprites are around for the movement type
        switch(mpMovement->getMovementType())
        {
        case Movement::MOVEMENT_WANDER:
            if(!mGraphic.asSpriteRef->getType() == SpriteRef::SPR_FOUR_WAY)
                throw CL_Error("Wandering MO needs a four way sprite ref.");

            meDirection = SOUTH;
            break;
        case Movement::MOVEMENT_PACE_NS:
            meDirection = SOUTH;
            if(!mGraphic.asSpriteRef->getType() == SpriteRef::SPR_TWO_WAY)
                throw CL_Error("Pacing MO needs a two way sprite ref.");
            break;
        case Movement::MOVEMENT_PACE_EW:
            if(!mGraphic.asSpriteRef->getType() == SpriteRef::SPR_TWO_WAY)
                throw CL_Error("Pacing MO needs a two way sprite ref.");
            meDirection = EAST;
            break;  
        default:
            break;
        }

        break;
    }
    }
}

void StoneRing::MappableObject::loadFinished()
{
    setFrameForDirection();
}
 
StoneRing::MappableObject::MappableObject():meDirection(NONE),mpSprite(NULL),mpMovement(0),
                                 mpCondition(0),cFlags(0),mnCellsMoved(0),
                                 mnFrameMarks(0),mnStepsUntilChange(0)
{
   
}


uint StoneRing::MappableObject::getCellHeight() const
{
    return calcCellDimensions(meSize).x;
}

uint StoneRing::MappableObject::getCellWidth() const
{
    return calcCellDimensions(meSize).y;
}
StoneRing::MappableObject::~MappableObject()
{
    for( std::list<Event*>::iterator i = mEvents.begin();
         i != mEvents.end();
         i++)
    {
        delete *i;
    }

    delete mpSprite;

    delete mpMovement;
         
}




StoneRing::Movement* StoneRing::MappableObject::getMovement() const
{
    return mpMovement;
}




std::string StoneRing::MappableObject::getName() const
{
    return mName;
}


CL_Point StoneRing::MappableObject::calcCellDimensions(eSize size)
{
    uint width,height;

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

CL_Rect StoneRing::MappableObject::getPixelRect() const
{
    CL_Rect pixelRect;
    CL_Point myDimensions = calcCellDimensions(meSize);
    myDimensions.x *= 32;
    myDimensions.y *= 32;

    if(isSprite())
    {
        pixelRect.top = mY - (mpSprite->get_height() - myDimensions.y);
        pixelRect.left = mX - (mpSprite->get_width() - myDimensions.x);
        pixelRect.right = pixelRect.left + mpSprite->get_width();
        pixelRect.bottom = pixelRect.top + mpSprite->get_height();

        return pixelRect;     
    }
    else if (cFlags & TILEMAP)
    {
        return CL_Rect(mX*32,mY*32, mX*32 + 32, mY *32 +32);
    }
    else
    {
        return CL_Rect(mX*32,mY*32,mX * 32 + myDimensions.x, mY* 32 + myDimensions.y);
    }
    
        
}

bool StoneRing::MappableObject::isSprite() const
{
    return cFlags & SPRITE;
}

void StoneRing::MappableObject::draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)
{


    if(isSprite())
    {
        setFrameForDirection();
        mpSprite->draw(dst, pGC );
    }
    else if( cFlags & TILEMAP )
    {
#ifndef NDEBUG
        std::cout << "Mappable Object is tilemap?" << std::endl;
#endif

        CL_Rect srcRect(mGraphic.asTilemap->getMapX() * 32 + src.left, mGraphic.asTilemap->getMapY() * 32 + src.top,
                        (mGraphic.asTilemap->getMapX() * 32) + src.right, (mGraphic.asTilemap->getMapY() * 32) + src.bottom);

        
        mGraphic.asTilemap->getTileMap()->draw(srcRect, dst, pGC);
        
    }
    

}

void StoneRing::MappableObject::pickOppositeDirection()
{
    switch(meDirection)
    {
    case WEST:
        meDirection = EAST;
        break;
    case EAST:
        meDirection = WEST;
        break;
    case NORTH:
        meDirection = SOUTH;
        break;
    case SOUTH:
        meDirection = NORTH;
        break;
    default:
        break;
    }

}


void StoneRing::MappableObject::randomNewDirection()
{


    if(!mpMovement) return;

    eDirection current = meDirection;
    
    while(meDirection == current)
    {
        int r= rand() % 5;
    
        switch ( mpMovement->getMovementType())
        {
        case Movement::MOVEMENT_NONE:
            break;
        case Movement::MOVEMENT_WANDER:
            if(r == 0)
                meDirection = NORTH;
            else if(r == 1)
                meDirection = SOUTH;
            else if(r == 2)
                meDirection = EAST;
            else if(r == 3)
                meDirection = WEST;
            else if(r == 4)
                meDirection = NONE;
            break;
        case Movement::MOVEMENT_PACE_NS:
        case Movement::MOVEMENT_PACE_EW:
            if(r > 2)
                pickOppositeDirection();
            break;
        default:
            break;
        
        }
    }

    mnCellsMoved = 0;
	mnStepsUntilChange = rand() % 20;

    setFrameForDirection();

#ifndef NDEBUG
	std::cout << "Random new direction" << std::endl;
#endif
        
}

void StoneRing::MappableObject::setFrameForDirection()
{

    
    if(!mpMovement && mpSprite) mpSprite->set_frame(0);
    else if (!mpMovement) return;

    switch(mpMovement->getMovementType())
    {
    case Movement::MOVEMENT_NONE:
        if(mbStep) mpSprite->set_frame(1);
        else mpSprite->set_frame(0);
    
        break;
    case Movement::MOVEMENT_WANDER:
    {
        switch( meDirection )
        {
        case NORTH:
            mpSprite->set_frame(mbStep? 6 : 7);
            break;
        case EAST:
            mpSprite->set_frame(mbStep? 0 : 1);
            break;
        case WEST:
            mpSprite->set_frame(mbStep? 2 : 3);
            break;
        case SOUTH:
            mpSprite->set_frame(mbStep? 4 : 5);
            break;
        case NONE:
            if(mbStep) mpSprite->set_frame(4);
            else mpSprite->set_frame(5);
            break;
        }
        break;
    }
    case Movement::MOVEMENT_PACE_NS:
    {
        switch(meDirection)
        {
        case NORTH:
            mpSprite->set_frame ( mbStep? 2 : 3);
            break;
        case SOUTH:
        case NONE:
            mpSprite->set_frame ( mbStep? 0 : 1);
            break;
        }
        break;
    }
    case Movement::MOVEMENT_PACE_EW:
    {
        switch(meDirection)
        {
        case EAST:
        case NONE:
            mpSprite->set_frame ( mbStep? 0 : 1 );
            break;
        case WEST:
            mpSprite->set_frame ( mbStep? 2 : 3 );
            break;
        }
        break;
    }
    }
}


void StoneRing::MappableObject::update()
{

    // Don't bother with disabled MOs
    if(!evaluateCondition()) return;

    if(isSprite())
    {
        mbStep = mbStep? false:true;
      
    }
}





bool StoneRing::MappableObject::isSolid() const
{
    return cFlags & StoneRing::MappableObject::SOLID;
}

int StoneRing::MappableObject::getDirectionBlock() const
{
    if (isSolid()) return  (DIR_NORTH | DIR_SOUTH | DIR_EAST | DIR_WEST);
    else return 0;
}

bool StoneRing::MappableObject::isTile() const
{
    return false;
}

void StoneRing::MappableObject::prod()
{
    randomNewDirection();
}

bool StoneRing::MappableObject::onScreen()
{
    CL_Rect screen = IApplication::getInstance()->getLevelRect();
  
    return screen.is_overlapped(getPixelRect());
}

void StoneRing::MappableObject::setOccupiedPoints(Level * pLevel,LevelPointMethod method)
{
	CL_Point dimensions = calcCellDimensions(meSize);
	CL_Point position = getPosition();

	uint effectiveWidth = dimensions.x ;
	uint effectiveHeight = dimensions.y ;

	if(mX % 32) ++effectiveWidth;
	if(mY % 32) ++effectiveHeight;

	for(uint x = position.x; x < position.x + effectiveWidth; x++)
	{
		for(uint y = position.y; y<position.y + effectiveHeight; y++)
		{
			(pLevel->*method)(CL_Point(x,y),this);
		}
	}

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

void StoneRing::MappableObject::provokeEvents ( Event::eTriggerType trigger )
{
    IParty *party = IApplication::getInstance()->getParty();

    for(std::list<Event*>::iterator i = mEvents.begin();
        i != mEvents.end();
        i++)
    {
        Event * event = *i;
        
        // If this is the correct trigger,
        // And the event is either repeatable or
        // Hasn't been done yet, invoke it

        if( event->getTriggerType() == trigger 
            && (event->repeatable() || ! party->didEvent ( event->getName() ))
            )
        {
            event->invoke();
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
}

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

void StoneRing::MappableObject::movedOneCell()
{
    if(++mnCellsMoved) 
    {
        if(mnCellsMoved == mnStepsUntilChange) ///@todo: get from somewhere
        {
			randomNewDirection();
        }
    }

}
bool StoneRing::MappableObject::isAligned() const
{
    return (mX % 32 == 0 )&& (mY  %32 == 0);
}

uint StoneRing::MappableObject::getMovesPerDraw() const
{
    if(mpMovement)
    {
        switch( mpMovement->getMovementSpeed() )
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

CL_Point StoneRing::MappableObject::getPositionAfterMove() const
{
    CL_Point point = getPosition();

    switch(meDirection)
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
    }

    return point;
}