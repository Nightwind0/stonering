#include "EditorElements.h"
#include "IApplication.h"
#include "GraphicsManager.h"

using namespace Editor;

//Editor::

Editor::Animation::Animation()
{
}
Editor::Animation::~Animation()
{
}
CL_DomElement Editor::Animation::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"animation");
}


Editor::AnimationDefinition::AnimationDefinition()
{
}
Editor::AnimationDefinition::~AnimationDefinition()
{
}
CL_DomElement Editor::AnimationDefinition::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"animationDefinition");
}


Editor::AnimationSpriteRef::AnimationSpriteRef()
{
}
Editor::AnimationSpriteRef::~AnimationSpriteRef()
{
}
CL_DomElement Editor::AnimationSpriteRef::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"animationSpriteRef");
}

Editor::ArmorClass::ArmorClass()
{
}
Editor::ArmorClass::~ArmorClass()
{
}
CL_DomElement Editor::ArmorClass::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"armorClass");
}


Editor::ArmorClassRef::ArmorClassRef()
{
}
Editor::ArmorClassRef::~ArmorClassRef()
{
}
CL_DomElement Editor::ArmorClassRef::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"armorClassRef");
}

Editor::ArmorEnhancer::ArmorEnhancer()
{
}
Editor::ArmorEnhancer::~ArmorEnhancer()
{
}
CL_DomElement Editor::ArmorEnhancer::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"armorEnhancer");
}


Editor::ArmorRef::ArmorRef()
{
}
Editor::ArmorRef::~ArmorRef()
{
}
CL_DomElement Editor::ArmorRef::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"armorRef");
}


Editor::ArmorType::ArmorType()
{
}
Editor::ArmorType::~ArmorType()
{
}
CL_DomElement Editor::ArmorType::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"armorType");
}


Editor::ArmorTypeExclusionList::ArmorTypeExclusionList()
{
}
Editor::ArmorTypeExclusionList::~ArmorTypeExclusionList()
{
}
CL_DomElement Editor::ArmorTypeExclusionList::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"armorTypeExclusionList");
}


Editor::ArmorTypeRef::ArmorTypeRef()
{
}
Editor::ArmorTypeRef::~ArmorTypeRef()
{
}
CL_DomElement Editor::ArmorTypeRef::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"armorTypeRef");
}


Editor::AttributeEnhancer::AttributeEnhancer()
{
}
Editor::AttributeEnhancer::~AttributeEnhancer()
{
}
CL_DomElement Editor::AttributeEnhancer::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"attributeEnhancer");
}


Editor::BattleMenu::BattleMenu()
{
}
Editor::BattleMenu::~BattleMenu()
{
}
CL_DomElement Editor::BattleMenu::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"battleMenu");
}


Editor::BattleMenuOption::BattleMenuOption()
{
}
Editor::BattleMenuOption::~BattleMenuOption()
{
}
CL_DomElement Editor::BattleMenuOption::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"battleMenuOption");
}


Editor::CharacterClass::CharacterClass()
{
}
Editor::CharacterClass::~CharacterClass()
{
}
CL_DomElement Editor::CharacterClass::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"characterClass");
}


Editor::CharacterDefinition::CharacterDefinition()
{
}
Editor::CharacterDefinition::~CharacterDefinition()
{
}
CL_DomElement Editor::CharacterDefinition::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"characterDefinition");
}


Editor::ConditionScript::ConditionScript()
:ScriptElement(true)
{
}
Editor::ConditionScript::~ConditionScript()
{
}
CL_DomElement Editor::ConditionScript::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"conditionScript");
}


Editor::DirectionBlock::DirectionBlock()
{
}

Editor::DirectionBlock::DirectionBlock(int i)
:StoneRing::DirectionBlock(i)
{
}

Editor::DirectionBlock::~DirectionBlock()
{
}
CL_DomElement Editor::DirectionBlock::createDomElement(CL_DomDocument &doc)const
{
    CL_DomElement element(doc,"directionBlock");

    element.set_attribute("north", (meDirectionBlock & StoneRing::DIR_NORTH )?"true":"false");
    element.set_attribute("south", (meDirectionBlock & StoneRing::DIR_SOUTH)?"true":"false");
    element.set_attribute("east",  (meDirectionBlock & StoneRing::DIR_EAST )?"true":"false" );
    element.set_attribute("west",  (meDirectionBlock & StoneRing::DIR_WEST )?"true":"false" );

    return element;
}


Editor::Event::Event()
{
}
Editor::Event::~Event()
{
}
CL_DomElement Editor::Event::createDomElement(CL_DomDocument &doc)const
{
    CL_DomElement element(doc,"event");

    element.set_attribute("name", mName );

    std::string triggertype;

    switch( meTriggerType )
    {
    case STEP:
        triggertype = "step";
        break;
    case TALK:
        triggertype = "talk";
        break;
    case ACT:
        triggertype = "act";
        break;
    }

    element.set_attribute("triggerType", triggertype);

    if(!mbRepeatable) element.set_attribute("repeatable","false");

    if(mbRemember) element.set_attribute("remember","true");

    if(mpCondition)
    {
        CL_DomElement e = mpCondition->createDomElement(doc);
        element.append_child ( e );
    }

    if(mpScript)
    {
        CL_DomElement e = mpScript->createDomElement(doc);
        element.append_child ( e );
    }

    return element;
}


Editor::IconRef::IconRef()
{
}
Editor::IconRef::~IconRef()
{
}
CL_DomElement Editor::IconRef::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"iconRef");
}


Editor::ItemRef::ItemRef()
{
}
Editor::ItemRef::~ItemRef()
{
}
CL_DomElement Editor::ItemRef::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"itemRef");
}


Editor::Level::Level()
{
}
Editor::Level::~Level()
{
}
CL_DomElement Editor::Level::createDomElement(CL_DomDocument &doc)const
{
    CL_DomElement element(doc, "level");

    element.set_attribute("name",mName);

    CL_DomElement levelHeader(doc, "levelHeader");
    CL_DomElement tiles(doc, "tiles");
    CL_DomElement mappableObjects(doc,"mappableObjects");


    levelHeader.set_attribute("music", mMusic );
    levelHeader.set_attribute("width", IntToString(mLevelWidth) );
    levelHeader.set_attribute("height", IntToString(mLevelHeight) );
    levelHeader.set_attribute("allowsRunning", mbAllowsRunning? "true" : "false");

    element.append_child( levelHeader );

    

    for(int x=0; x< mLevelWidth; x++)
    {
        for(int y =0;y< mLevelHeight; y++)
        {
            for( std::list<StoneRing::Tile*>::const_iterator i = mTileMap[x][y].begin();
                 i != mTileMap[x][y].end();
                 i++)
            {
                CL_DomElement tileEl = dynamic_cast<Editor::Tile*>(*i)->createDomElement(doc);

                tiles.append_child ( tileEl );

            }
        }
    }

    for(std::map<CL_Point,std::list<StoneRing::Tile*> >::const_iterator j = mFloaterMap.begin();
        j != mFloaterMap.end();
        j++)
    {
        for(std::list<StoneRing::Tile*>::const_iterator jj = j->second.begin();
            jj != j->second.end();
            jj++)
        {
            CL_DomElement floaterEl = dynamic_cast<Editor::Tile*>(*jj)->createDomElement(doc);

            tiles.append_child ( floaterEl );

        }
        
    }

    element.append_child(tiles);

    ++mnFrameCount; // Use to make sure we don't save each MO more than once. Wee.
 
    for(StoneRing::MOMap::const_iterator iter = mMOMap.begin(); iter != mMOMap.end();
        iter++)
    {
        Editor::MappableObject * pMo = dynamic_cast<Editor::MappableObject*>(iter->second);
        if(mnFrameCount > pMo->getFrameMarks())
        {
            mappableObjects.append_child(pMo->createDomElement(doc));
            pMo->markFrame();
        }
    }

    element.append_child(mappableObjects);
    

    return element;
}

void Editor::Level::drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)
{
}
 

void Editor::Level::addRows(int rows)
{
    for(std::vector<std::vector<std::list<StoneRing::Tile*> > >::iterator iter= mTileMap.begin();
        iter != mTileMap.end();
        iter++)
    {
        iter->resize ( iter->size() + rows );
    }

    mLevelHeight+=rows;
}

void Editor::Level::addColumns(int columns)
{
    int orig_size = static_cast<int>(mTileMap.size());
    mTileMap.resize( orig_size + columns );

    for(int x = orig_size-1; x < mTileMap.size(); x++)
    {
        mTileMap[x].resize ( mLevelHeight );
    }

    mLevelWidth+=columns;
}

void Editor::Level::setName(const std::string &name)
{
    mName = name;
}

void Editor::Level::setMusic(const std::string &music)
{
    mMusic = music;
}

        
void Editor::Level::addTile ( StoneRing::Tile * pTile )
{

    if(pTile->getX() >= mLevelWidth || pTile->getY() >= mLevelHeight )
        return;

    if( pTile->isFloater())
    {
        mFloaterMap[ CL_Point(pTile->getX(),pTile->getY()) ].push_back(pTile);
    }
    else
    {
        mTileMap[ pTile->getX() ][ pTile->getY()].push_back ( pTile );  
    }

    
}

void Editor::Level::removeTile ( StoneRing::Tile * pTile )
{
    if(pTile->isFloater())
    {
        if(mFloaterMap.count( CL_Point(pTile->getX(), pTile->getY() ) ))
        {
            mFloaterMap[CL_Point(pTile->getX(),pTile->getY())].remove( pTile );
        }
    }
    else
    {
        mTileMap[pTile->getX()][pTile->getY()].remove (pTile );
    }
}
 
std::list<StoneRing::Tile*> Editor::Level::getTilesAt(uint levelX, uint levelY) const
{

    std::list<StoneRing::Tile *> tiles = mTileMap[ levelX][levelY];

    
    std::map<CL_Point,std::list<StoneRing::Tile*> >::const_iterator iter = mFloaterMap.find ( CL_Point(levelX,levelY));

    if(iter != mFloaterMap.end())
    {
        for(std::list<StoneRing::Tile*>::const_iterator i = iter->second.begin();
            i != iter->second.end();
            i++)
        {
            tiles.push_back ( *i );
        }
    }
    


    return tiles;

}
    


// Operates on ALL tiles at a location. For finer control, one must operate on the tiles individually.
// bOn of true turns the direction block on for the specified direction,
// false will turn it off.
void Editor::Level::setDirectionBlockAt(uint levelX, uint levelY, StoneRing::eDirectionBlock dir, bool bOn)
{
    
    std::list<StoneRing::Tile*> tiles = getTilesAt(levelX,levelY);

    for(std::list<StoneRing::Tile*>::iterator iter = tiles.begin();
        iter != tiles.end();
        iter++)
    {
        switch(dir)
        {
        case StoneRing::DIR_NORTH:
            dynamic_cast<Editor::Tile*>(*iter)->setNorthBlock(bOn);
            break;
        case StoneRing::DIR_SOUTH:
            dynamic_cast<Editor::Tile*>(*iter)->setSouthBlock(bOn);
            break;
        case StoneRing::DIR_EAST:
            dynamic_cast<Editor::Tile*>(*iter)->setEastBlock(bOn);
            break;
        case StoneRing::DIR_WEST:
            dynamic_cast<Editor::Tile*>(*iter)->setWestBlock(bOn);
            break;
        }
    }
}

void Editor::Level::setHotAt(uint levelX, uint levelY, bool bHot)
{
    std::list<StoneRing::Tile*> tiles = getTilesAt(levelX,levelY);

    for(std::list<StoneRing::Tile*>::iterator iter = tiles.begin();
        iter != tiles.end();
        iter++)
    {
        if(bHot)
            dynamic_cast<Editor::Tile*>(*iter)->setIsHot();
        else dynamic_cast<Editor::Tile*>(*iter)->setNotHot();
    }
}



Editor::LevelHeader::LevelHeader()
{
}
Editor::LevelHeader::~LevelHeader()
{
}
CL_DomElement Editor::LevelHeader::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"levelHeader");
}


Editor::MagicDamageCategory::MagicDamageCategory()
{
}
Editor::MagicDamageCategory::~MagicDamageCategory()
{
}
CL_DomElement Editor::MagicDamageCategory::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"magicDamageCategory");
}


Editor::MagicResistance::MagicResistance()
{
}
Editor::MagicResistance::~MagicResistance()
{
}
CL_DomElement Editor::MagicResistance::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"magicResistance");
}


MappableObject::MappableObject()
{
}
MappableObject::~MappableObject()
{
}
CL_DomElement Editor::MappableObject::createDomElement(CL_DomDocument &doc)const
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
        CL_DomElement tilemapEl = dynamic_cast<Editor::Tilemap*>(mGraphic.asTilemap)->createDomElement(doc);

        element.append_child (  tilemapEl );

    }

    if(isSprite())
    {
    
        CL_DomElement spriteRefEl = dynamic_cast<Editor::SpriteRef*>(mGraphic.asSpriteRef)->createDomElement(doc);

        element.append_child ( spriteRefEl );

    }

    if(mpCondition)
    {
        CL_DomElement condition = mpCondition->createDomElement(doc);

        element.append_child( condition );
    }

    for(std::list<StoneRing::Event*>::const_iterator h = mEvents.begin();
        h != mEvents.end(); h++)
    {
        CL_DomElement evEl= dynamic_cast<Editor::Event*>(*h)->createDomElement(doc);

        element.append_child( evEl );


    }

    if(mpMovement)
    {
        CL_DomElement moveEl = dynamic_cast<Editor::Movement*>(mpMovement)->createDomElement(doc);

        element.append_child ( moveEl );

    }


    return element;
}


Editor::MappableObjects::MappableObjects()
{
}
Editor::MappableObjects::~MappableObjects()
{
}
CL_DomElement Editor::MappableObjects::createDomElement(CL_DomDocument &doc)const
{
   return CL_DomElement(doc,"mappableObjects");
}


Editor::Movement::Movement()
{
}
Editor::Movement::~Movement()
{
}
CL_DomElement Editor::Movement::createDomElement(CL_DomDocument &doc)const
{
    CL_DomElement element(doc,"movement") ;
    
    std::string movementType;

    switch( meType )
    {
    case MOVEMENT_WANDER:
        movementType = "wander";
        break;
    case MOVEMENT_PACE_NS:
        movementType = "paceNS";
        break;
    case MOVEMENT_PACE_EW:
        movementType = "paceEW";
        break;
    case MOVEMENT_NONE:
        movementType = "none";
        break;
    }


    element.set_attribute("movementType", movementType );

    std::string speed;

    switch(meSpeed)
    {
    case SLOW:
        speed = "slow";
        break;
    case FAST:
        speed = "fast";
        break;
    }

    element.set_attribute("speed", speed );

    return element;

}


Editor::NamedItemElement::NamedItemElement()
{
}
Editor::NamedItemElement::~NamedItemElement()
{
}
CL_DomElement Editor::NamedItemElement::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"namedItemElement");
}


Editor::NamedItemRef::NamedItemRef()
{
}
Editor::NamedItemRef::~NamedItemRef()
{
}
CL_DomElement Editor::NamedItemRef::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"namedItemRef");
}


Editor::OnEquip::OnEquip()
{
}
Editor::OnEquip::~OnEquip()
{
}
CL_DomElement Editor::OnEquip::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"onEquip");
}


Editor::OnUnequip::OnUnequip()
{
}
Editor::OnUnequip::~OnUnequip()
{
}
CL_DomElement Editor::OnUnequip::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"onUnequip");
}

Editor::OnRound::OnRound()
{
}
Editor::OnRound::~OnRound()
{
}
CL_DomElement Editor::OnRound::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"onRound");
}

Editor::OnStep::OnStep()
{
}
Editor::OnStep::~OnStep()
{
}
CL_DomElement Editor::OnStep::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"onStep");
}


Editor::OnCountdown::OnCountdown()
{
}
Editor::OnCountdown::~OnCountdown()
{
}
CL_DomElement Editor::OnCountdown::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"onCountdown");
}


Editor::OnInvoke::OnInvoke()
{
}
Editor::OnInvoke::~OnInvoke()
{
}
CL_DomElement Editor::OnInvoke::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"onInvoke");
}

Editor::OnRemove::OnRemove()
{
}
Editor::OnRemove::~OnRemove()
{
}
CL_DomElement Editor::OnRemove::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"onRemove");
}


Editor::Par::Par()
{
}
Editor::Par::~Par()
{
}
CL_DomElement Editor::Par::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"par");
}


Editor::RegularItem::RegularItem()
{
}
Editor::RegularItem::~RegularItem()
{
}
CL_DomElement Editor::RegularItem::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"regularItem");
}


Editor::Rune::Rune()
{
}
Editor::Rune::~Rune()
{
}
CL_DomElement Editor::Rune::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"rune");
}

Editor::RuneType::RuneType()
{
}
Editor::RuneType::~RuneType()
{
}
CL_DomElement Editor::RuneType::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"runeType");
}

Editor::ScriptElement::ScriptElement()
:StoneRing::ScriptElement(false)
{
}
Editor::ScriptElement::~ScriptElement()
{
}
CL_DomElement Editor::ScriptElement::createDomElement(CL_DomDocument &doc)const
{
    CL_DomElement element(doc, isConditionScript()?"conditionScript":"script");

    element.set_attribute("id",mId);

    CL_DomText text ( doc, mScript );
    text.set_node_value ( mScript );

    element.append_child ( text );

    return element;
}


void Editor::ScriptElement::setScript(const std::string &script)
{
    mScript = script;
}

void Editor::ScriptElement::parse()
{
    mpScript = StoneRing::IApplication::getInstance()->loadScript(mId,mScript);
}

void Editor::ScriptElement::handleText(const std::string &script)
{
    mScript = script;
}

Editor::Skill::Skill()
{
}
Editor::Skill::~Skill()
{
}
CL_DomElement Editor::Skill::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"skill");
}

Editor::SkillRef::SkillRef()
{
}
Editor::SkillRef::~SkillRef()
{
}
CL_DomElement Editor::SkillRef::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"skillRef");
}

Editor::SpecialItem::SpecialItem()
{
}
Editor::SpecialItem::~SpecialItem()
{
}
CL_DomElement Editor::SpecialItem::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"specialItem");
}

Editor::Spell::Spell()
{
}
Editor::Spell::~Spell()
{
}
CL_DomElement Editor::Spell::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"spell");
}

Editor::SpellRef::SpellRef()
{
}
Editor::SpellRef::~SpellRef()
{
}
CL_DomElement Editor::SpellRef::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"spellRef");
}

Editor::SpriteRef::SpriteRef()
{
}
Editor::SpriteRef::~SpriteRef()
{
}
CL_DomElement Editor::SpriteRef::createDomElement(CL_DomDocument &doc)const
{
    CL_DomElement  element(doc,"spriteRef");

    std::string dir;

    switch(meType)
    {
    case SPR_STILL:
        dir = "still";
        break;
    case SPR_TWO_WAY:
        dir = "twoway";
        break;
    case SPR_FOUR_WAY:
        dir = "fourway";
        break;
    case SPR_NONE:
        break;
    }

    if(dir.length())
    {
        element.set_attribute("type", dir);
    }

    CL_DomText text(doc,mRef);
    text.set_node_value( mRef );

    element.append_child ( text );

    return element;

}

void Editor::SpriteRef::setSpriteRef( const std::string &ref)
{
    mRef = ref;
}

void Editor::SpriteRef::setType( StoneRing::SpriteRef::eType dir)
{
    meType = dir;
}


Editor::StatIncrease::StatIncrease()
{
}
Editor::StatIncrease::~StatIncrease()
{
}
CL_DomElement Editor::StatIncrease::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"statIncrease");
}

Editor::StatusEffect::StatusEffect()
{
}
Editor::StatusEffect::~StatusEffect()
{
}
CL_DomElement Editor::StatusEffect::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"statusEffect");
}

Editor::StatusEffectModifier::StatusEffectModifier()
{
}
Editor::StatusEffectModifier::~StatusEffectModifier()
{
}
CL_DomElement Editor::StatusEffectModifier::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"statusEffectModifier");
}

Editor::SystemItem::SystemItem()
{
}
Editor::SystemItem::~SystemItem()
{
}
CL_DomElement Editor::SystemItem::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"systemItem");
}

Editor::Tile::Tile()
{
}
Editor::Tile::~Tile()
{
}
CL_DomElement Editor::Tile::createDomElement(CL_DomDocument &doc)const
{ 
    CL_DomElement element(doc,"tile");

    element.set_attribute("xpos", IntToString ( mX ) );
    element.set_attribute("ypos", IntToString ( mY ) );
    if(mZOrder >0 ) element.set_attribute("zorder", IntToString (mZOrder ) );
    if(isFloater()) element.set_attribute("floater", "true");
    if(isHot())     element.set_attribute("hot", "true");
    if(pops())      element.set_attribute("pops","true");

    if(isSprite())
    {
        CL_DomElement spriteEl = dynamic_cast<Editor::SpriteRef*>(mGraphic.asSpriteRef)->createDomElement(doc);

        element.append_child ( spriteEl );

    }
    else
    {
        CL_DomElement tilemapEl = dynamic_cast<Editor::Tilemap*>(mGraphic.asTilemap)->createDomElement(doc);

        element.append_child (  tilemapEl );

    }

    if(mpCondition)
    {
        CL_DomElement condEl = mpCondition->createDomElement(doc);

        element.append_child ( condEl );

    }
    if( getDirectionBlock() > 0)
    {
        Editor::DirectionBlock block( getDirectionBlock() );

        CL_DomElement dirEl = block.createDomElement(doc);

        element.append_child( dirEl );

    }
    

    return element;

}


void Editor::Tile::setLevelX(int x)
{
    mX = x;
}

void Editor::Tile::setLevelY(int y)
{
    mY = y;
}

void Editor::Tile::setZOrder(int z)
{
    mZOrder = z;
}

// Has to have one of these if it's new
void Editor::Tile::setTilemap( const std::string &mapname, uint mapX, uint mapY)
{
    // TODO: Factory method...
    mGraphic.asTilemap = new Editor::Tilemap();   

    dynamic_cast<Editor::Tilemap*>(mGraphic.asTilemap)->setMapName(mapname);
    dynamic_cast<Editor::Tilemap*>(mGraphic.asTilemap)->setMapX(mapX);
    dynamic_cast<Editor::Tilemap*>(mGraphic.asTilemap)->setMapY(mapY);
}

void Editor::Tile::setSpriteRef ( const std::string &spriteRef, StoneRing::SpriteRef::eType direction )
{
    StoneRing::GraphicsManager * GM = StoneRing::GraphicsManager::getInstance();
    mGraphic.asSpriteRef = new Editor::SpriteRef ();

    dynamic_cast<Editor::SpriteRef*>(mGraphic.asSpriteRef)->setSpriteRef ( spriteRef );
    dynamic_cast<Editor::SpriteRef*>(mGraphic.asSpriteRef)->setType( direction );
}


void Editor::Tile::setIsFloater()
{
    cFlags |= FLOATER;
}

void Editor::Tile::setIsHot()
{
    cFlags |= HOT;
}

void Editor::Tile::setDirectionBlock (int dirBlock )
{
    if(dirBlock & StoneRing::DIR_NORTH)
        cFlags |= BLK_NORTH;
    if(dirBlock & StoneRing::DIR_SOUTH)
        cFlags |= StoneRing::DIR_SOUTH;
    if(dirBlock & StoneRing::DIR_WEST)
        cFlags |= StoneRing::DIR_WEST;
    if(dirBlock & StoneRing::DIR_EAST)
        cFlags |= StoneRing::DIR_EAST;
}
    

void Editor::Tile::setNotHot()
{
    cFlags &= ~HOT;
}

void Editor::Tile::setNorthBlock(bool bOn)
{
    if(bOn)
    {
        cFlags |= BLK_NORTH;
    }
    else
    {
        cFlags &= ~BLK_NORTH;
    }
}

void Editor::Tile::setSouthBlock(bool bOn)
{
    if(bOn)
    {
        cFlags |= BLK_SOUTH;
    }
    else
    {
        cFlags &= ~BLK_SOUTH;
    }
}

void Editor::Tile::setEastBlock(bool bOn)
{
    if(bOn)
    {
        cFlags |= BLK_EAST;
    }
    else
    {
        cFlags &= ~BLK_EAST;
    }
}

void Editor::Tile::setWestBlock(bool bOn)
{
    if(bOn)
    {
        cFlags |= BLK_WEST;
    }
    else
    {
        cFlags &= ~BLK_WEST;
    }
}


Editor::Tiles::Tiles()
{
}
Editor::Tiles::~Tiles()
{
}
CL_DomElement Editor::Tiles::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"tiles");
}

Tilemap::Tilemap()
{
}
Tilemap::~Tilemap()
{
}
CL_DomElement Editor::Tilemap::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"tilemap");
 }


void Editor::Tilemap::setMapName(const std::string & mapname)
{
    mpSurface = StoneRing::GraphicsManager::getInstance()->getTileMap(mapname); 
}
void Editor::Tilemap::setMapX(int x)
{
    mX = x;
}
void Tilemap::setMapY(int y)
{
    mY = y;
}



Editor::UniqueArmor::UniqueArmor()
{
}
Editor::UniqueArmor::~UniqueArmor()
{
}
CL_DomElement Editor::UniqueArmor::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"uniqueArmor");
}

Editor::UniqueWeapon::UniqueWeapon()
{
}
Editor::UniqueWeapon::~UniqueWeapon()
{
}
CL_DomElement Editor::UniqueWeapon::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"uniqueWeapon");
}

Editor::WeaponClass::WeaponClass()
{
}
Editor::WeaponClass::~WeaponClass()
{
}
CL_DomElement Editor::WeaponClass::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"weaponClass");
}

Editor::WeaponClassRef::WeaponClassRef()
{
}
Editor::WeaponClassRef::~WeaponClassRef()
{
}
CL_DomElement Editor::WeaponClassRef::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"weaponClassRef");
}

Editor::WeaponDamageCategory::WeaponDamageCategory()
{
}
Editor::WeaponDamageCategory::~WeaponDamageCategory()
{
}
CL_DomElement Editor::WeaponDamageCategory::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"weaponDamageCategory");
}

Editor::WeaponEnhancer::WeaponEnhancer()
{
}
Editor::WeaponEnhancer::~WeaponEnhancer()
{
}
CL_DomElement Editor::WeaponEnhancer::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"weaponEnhancer");
}

Editor::WeaponRef::WeaponRef()
{
}
Editor::WeaponRef::~WeaponRef()
{
}
CL_DomElement Editor::WeaponRef::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"weaponRef");
}

Editor::WeaponType::WeaponType()
{
}
Editor::WeaponType::~WeaponType()
{
}
CL_DomElement Editor::WeaponType::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"weaponType");
}

Editor::WeaponTypeExclusionList::WeaponTypeExclusionList()
{
}
Editor::WeaponTypeExclusionList::~WeaponTypeExclusionList()
{
}
CL_DomElement Editor::WeaponTypeExclusionList::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"weaponTypeExclusionList");
}

Editor::WeaponTypeRef::WeaponTypeRef()
{
}
Editor::WeaponTypeRef::~WeaponTypeRef()
{
}
CL_DomElement Editor::WeaponTypeRef::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"weaponTypeRef");
}

Editor::WeaponTypeSprite::WeaponTypeSprite()
{
}
Editor::WeaponTypeSprite::~WeaponTypeSprite()
{
}
CL_DomElement Editor::WeaponTypeSprite::createDomElement(CL_DomDocument &doc)const
{
    return CL_DomElement(doc,"weaponTypeSprite");
}
