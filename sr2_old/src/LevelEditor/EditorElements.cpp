#include "EditorElements.h"
#include "IApplication.h"
#include "GraphicsManager.h"
#include "Magic.h"

using namespace Editor;
using StoneRing::Magic;


Editor::SpriteDefinition::SpriteDefinition()
{
}

Editor::SpriteDefinition::~SpriteDefinition()
{
}

clan::DomElement Editor::SpriteDefinition::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"spriteDefinition");
}

Editor::SpriteAnimation::SpriteAnimation()
{
}

Editor::SpriteAnimation::~SpriteAnimation()
{
}

clan::DomElement Editor::SpriteAnimation::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"spriteAnimation");
}
 

Editor::AlterSprite::AlterSprite()
{
}

Editor::AlterSprite::~AlterSprite()
{
}

clan::DomElement Editor::AlterSprite::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"alterSprite");
}

Editor::SpriteMovement::SpriteMovement()
{
}

Editor::SpriteMovement::~SpriteMovement()
{
}

clan::DomElement Editor::SpriteMovement::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"spriteMovement");
}

Editor::Animation::Animation()
{
}
Editor::Animation::~Animation()
{
}
clan::DomElement Editor::Animation::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"animation");
}


Editor::ArmorClass::ArmorClass()
{
}
Editor::ArmorClass::~ArmorClass()
{
}
clan::DomElement Editor::ArmorClass::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"armorClass");
}


Editor::ArmorClassRef::ArmorClassRef()
{
}
Editor::ArmorClassRef::~ArmorClassRef()
{
}
clan::DomElement Editor::ArmorClassRef::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc,"armorClassRef");
    clan::DomText text(doc, mName );

    text.set_node_value ( mName );
    element.append_child ( text );

    return element;
}

Editor::ArmorEnhancer::ArmorEnhancer()
{
}
Editor::ArmorEnhancer::~ArmorEnhancer()
{
}
clan::DomElement Editor::ArmorEnhancer::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"armorEnhancer");
}


Editor::ArmorRef::ArmorRef()
{
}
Editor::ArmorRef::~ArmorRef()
{
}
clan::DomElement Editor::ArmorRef::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc,"armorRef");

    
    WRITE_CHILD(element,mpType,doc);
    WRITE_CHILD(element,mpClass,doc);

    if(mpSpellRef )
    {
        WRITE_CHILD(element,mpSpellRef,doc);
    }
    else if (mpRuneType)
    {
        WRITE_CHILD(element,mpRuneType,doc);
    }

    return element;
}


Editor::ArmorType::ArmorType()
{
}
Editor::ArmorType::~ArmorType()
{
}
clan::DomElement Editor::ArmorType::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"armorType");
}


Editor::ArmorTypeExclusionList::ArmorTypeExclusionList()
{
}
Editor::ArmorTypeExclusionList::~ArmorTypeExclusionList()
{
}
clan::DomElement Editor::ArmorTypeExclusionList::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"armorTypeExclusionList");
}


Editor::ArmorTypeRef::ArmorTypeRef()
{
}
Editor::ArmorTypeRef::~ArmorTypeRef()
{
}
clan::DomElement Editor::ArmorTypeRef::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc,"armorTypeRef");
    clan::DomText text(doc, mName );

    text.set_node_value ( mName );
    element.append_child ( text );

    return element;
}


Editor::attributeModifier::attributeModifier()
{
}
Editor::attributeModifier::~attributeModifier()
{
}
clan::DomElement Editor::attributeModifier::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"attributeModifier");
}


Editor::BattleMenu::BattleMenu()
{
}
Editor::BattleMenu::~BattleMenu()
{
}
clan::DomElement Editor::BattleMenu::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"battleMenu");
}


Editor::BattleMenuOption::BattleMenuOption()
{
}
Editor::BattleMenuOption::~BattleMenuOption()
{
}
clan::DomElement Editor::BattleMenuOption::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"battleMenuOption");
}


Editor::CharacterClass::CharacterClass()
{
}
Editor::CharacterClass::~CharacterClass()
{
}
clan::DomElement Editor::CharacterClass::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"characterClass");
}


Editor::CharacterDefinition::CharacterDefinition()
{
}
Editor::CharacterDefinition::~CharacterDefinition()
{
}
clan::DomElement Editor::CharacterDefinition::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"characterDefinition");
}


Editor::ConditionScript::ConditionScript()
:ScriptElement(true)
{
}
Editor::ConditionScript::~ConditionScript()
{
}
clan::DomElement Editor::ConditionScript::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"conditionScript");
}


Editor::SideBlock::SideBlock()
{
}

Editor::SideBlock::SideBlock(int i)
:StoneRing::SideBlock(i)
{
}

Editor::SideBlock::~SideBlock()
{
}
clan::DomElement Editor::SideBlock::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc,"block");

    element.set_attribute("north", (meSideBlock & StoneRing::DIR_NORTH )?"true":"false");
    element.set_attribute("south", (meSideBlock & StoneRing::DIR_SOUTH)?"true":"false");
    element.set_attribute("east",  (meSideBlock & StoneRing::DIR_EAST )?"true":"false" );
    element.set_attribute("west",  (meSideBlock & StoneRing::DIR_WEST )?"true":"false" );

    return element;
}


Editor::Event::Event()
{
}
Editor::Event::~Event()
{
}
clan::DomElement Editor::Event::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc,"event");

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
        WRITE_CHILD(element,mpCondition,doc);
    }

    if(mpScript)
    {
        WRITE_CHILD(element,mpScript,doc);
    }

    return element;
}


Editor::IconRef::IconRef()
{
}
Editor::IconRef::~IconRef()
{
}
clan::DomElement Editor::IconRef::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"iconRef");
}


Editor::ItemRef::ItemRef()
{
}
Editor::ItemRef::~ItemRef()
{
}
clan::DomElement Editor::ItemRef::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc, std::string("itemRef"));

    switch(meType)
    {
    case NAMED_ITEM:
        WRITE_CHILD(element,mpNamedItemRef,doc);
        break;
    case WEAPON_REF:
        WRITE_CHILD(element,mpWeaponRef,doc);
        break;
    case ARMOR_REF:
        WRITE_CHILD(element,mpArmorRef,doc);
        break;
    }

    return element;
}


Editor::Level::Level()
{
}
Editor::Level::~Level()
{
}
clan::DomElement Editor::Level::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc, "level");

    element.set_attribute("name",mName);

    clan::DomElement levelHeader(doc, "levelHeader");
    clan::DomElement tiles(doc, "tiles");
    clan::DomElement mappableObjects(doc,"mappableObjects");


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
                clan::DomElement tileEl = dynamic_cast<Editor::Tile*>(*i)->createDomElement(doc);

                tiles.append_child ( tileEl );

            }
        }
    }

    for(std::map<clan::Point,std::list<StoneRing::Tile*> >::const_iterator j = mFloaterMap.begin();
        j != mFloaterMap.end();
        j++)
    {
        for(std::list<StoneRing::Tile*>::const_iterator jj = j->second.begin();
            jj != j->second.end();
            jj++)
        {
            clan::DomElement floaterEl = dynamic_cast<Editor::Tile*>(*jj)->createDomElement(doc);

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

void Editor::Level::drawMappableObjects(const clan::Rect &src, const clan::Rect &dst, clan::Canvas *pGC)
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
        mFloaterMap[ clan::Point(pTile->getX(),pTile->getY()) ].push_back(pTile);
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
        if(mFloaterMap.count( clan::Point(pTile->getX(), pTile->getY() ) ))
        {
            mFloaterMap[clan::Point(pTile->getX(),pTile->getY())].remove( pTile );
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

    
    std::map<clan::Point,std::list<StoneRing::Tile*> >::const_iterator iter = mFloaterMap.find ( clan::Point(levelX,levelY));

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
void Editor::Level::setSideBlockAt(uint levelX, uint levelY, StoneRing::eSideBlock dir, bool bOn)
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
clan::DomElement Editor::LevelHeader::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"levelHeader");
}


Editor::MagicDamageCategory::MagicDamageCategory()
{
}
Editor::MagicDamageCategory::~MagicDamageCategory()
{
}
clan::DomElement Editor::MagicDamageCategory::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"magicDamageCategory");
}


Editor::MagicResistance::MagicResistance()
{
}
Editor::MagicResistance::~MagicResistance()
{
}
clan::DomElement Editor::MagicResistance::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"magicResistance");
}


MappableObject::MappableObject()
{
}
MappableObject::~MappableObject()
{
}
clan::DomElement Editor::MappableObject::createDomElement(clan::DomDocument &doc)const
{    
    clan::DomElement element(doc,"mo");

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
        WRITE_CHILD(element,mGraphic.asTilemap,doc);
    }

    if(isSprite())
    {
        WRITE_CHILD(element,mGraphic.asSpriteRef,doc);
    }

    if(mpCondition)
    {
        WRITE_CHILD(element,mpCondition,doc);
    }

    for(std::list<StoneRing::Event*>::const_iterator h = mEvents.begin();
        h != mEvents.end(); h++)
    {
        WRITE_CHILD(element,*h,doc);
    }

    if(mpMovement)
    {
        WRITE_CHILD(element,mpMovement,doc);
    }


    return element;
}


Editor::MappableObjects::MappableObjects()
{
}
Editor::MappableObjects::~MappableObjects()
{
}
clan::DomElement Editor::MappableObjects::createDomElement(clan::DomDocument &doc)const
{
   return clan::DomElement(doc,"mappableObjects");
}

Editor::Monster::Monster()
{
}

Editor::Monster::~Monster()
{
}

clan::DomElement Editor::Monster::createDomElement(clan::DomDocument &doc) const
{
    clan::DomElement element(doc,"monster");

    return element;
}


Editor::Movement::Movement()
{
}
Editor::Movement::~Movement()
{
}
clan::DomElement Editor::Movement::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc,"movement") ;
    
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
clan::DomElement Editor::NamedItemElement::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"namedItemElement");
}


Editor::NamedItemRef::NamedItemRef()
{
}
Editor::NamedItemRef::~NamedItemRef()
{
}
clan::DomElement Editor::NamedItemRef::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc,"namedItemRef");
    clan::DomText text(doc, mName );

    text.set_node_value ( mName );
    element.append_child ( text );

    return element;
}


Editor::OnEquip::OnEquip()
{
}
Editor::OnEquip::~OnEquip()
{
}
clan::DomElement Editor::OnEquip::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"onEquip");
}


Editor::OnUnequip::OnUnequip()
{
}
Editor::OnUnequip::~OnUnequip()
{
}
clan::DomElement Editor::OnUnequip::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"onUnequip");
}

Editor::OnRound::OnRound()
{
}
Editor::OnRound::~OnRound()
{
}
clan::DomElement Editor::OnRound::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"onRound");
}

Editor::OnStep::OnStep()
{
}
Editor::OnStep::~OnStep()
{
}
clan::DomElement Editor::OnStep::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"onStep");
}


Editor::OnCountdown::OnCountdown()
{
}
Editor::OnCountdown::~OnCountdown()
{
}
clan::DomElement Editor::OnCountdown::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"onCountdown");
}


Editor::OnInvoke::OnInvoke()
{
}
Editor::OnInvoke::~OnInvoke()
{
}
clan::DomElement Editor::OnInvoke::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"onInvoke");
}

Editor::OnRemove::OnRemove()
{
}
Editor::OnRemove::~OnRemove()
{
}
clan::DomElement Editor::OnRemove::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"onRemove");
}


Editor::Phase::Phase()
{
}
Editor::Phase::~Phase()
{
}
clan::DomElement Editor::Phase::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"Phase");
}


Editor::RegularItem::RegularItem()
{
}
Editor::RegularItem::~RegularItem()
{
}
clan::DomElement Editor::RegularItem::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"regularItem");
}

Editor::SpriteStub::SpriteStub()
{
}
Editor::SpriteStub::~SpriteStub()
{
}
clan::DomElement Editor::SpriteStub::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"spriteStub");
}


Editor::Rune::Rune()
{
}
Editor::Rune::~Rune()
{
}
clan::DomElement Editor::Rune::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"rune");
}

Editor::RuneType::RuneType()
{
}
Editor::RuneType::~RuneType()
{
}
clan::DomElement Editor::RuneType::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc,"runeType");

    switch(meRuneType)
    {
    case NONE:
        element.set_attribute("type", "none");
        break;
    case RUNE:
        element.set_attribute("type","rune");
        break;
    case ULTRA_RUNE:
        element.set_attribute("type","ultraRune");
        break;
    }

    return element;
}

Editor::ScriptElement::ScriptElement()
:StoneRing::ScriptElement(false)
{
}
Editor::ScriptElement::~ScriptElement()
{
}
clan::DomElement Editor::ScriptElement::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc, isConditionScript()?"conditionScript":"script");

    element.set_attribute("id",mId);

    clan::DomText text ( doc, mScript );
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
clan::DomElement Editor::Skill::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"skill");
}

Editor::SkillRef::SkillRef()
{
}
Editor::SkillRef::~SkillRef()
{
}
clan::DomElement Editor::SkillRef::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"skillRef");
}

Editor::SpecialItem::SpecialItem()
{
}
Editor::SpecialItem::~SpecialItem()
{
}
clan::DomElement Editor::SpecialItem::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"specialItem");
}

Editor::Spell::Spell()
{
}
Editor::Spell::~Spell()
{
}
clan::DomElement Editor::Spell::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"spell");
}

Editor::SpellRef::SpellRef()
{
}
Editor::SpellRef::~SpellRef()
{
}
clan::DomElement Editor::SpellRef::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc,"spellRef");

    std::string spellType = Magic::toString(meSpellType);

    element.set_attribute("type", spellType );

    clan::DomText text(doc, mName );

    text.set_node_value ( mName );

    element.append_child ( text );

    return element;
}

Editor::SpriteRef::SpriteRef()
{
}
Editor::SpriteRef::~SpriteRef()
{
}
clan::DomElement Editor::SpriteRef::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement  element(doc,"spriteRef");

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

    clan::DomText text(doc,mRef);
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


Editor::StatScript::StatScript()
{
}
Editor::StatScript::~StatScript()
{
}
clan::DomElement Editor::StatScript::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"statScript");
}

Editor::StatusEffect::StatusEffect()
{
}
Editor::StatusEffect::~StatusEffect()
{
}
clan::DomElement Editor::StatusEffect::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"statusEffect");
}

Editor::StatusEffectModifier::StatusEffectModifier()
{
}
Editor::StatusEffectModifier::~StatusEffectModifier()
{
}
clan::DomElement Editor::StatusEffectModifier::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"statusEffectModifier");
}

Editor::SystemItem::SystemItem()
{
}
Editor::SystemItem::~SystemItem()
{
}
clan::DomElement Editor::SystemItem::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"systemItem");
}

Editor::Tile::Tile()
{
}
Editor::Tile::~Tile()
{
}
clan::DomElement Editor::Tile::createDomElement(clan::DomDocument &doc)const
{ 
    clan::DomElement element(doc,"tile");

    element.set_attribute("xpos", IntToString ( mX ) );
    element.set_attribute("ypos", IntToString ( mY ) );
    if(mZOrder >0 ) element.set_attribute("zorder", IntToString (mZOrder ) );
    if(isFloater()) element.set_attribute("floater", "true");
    if(isHot())     element.set_attribute("hot", "true");
    if(pops())      element.set_attribute("pops","true");

    if(isSprite())
    {
        WRITE_CHILD(element,mGraphic.asSpriteRef,doc);
    }
    else
    {
        WRITE_CHILD(element,mGraphic.asTilemap,doc);
    }

    if(mpCondition)
    {
        WRITE_CHILD(element,mpCondition,doc);
    }
    if( getSideBlock() > 0)
    {
        Editor::SideBlock block( getSideBlock() );
        clan::DomElement dirEl = block.createDomElement(doc);

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

void Editor::Tile::setSideBlock (int dirBlock )
{
    if(dirBlock & StoneRing::DIR_NORTH)
        cFlags |= TBK_NORTH;
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
        cFlags |= TBK_NORTH;
    }
    else
    {
        cFlags &= ~TBK_NORTH;
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
clan::DomElement Editor::Tiles::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"tiles");
}

Tilemap::Tilemap()
{
}
Tilemap::~Tilemap()
{
}
clan::DomElement Editor::Tilemap::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc,"tilemap");

    element.set_attribute( "mapname" , StoneRing::GraphicsManager::getInstance()->lookUpMapWithSurface ( mpSurface ) );
    element.set_attribute("mapx", IntToString(mX));
    element.set_attribute("mapy", IntToString(mY));

    return element;
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
clan::DomElement Editor::UniqueArmor::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"uniqueArmor");
}

Editor::UniqueWeapon::UniqueWeapon()
{
}
Editor::UniqueWeapon::~UniqueWeapon()
{
}
clan::DomElement Editor::UniqueWeapon::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"uniqueWeapon");
}

Editor::WeaponClass::WeaponClass()
{
}
Editor::WeaponClass::~WeaponClass()
{
}
clan::DomElement Editor::WeaponClass::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"weaponClass");
}

Editor::WeaponClassRef::WeaponClassRef()
{
}
Editor::WeaponClassRef::~WeaponClassRef()
{
}
clan::DomElement Editor::WeaponClassRef::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc,"weaponClassRef");
    clan::DomText text(doc, mName );

    text.set_node_value ( mName );
    element.append_child ( text );

    return element;
}

Editor::WeaponDamageCategory::WeaponDamageCategory()
{
}
Editor::WeaponDamageCategory::~WeaponDamageCategory()
{
}
clan::DomElement Editor::WeaponDamageCategory::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"weaponDamageCategory");
}

Editor::WeaponEnhancer::WeaponEnhancer()
{
}
Editor::WeaponEnhancer::~WeaponEnhancer()
{
}
clan::DomElement Editor::WeaponEnhancer::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"weaponEnhancer");
}

Editor::WeaponRef::WeaponRef()
{
}
Editor::WeaponRef::~WeaponRef()
{
}
clan::DomElement Editor::WeaponRef::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc,"weaponRef");

    WRITE_CHILD(element,mpType,doc);
    WRITE_CHILD(element,mpClass,doc);

    if(mpSpellRef)
    {
        WRITE_CHILD(element,mpSpellRef,doc);
    }
    else if (mpRuneType)
    {
        WRITE_CHILD(element,mpRuneType,doc);
    }

    return element;
}

Editor::WeaponType::WeaponType()
{
}
Editor::WeaponType::~WeaponType()
{
}
clan::DomElement Editor::WeaponType::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"weaponType");
}

Editor::WeaponTypeExclusionList::WeaponTypeExclusionList()
{
}
Editor::WeaponTypeExclusionList::~WeaponTypeExclusionList()
{
}
clan::DomElement Editor::WeaponTypeExclusionList::createDomElement(clan::DomDocument &doc)const
{
    return clan::DomElement(doc,"weaponTypeExclusionList");
}

Editor::WeaponTypeRef::WeaponTypeRef()
{
}
Editor::WeaponTypeRef::~WeaponTypeRef()
{
}
clan::DomElement Editor::WeaponTypeRef::createDomElement(clan::DomDocument &doc)const
{
    clan::DomElement element(doc,"weaponTypeRef");
    clan::DomText text(doc, mName );

    text.set_node_value ( mName );
    element.append_child ( text );

    return element;
}


