#include <ClanLib/core.h>
#include <string>
#include <map>
#include <list>
#include "IApplication.h"
#include "Level.h"
#include <algorithm>
#include <stdlib.h>
#include <time.h>
#include <set>
#include <functional>
#include <sstream>
#include "GraphicsManager.h"
#include "LevelFactory.h"
#include "ItemFactory.h"
#include "ItemManager.h"
#include "Character.h"

using namespace StoneRing;

template<typename MapType,
         typename KeyArgType,
         typename ValueArgType>
typename MapType::iterator
efficientAddOrUpdate(MapType &m,
                     const KeyArgType &k,
                     const ValueArgType &v)
{
    typename MapType::iterator lb = m.lower_bound(k);

    if(lb != m.end() &&
       !(m.key_comp()(k,lb->first)))
    {
        lb->second = v;
        return lb;
    }
    else
    {
        typedef typename MapType::value_type MVT;
        return m.insert(lb,MVT(k,v));
    }
}


std::string BoolToString( const bool &b)
{
    if(b) return "true";
    else return "false";
}


Option::Option():mpCondition(NULL)
{
}


void Option::handleElement(eElement element, Element * pElement )
{
    switch(element)
    {
    case ECONDITION:
        mpCondition = dynamic_cast<Condition *>(pElement);
        break;
    case EATTRIBUTEMODIFIER:
    case ESAY:
    case EGIVE:
    case ETAKE:
    case EPLAYANIMATION:
    case EPLAYSOUND:
    case ELOADLEVEL:
    case ESTARTBATTLE:
    case EPAUSE:
    case EINVOKESHOP:
    case EGIVEGOLD:
    case ECHOICE:

        mActions.push_back ( dynamic_cast<Action*>(pElement) );
        break;
    default:
        throw CL_Error("Found wacky element in option");
    }
}

void Option::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mText = getRequiredString("text",pAttributes);
}

    
Option::~Option()
{
    delete mpCondition;

    for(std::list<Action*>::const_iterator iter = mActions.begin();
        iter != mActions.end();
        iter++)
    {
        delete *iter;
    }
}

CL_DomElement  Option::createDomElement(CL_DomDocument &document) const
{

    CL_DomElement  element(document,"option");

    element.set_attribute("text",mText );


    for(std::list<Action*>::const_iterator i = mActions.begin();
        i != mActions.end();
        i++)
    {
        CL_DomElement  e = (*i)->createDomElement(document);
        element.append_child(e );

    }

    return element;
    
}

std::string Option::getText() const
{
    return mText;
}
    
bool Option::evaluateCondition() const
{
    if(!mpCondition) return true;

    return mpCondition->evaluate();
}

void Option::choose()
{
#ifndef _MSC_VER
    if(evaluateCondition())
        std::for_each( mActions.begin(), mActions.end(), std::mem_fun(&Action::invoke) );
#else
    if(evaluateCondition())
    {
        for(std::list<Action*>::iterator iter = mActions.begin();
            iter != mActions.end();
            iter++)
        {
            (*iter)->invoke();
        }
    }
#endif
}



Choice::Choice()
{
}


void Choice::handleElement(eElement element, Element * pElement )
{
    switch(element)
    {
    case EOPTION:
        mOptions.push_back ( dynamic_cast<Option*>(pElement) );
        break;
    default:
        
        break;
    }
}

void Choice::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mText = getRequiredString("text",pAttributes);
}

void Choice::loadFinished()
{
    if(!mOptions.size())
        throw CL_Error("Choice missing options!!!");

}

CL_DomElement Choice::createDomElement(CL_DomDocument &doc) const
{

    CL_DomElement  element(doc,"choice");

    element.set_attribute("text",mText );


    for(std::vector<Option*>::const_iterator i = mOptions.begin();
        i != mOptions.end();
        i++)
    {
        CL_DomElement  e = (*i)->createDomElement(doc);
        element.append_child(e );

    }

    return element;

}

void Choice::invoke()
{
    std::vector<std::string> options;


    // fill the "options" vector with the text of each Option object
#ifndef _MSC_VER
    std::transform(mOptions.begin(), mOptions.end(), std::back_inserter(options),
                   std::mem_fun(&Option::getText));
#else
    for(std::vector<Option*>::iterator iter = mOptions.begin();
        iter != mOptions.end();
        iter++)
    {   
        options.push_back ( (*iter)->getText() );
    }
#endif

    IApplication::getInstance()->choice( mText, options, this );

}

std::string Choice::getText() const
{
    return mText;
}

uint Choice::getOptionCount() const
{
    return mOptions.size();
}

Option * Choice::getOption(uint index )
{
    return mOptions[index];
}

void Choice::chooseOption( uint index)
{
    mOptions[index]->choose();
}



// For the multimap of points
bool operator < (const CL_Point &p1, const CL_Point &p2)
{
    uint p1value = (p1.y  *  IApplication::getInstance()->getScreenWidth()) + p1.x;
    uint p2value = (p2.y  * IApplication::getInstance()->getScreenWidth()) + p2.x;
    
    return p1value < p2value;
}

bool operator < (const MappableObject::eDirection dir1, const MappableObject::eDirection dir2)
{
    return (int)dir1 < (int)dir2;
}



void ItemRef::handleElement(eElement element, Element * pElement )
{
    switch(element)
    {
    case ENAMEDITEMREF:
        meType = NAMED_ITEM;
        mpNamedItemRef = dynamic_cast<NamedItemRef*>(pElement);
        break;
    case EWEAPONREF:
        meType = WEAPON_REF;
        mpWeaponRef = dynamic_cast<WeaponRef*>(pElement);
        break;
    case EARMORREF:
        meType = ARMOR_REF;
        mpArmorRef = dynamic_cast<ArmorRef*>(pElement);
        break;
    default:
        
        break;
    }
}

void ItemRef::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    
}

void ItemRef::loadFinished()
{

    const ItemManager * pItemManager = IApplication::getInstance()->getItemManager();

    if(!mpNamedItemRef && !mpWeaponRef && !mpArmorRef)
    {
        throw CL_Error("Item Ref with no child");
    }
    
    mpItem = pItemManager->getItem ( *this ); 

}


ItemRef::ItemRef( ):
    mpNamedItemRef(NULL),mpWeaponRef(NULL),mpArmorRef(NULL)
{
 
  
}


CL_DomElement ItemRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc, std::string("itemRef"));


    switch(meType)
    {
    case NAMED_ITEM:
        element.append_child ( mpNamedItemRef->createDomElement(doc) );
        break;
    case WEAPON_REF:
        element.append_child ( mpWeaponRef->createDomElement(doc) );
        break;
    case ARMOR_REF:
        element.append_child ( mpArmorRef->createDomElement(doc) );
        break;
    }

    return element;

}

ItemRef::~ItemRef()
{
    delete mpNamedItemRef;
    delete mpWeaponRef;
    delete mpArmorRef;

}

#if 0
std::string ItemRef::getItemName() const
{
    switch ( meType )
    {
    case NAMED_ITEM:
        return mpNamedItemRef->getItemName();
        break;
    case WEAPON_REF:
        // Need a standard way to generate names from these suckers
        break;
    case ARMOR_REF:
        break;
    }
}
#endif

ItemRef::eRefType ItemRef::getType() const
{
    return meType;
}

NamedItemRef * ItemRef::getNamedItemRef() const
{
    return mpNamedItemRef;
}

WeaponRef * ItemRef::getWeaponRef() const
{
    return mpWeaponRef;
}

ArmorRef * ItemRef::getArmorRef() const
{
    return mpArmorRef;
}



NamedItemRef::NamedItemRef()
{
}

NamedItemRef:: ~NamedItemRef()
{
}


std::string NamedItemRef::getItemName()
{
    return mName;
}

void NamedItemRef::handleText(const std::string &text)
{
    mName = text;
}

CL_DomElement  NamedItemRef::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"namedItemRef");

    CL_DomText text(doc, mName );

    text.set_node_value ( mName );

    element.append_child ( text );

    return element;
}


CL_DomElement  Tilemap::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"tilemap");

    element.set_attribute( "mapname" , GraphicsManager::getInstance()->lookUpMapWithSurface ( mpSurface ) );
    element.set_attribute( "mapx", IntToString ( mX ) );
    element.set_attribute( "mapy", IntToString ( mY ) );

    return element;
}

void Tilemap::handleElement(eElement element, Element * pElement)
{

}

void Tilemap::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string name = getRequiredString("mapname",pAttributes);
    mpSurface = GraphicsManager::getInstance()->getTileMap(name);
    mX = getRequiredInt("mapx",pAttributes);
    mY = getRequiredInt("mapy",pAttributes);
}


Tilemap::Tilemap():mpSurface(NULL)
{
    

}
 
Tilemap::~Tilemap()
{
}
      
ushort Tilemap::getMapX() const
{
    return mX;
}

ushort Tilemap::getMapY() const
{
    return mY;
}

CL_Surface* Tilemap::getTileMap() const
{
    return mpSurface;
}

CL_DomElement  SpriteRef::createDomElement(CL_DomDocument &doc) const
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


void SpriteRef::handleElement(eElement element, Element * pElement)
{

}

void SpriteRef::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    if(hasAttr("type",pAttributes))
    {
        std::string direction = getString("type",pAttributes);
        
        if(direction == "still") meType = SPR_STILL;
        if(direction == "twoway") meType = SPR_TWO_WAY;
        if(direction == "fourway")  meType = SPR_FOUR_WAY;

    }

}

void SpriteRef::handleText(const std::string &text)
{
    mRef = text;
}


SpriteRef::SpriteRef( ):meType(SPR_NONE)
{
  
}

SpriteRef::~SpriteRef()
{
}

std::string SpriteRef::getRef() const
{
    return mRef;
}

SpriteRef::eType SpriteRef::getType() const
{
    return meType;
}

CL_DomElement  AttributeModifier::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"attributeModifier");

    element.set_attribute("attribute",CAToString(mnAttribute) );

    if(mAdd)
        element.set_attribute("add", IntToString ( mAdd )  );
	if(mfMultiplier != 1)
		element.set_attribute("multiplier",FloatToString(mfMultiplier ) );

    switch ( meTarget )
    {
    case CURRENT:
        element.set_attribute("target","current");
        break;
    case ALL:
        element.set_attribute("target","all");
        break;
    case CASTER:
        element.set_attribute("target","caster");
        break;
	case COMMON:
		element.set_attribute("target","common");
		break;
    }

    switch ( meChangeTo )
    {
    case ADD:
        element.set_attribute("changeTo","add");
        break;
	case MULTIPLIER:
		element.set_attribute("changeTo","multiplier");
		break;
	case MULTIPLY_ADD:
		element.set_attribute("changeTo","multiply_add");
		break;
    case TO_MIN:
        element.set_attribute("changeTo", "min");
        break;
    case TO_MAX:
        element.set_attribute("changeTo", "max");
        break;
    }


    for(std::list<Condition*>::const_iterator i = mConditions.begin();
        i != mConditions.end();
        i++)
    {
        CL_DomElement  e = (*i)->createDomElement(doc);
        element.append_child(e );

    }

    return element;

}

void AttributeModifier::handleElement(eElement element, Element *pElement)
{
    switch(element)
    {
    case ECONDITION:
    {
        Condition * pCondition = dynamic_cast<Condition*>(pElement);
        mConditions.push_back ( pCondition );
        break;
    }
    default:
        throw CL_Error("Bad element in AM");
    }
}

void AttributeModifier::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mnAttribute = CAFromString(getRequiredString("attribute",pAttributes));
    mAdd = getImpliedInt("add",pAttributes,0);
	mfMultiplier = getImpliedFloat("multiplier",pAttributes,1);
        
    std::string changeTo = getRequiredString("changeTo",pAttributes);

    if(changeTo == "max") meChangeTo = TO_MAX;
    else if (changeTo == "min") meChangeTo = TO_MIN;
    else if (changeTo == "add") meChangeTo = ADD;
	else if (changeTo == "multiplier") meChangeTo = MULTIPLIER;
	else if (changeTo == "multiply_add") meChangeTo = MULTIPLY_ADD;
    else throw CL_Error("Unrecognized changeTo type on AM: " + changeTo);

    if(hasAttr("target",pAttributes))
    {
        std::string target = getString("target",pAttributes);

        if(target == "current")
        {
            meTarget = CURRENT;
        }
        else if (target == "all")
        {
            meTarget = ALL;
        }
        else if (target == "caster")
        {
            meTarget = CASTER;
        }
		else if (target == "common")
		{
			meTarget = COMMON;
		}
        else throw CL_Error("Unrecognized target type in attribute modifier: " + target);

    }

        
}

 
AttributeModifier::AttributeModifier ():mAdd(0),meTarget(CURRENT)
{
}

AttributeModifier::~AttributeModifier()
{
    for(std::list<Condition*>::iterator i = mConditions.begin();
        i != mConditions.end();
        i++)
    {
        delete *i;
    }
}


bool AttributeModifier::applicable() const
{
    for( std::list<Condition*>::const_iterator i = mConditions.begin();
         i != mConditions.end();
         i++)
    {
        Condition * condition = *i;
        if( ! condition->evaluate() ) return false;
    }


    ICharacterGroup * pParty = IApplication::getInstance()->getSelectedCharacterGroup();
    ICharacter * pCharacter = NULL;


    switch(meTarget)
    {
    case CURRENT:
        pCharacter = pParty->getSelectedCharacter();
        break;
    case CASTER:
        pCharacter = pParty->getCasterCharacter();
        break;
    }


    //@todo: Act on ALL or CURRENT

    switch(meChangeTo)
    {
    case TO_MAX:
        if( pCharacter->getAttribute(static_cast<eCharacterAttribute>(mnAttribute)) 
			< pCharacter->getMaxAttribute(static_cast<eCharacterAttribute>(mnAttribute)))
        {
            return true;
        }
        else return false;
    case TO_MIN:
        if( pCharacter->getAttribute(static_cast<eCharacterAttribute>(mnAttribute)) 
			> pCharacter->getMinAttribute(static_cast<eCharacterAttribute>(mnAttribute)))
        {
            return true;
        }
        else return false;
    case ADD:
        if(mAdd > 0 )
        {
            if ( pCharacter->getAttribute(static_cast<eCharacterAttribute>(mnAttribute)) 
				< pCharacter->getMaxAttribute(static_cast<eCharacterAttribute>(mnAttribute)))
            {
                return true;
            }
            else return false;
        }
        else // counts 0, but... that does nothing anyway
        {
            if ( pCharacter->getAttribute(static_cast<eCharacterAttribute>(mnAttribute))
				> pCharacter->getMinAttribute(static_cast<eCharacterAttribute>(mnAttribute)))
            {
                return true;
            }
            else return false;
        }
    }

    

}


void AttributeModifier::invoke()
{
    for( std::list<Condition*>::iterator i = mConditions.begin();
         i != mConditions.end();
         i++)
    {
        Condition * condition = *i;
        if( ! condition->evaluate() ) return;
    }


    ICharacterGroup * pParty = IApplication::getInstance()->getSelectedCharacterGroup();
    ICharacter * pCharacter = NULL;


    switch(meTarget)
    {
    case CURRENT:
        pCharacter = pParty->getSelectedCharacter();
        break;
    case CASTER:
        pCharacter = pParty->getCasterCharacter();
        break;
    }

    int add = 0;

    //@todo: ALL or CURRENT


    switch(meChangeTo)
    {
    case TO_MAX:
        add = pCharacter->getMaxAttribute(static_cast<eCharacterAttribute>(mnAttribute)) - 
            pCharacter->getAttribute(static_cast<eCharacterAttribute>(mnAttribute));
        break;
    case TO_MIN:
        add = 0 - (pCharacter->getAttribute(static_cast<eCharacterAttribute>(mnAttribute)) -
                   pCharacter->getMinAttribute(static_cast<eCharacterAttribute>(mnAttribute)));
        break;
    case ADD:
        add = mAdd;
        break;
    }


    //    pCharacter->modifyAttribute(mAttribute, add, 1) ;
}


HasGold::HasGold()
{
}

CL_DomElement  HasGold::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc, "hasGold");

    std::string oper;

    switch(meOperator)
    {
    case LT:
        oper = "lt";
        break;
    case GT:
        oper = "gt";
        break;
    case GTE:
        oper = "gte";
        break;
    case LTE:
        oper = "lte";
        break;
    case EQ:
        oper = "eq";
        break;
    }

    element.set_attribute("operator",oper);

    if(mbNot) element.set_attribute("not","true");

    CL_DomText text (doc, IntToString(mAmount ) );

    text.set_node_value( IntToString(mAmount ) );

    element.append_child( text );

    return element;
}

void HasGold::handleElement(eElement element, Element * pElement)
{

}

void HasGold::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    std::string op = getRequiredString("operator",pAttributes);   

    if(op == "lt") meOperator = LT;
    else if(op == "gt") meOperator = GT;
    else if(op == "gte") meOperator = GTE;
    else if(op == "lte") meOperator = LTE;
    else if(op == "eq") meOperator = EQ;
    else throw CL_Error("Bad operator type in hasGold");

    mbNot = getImpliedBool("not",pAttributes,false);

}

void HasGold::handleText(const std::string &text)
{
    mAmount = atoi(text.c_str());
}


HasGold::~HasGold()
{

}

bool HasGold::evaluate()
{
    IParty * party = IApplication::getInstance()->getParty();
    int gold = party->getGold();

    if(!mbNot)
    {
        switch ( meOperator )
        {
        case LT:
            return gold < mAmount;
        case GT:
            return gold > mAmount;
        case GTE:
            return gold >= mAmount;
        case LTE:
            return gold <= mAmount;
        case EQ:
            return gold == mAmount;
        }
    }
    else
    {
        // Opposites (Why not just specify these in the level xml? cause whoever made it felt like using a not)
        switch ( meOperator )
        {
        case LT:
            return gold >= mAmount;
        case GT:
            return gold <= mAmount;
        case GTE:
            return gold < mAmount;
        case LTE:
            return gold > mAmount;
        case EQ:
            return gold != mAmount;
        }
    }
}

CL_DomElement  HasItem::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"hasItem");

    if(mbNot) element.set_attribute("not","true");

    if(mCount != 1)
        element.set_attribute("count", IntToString(mCount));

    CL_DomElement e = mpItemRef->createDomElement(doc);
    element.append_child (e );


    return element;
}

void HasItem::handleElement(eElement element, Element *pElement)
{
    if(element == EITEMREF)
    {
        mpItemRef = dynamic_cast<ItemRef*>(pElement);
    }
    else throw CL_Error("Bad element found within hasItem");
}
 
void HasItem::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mbNot = getImpliedBool("not",pAttributes,false);

    mCount = getImpliedInt("count",pAttributes,1);
}

HasItem::HasItem( ):mpItemRef(NULL)
{
    
}

HasItem::~HasItem()
{
    delete mpItemRef;
}

bool HasItem::evaluate()
{
    if(mbNot) return ! (IApplication::getInstance()->getParty()->hasItem(mpItemRef, mCount )) ;
    else  return (IApplication::getInstance()->getParty()->hasItem(mpItemRef, mCount )) ;
}



DidEvent::DidEvent()
{
}

CL_DomElement  DidEvent::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"didEvent");

    if(mbNot) element.set_attribute("not","true");

    CL_DomText text ( doc, mEvent );
    text.set_node_value ( mEvent );

    element.append_child ( text);

    return element;
}

void DidEvent::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mbNot = getImpliedBool("not",pAttributes,false);
}

void DidEvent::handleText(const std::string &text)
{
    mEvent = text;
}

void DidEvent::handleElement(eElement element, Element *pElement)
{
}

DidEvent::~DidEvent()
{
}

bool DidEvent::evaluate()
{
    if(mbNot) return ! IApplication::getInstance()->getParty()->didEvent ( mEvent );
    else  return  IApplication::getInstance()->getParty()->didEvent ( mEvent );
}


And::And()
{
}

CL_DomElement  And::createDomElement(CL_DomDocument &doc) const
{

    CL_DomElement  element(doc,"and");

    for(std::list<Check*>::const_iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        CL_DomElement e = (*i)->createDomElement(doc);
        element.append_child ( e );

    }

    return element;
}

void And::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EOPERATOR:
    case EHASITEM:
    case EHASGOLD:
    case EDIDEVENT:
    
        mOperands.push_back(dynamic_cast<Check*>(pElement));
        break;
    }
}

And::~And()
{
    for(std::list<Check*>::iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        delete *i;
    }
}

bool And::evaluate()
{
    for(std::list<Check*>::const_iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        Check * check = *i;
        if(! check->evaluate() )
            return false;
    }

    return true;
}

ushort And::order()
{
    return 0;
}


Or::Or()
{
}

CL_DomElement  Or::createDomElement(CL_DomDocument &doc) const
{

    CL_DomElement  element(doc,"or");

    for(std::list<Check*>::const_iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        CL_DomElement e = (*i)->createDomElement(doc);
        element.append_child ( e );


    }

    return element;
}


void Or::handleElement(eElement element, Element *pElement)
{
    switch(element)
    {
    case EOPERATOR:
    case EHASITEM:
    case EHASGOLD:
    case EDIDEVENT:
        mOperands.push_back(dynamic_cast<Check*>(pElement));
        break;
    }
}
 
Or::~Or()
{
    for(std::list<Check*>::iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        delete *i;
    }
}

bool Or::evaluate()
{
    for(std::list<Check*>::const_iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        Check * check = *i;
        if( check->evaluate() )
            return true;
    }

    return false;
}

ushort Or::order()
{
    return 0;
}

Operator::Operator()
{
}

CL_DomElement  Operator::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"operator");

    for(std::list<Check*>::const_iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        element.append_child (  (*i)->createDomElement(doc) );
    }

    return element;    

}

void Operator::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EOR:
    case EAND:
        mOperands.push_back(dynamic_cast<Check*>(pElement));
        break;
    }   
}

Operator::~Operator()
{
    for(std::list<Check*>::iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        delete *i;
    }
    
}

bool Operator::evaluate()
{
    for(std::list<Check*>::const_iterator i = mOperands.begin();
        i != mOperands.end();
        i++)
    {
        Check * check = *i;
        if(! check->evaluate() )
            return false;
    }

    return true;
}

ushort Operator::order()
{
    return 0;
}
      

Condition::Condition()
{
}

CL_DomElement  Condition::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"condition");

    for(std::list<Check*>::const_iterator i = mChecks.begin();
        i != mChecks.end();
        i++)
    {
        CL_DomElement e= (*i)->createDomElement(doc);
        element.append_child ( e );

    }

    return element;
}


void Condition::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EOPERATOR:
    case EHASITEM:
    case EHASGOLD:
    case EDIDEVENT:
        mChecks.push_back(dynamic_cast<Check*>(pElement));
        break;
    }
}
 

Condition::~Condition()
{
    for(std::list<Check*>::iterator i = mChecks.begin();
        i != mChecks.end(); i++)
    {
        delete *i;
    }
}

bool Condition::evaluate() const
{
    for(std::list<Check*>::const_iterator i = mChecks.begin();
        i != mChecks.end(); i++)
    {
        Check * check = *i;
        // If anybody returns false, the whole condition is false
        if(! check->evaluate() ) return false;
    }

    return true;
}


Pop::Pop():mbAll(false)
{
}

Pop::~Pop()
{
}

void Pop::invoke()
{
	IApplication::getInstance()->pop(mbAll);
}

CL_DomElement Pop::createDomElement(CL_DomDocument &doc) const
{
	CL_DomElement element(doc,"pop");

	element.set_attribute("all", mbAll?"true":"false");

	return element;
}

void Pop::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
	mbAll = getImpliedBool("all",pAttributes,false);
}

Event::Event():mbRepeatable(true),mbRemember(false),mpCondition(NULL)
{
}

CL_DomElement  Event::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"event");

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

    if(mpCondition )
    {
        CL_DomElement e = mpCondition->createDomElement(doc);

        element.append_child ( e );

    }

    for(std::list<Action*>::const_iterator i = mActions.begin();
        i != mActions.end();
        i++)
    {
        CL_DomElement e = (*i)->createDomElement(doc);
    
        element.append_child ( e );

    }


    return element;

}

void Event::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mName = getRequiredString("name",pAttributes);

    std::string triggertype = getString("triggerType",pAttributes);

    if(triggertype == "step")
        meTriggerType = STEP;
    else if (triggertype == "talk")
        meTriggerType = TALK;
    else if (triggertype == "act")
        meTriggerType = ACT;
    else throw CL_Error(" Bad trigger type on event " + mName );

    mbRepeatable = getImpliedBool("repeatable",pAttributes,true);
    mbRemember = getImpliedBool("remember",pAttributes,false);
}

void Event::handleElement(eElement element, Element *pElement)
{
    if(isAction(element))
    {
        mActions.push_back( dynamic_cast<Action*>(pElement));
    }
    else if(element == ECONDITION)
    {
        mpCondition = dynamic_cast<Condition*>(pElement);
    }
}


Event::~Event()
{
    for(std::list<Action*>::iterator i = mActions.begin();
        i != mActions.end();
        i++)
    {
        delete *i;
    }
}

std::string Event::getName() const
{
    return mName;
}

Event::eTriggerType Event::getTriggerType()
{
    return meTriggerType;
}

bool Event::repeatable()
{
    return mbRepeatable;
}

bool Event::remember()
{
    return mbRemember;
}
 
bool Event::invoke()
{
    
    if(mpCondition && !mpCondition->evaluate() ) return false;

    IApplication::getInstance()->getParty()->doEvent ( mName, mbRemember );

    for(std::list<Action*>::iterator i = mActions.begin();
        i != mActions.end();
        i++)
    {
        Action *action = *i;

        action->invoke();
    }

    return true;
}
      

PlayAnimation::PlayAnimation()
{

}


CL_DomElement  PlayAnimation::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"playAnimation");

    CL_DomText text(doc,mAnimation);

    text.set_node_value( mAnimation );

    element.append_child ( text );

    return element;
}

void PlayAnimation::handleText(const std::string &text)
{
    mAnimation = text;
}


PlayAnimation::~PlayAnimation()
{
}

void PlayAnimation::invoke()
{
    IApplication::getInstance()->playAnimation ( mAnimation );
}
 

PlaySound::PlaySound()
{
}

CL_DomElement  PlaySound::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"playSound");

    CL_DomText text(doc, mSound );

    text.set_node_value ( mSound );

    element.append_child ( text );

    return element;
}


void PlaySound::handleText(const std::string &text)
{
    mSound = text;
}

PlaySound::~PlaySound()
{
}

void PlaySound::invoke()
{
    IApplication::getInstance()->playSound ( mSound );

}

LoadLevel::LoadLevel()
{
}


CL_DomElement  LoadLevel::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"loadLevel");

    element.set_attribute("startx", IntToString (mStartX ) ) ;
    element.set_attribute("starty", IntToString (mStartY ) );
    element.set_attribute("name", mName );
    
    return element;
}

void LoadLevel::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mStartX = getRequiredInt("startx",pAttributes);
    mStartY = getRequiredInt("starty",pAttributes);

    mName = getRequiredString("name",pAttributes);
}
 
LoadLevel::~LoadLevel()
{
}

CL_DomElement Movement::createDomElement(CL_DomDocument &doc) const
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

void Movement::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    if(hasAttr("speed",pAttributes))
    {
        std::string speed = getString("speed",pAttributes);

        if(speed == "medium")
        {
            meSpeed = MEDIUM;
        }
        else if(speed == "slow")
        {
            meSpeed = SLOW;
        }
        else if(speed == "fast")
        {
            meSpeed = FAST;
        }
        else throw CL_Error("Error, movement speed must be fast or slow.");
            
    }

    std::string type = getRequiredString("movementType",pAttributes);

    if(type == "wander")
    {   
        meType = MOVEMENT_WANDER;
    }
    else if(type == "paceNS")
    {
        meType = MOVEMENT_PACE_NS;
    }
    else if(type == "paceEW")
    {
        meType = MOVEMENT_PACE_EW;
    }
    else if(type == "none")
    {
        // Why would they ever....
        meType = MOVEMENT_NONE;
    }

}

Movement::Movement ( ):meType(MOVEMENT_NONE),meSpeed(SLOW)
{
   
}

Movement::~Movement()
{
}


Movement::eMovementType Movement::getMovementType() const
{
    return meType;
}

Movement::eMovementSpeed Movement::getMovementSpeed() const
{
    return meSpeed;
}


void LoadLevel::invoke()
{
    IApplication::getInstance()->loadLevel ( mName, mStartX, mStartY );
}


CL_DomElement  StartBattle::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"startBattle");

    element.set_attribute("isBoss", mbIsBoss? "true":"false");
    element.set_attribute("count", IntToString (mCount ) );
    element.set_attribute("monster", mMonster );

    return element;
}

void StartBattle::handleElement(eElement element, Element *pElement)
{

}

void StartBattle::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mbIsBoss = getImpliedBool("isBoss",pAttributes,false);

    mCount = getImpliedInt("count",pAttributes,1);

    mMonster = getRequiredString("monster",pAttributes);
}

StartBattle::StartBattle():mbIsBoss(false),mCount(1)
{

}
 
StartBattle::~StartBattle()
{
}

void StartBattle::invoke()
{
    IApplication::getInstance()->startBattle ( mMonster, mCount, mbIsBoss ) ;
}


InvokeShop::InvokeShop()
{
}


CL_DomElement  InvokeShop::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"invokeShop");

    element.set_attribute("shopType", mShopType );

    return element;
}

void InvokeShop::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    mShopType = getRequiredString("shopType",pAttributes);
}
 

InvokeShop::~InvokeShop()
{
}

void InvokeShop::invoke()
{
    IApplication::getInstance()->invokeShop ( mShopType );
}


Pause::Pause()
{
}

CL_DomElement  Pause::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement  element(doc,"pause");

    CL_DomText text(doc,IntToString(mMs));

    //    text.set_node_value( IntToString (mMs ) );

    element.append_child ( text );

    return element;
}
 

void Pause::handleText(const std::string &text)
{
    mMs = atoi ( text.c_str() );
}

Pause::~Pause()
{
}

void Pause::invoke()
{
    IApplication::getInstance()->pause ( mMs ) ;
}
 
Say::Say()
{
}

CL_DomElement  Say::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"say");

    element.set_attribute("speaker", mSpeaker );

    CL_DomText text(doc, mText );

    //    text.set_node_value( mText );

    element.append_child( text );

    return element;

}

void Say::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mSpeaker = getRequiredString("speaker",pAttributes);
}

void Say::handleText(const std::string &text)
{
    mText = text;
}


Say::~Say()
{
}

void Say::invoke()
{
    IApplication::getInstance()->say ( mSpeaker, mText );
}
 
CL_DomElement  Give::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"give");

    if(mCount != 1)
        element.set_attribute("count", IntToString ( mCount ) );

    CL_DomElement  itemRef = mpItemRef->createDomElement(doc);

    element.append_child(itemRef );

    return element;

}

void Give::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
        
    mCount = getImpliedInt("count",pAttributes,1);
}

void Give::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EITEMREF:
        mpItemRef = dynamic_cast<ItemRef*>(pElement);
        break;
    }
}

Give::Give( ):mpItemRef(NULL),mCount(1)
{

}
Give::~Give()
{
    delete mpItemRef;
}

void Give::invoke()
{
    IApplication::getInstance()->getParty()->giveItem ( mpItemRef, mCount );
}

CL_DomElement  Take::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"take");

    if(mCount != 1)
        element.set_attribute("count", IntToString ( mCount ) );

    CL_DomElement  itemRef = mpItemRef->createDomElement(doc);

    element.append_child(itemRef );


    return element;

}

void Take::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
        
    mCount = getImpliedInt("count",pAttributes,1);
}

void Take::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case EITEMREF:
        mpItemRef = dynamic_cast<ItemRef*>(pElement);
        break;
    }
}
 
Take::Take( ):mpItemRef(NULL)
{
   
}


Take::~Take()
{
    delete mpItemRef;
}

void Take::invoke()
{
    IApplication::getInstance()->getParty()->takeItem ( mpItemRef, mCount );
}


GiveGold::GiveGold()
{
}

CL_DomElement  GiveGold::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"giveGold");

    element.set_attribute("count", IntToString ( mCount ) );


    return element;
}
 

void GiveGold::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mCount = getImpliedInt("count",pAttributes,1);
}

 
GiveGold::~GiveGold()
{
}

void GiveGold::invoke()
{
    IApplication::getInstance()->getParty()->giveGold ( mCount );
}


Graphic::Graphic()
{
}

Graphic::~Graphic()
{
}


DirectionBlock::DirectionBlock():meDirectionBlock(0)
{
}

DirectionBlock::DirectionBlock(int i )
{
    meDirectionBlock = i;
}

CL_DomElement  DirectionBlock::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"directionBlock");

    element.set_attribute("north", BoolToString (meDirectionBlock & DIR_NORTH ));
    element.set_attribute("south", BoolToString (meDirectionBlock & DIR_SOUTH));
    element.set_attribute("east", BoolToString ( meDirectionBlock & DIR_EAST ) );
    element.set_attribute("west", BoolToString ( meDirectionBlock & DIR_WEST ) );

    return element;
}

void DirectionBlock::loadAttributes(CL_DomNamedNodeMap *pAttributes)
{
    bool north = getRequiredBool("north",pAttributes);
    bool south = getRequiredBool("south",pAttributes);
    bool east =  getRequiredBool("east",pAttributes);
    bool west =  getRequiredBool("west",pAttributes);

    if(north) meDirectionBlock |= DIR_NORTH;
    if(south) meDirectionBlock |= DIR_SOUTH;
    if(east) meDirectionBlock |= DIR_EAST;
    if(west) meDirectionBlock |= DIR_WEST;
}


DirectionBlock::~DirectionBlock()
{
}

int DirectionBlock::getDirectionBlock() const
{
    return meDirectionBlock;
}

bool Tile::isHot() const
{

    return cFlags & HOT;
}

bool Tile::pops() const
{
	return cFlags & POPS;
}

Tile::Tile():mpSprite(NULL),mpCondition(NULL),mpAM(NULL),mZOrder(0),cFlags(0)
{
}

CL_DomElement  Tile::createDomElement(CL_DomDocument &doc) const
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
        CL_DomElement  spriteEl = mGraphic.asSpriteRef->createDomElement(doc);

        element.append_child ( spriteEl );

    }
    else
    {
        CL_DomElement  tilemapEl = mGraphic.asTilemap->createDomElement(doc);

        element.append_child (  tilemapEl );

    }

    if(mpCondition)
    {
        CL_DomElement  condEl = mpCondition->createDomElement(doc);

        element.append_child ( condEl );

    }

    if(mpAM)
    {
        CL_DomElement  amEl = mpAM->createDomElement(doc);

        element.append_child ( amEl );

    }

    if( getDirectionBlock() > 0)
    {
        DirectionBlock block( getDirectionBlock() );

        CL_DomElement  dirEl = block.createDomElement(doc);

        element.append_child( dirEl );

    }
    

    return element;

    
}

void Tile::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mX = getRequiredInt("xpos",pAttributes);
    mY = getRequiredInt("ypos",pAttributes);

    mZOrder = getImpliedInt("zorder",pAttributes,0);

    bool floater = getImpliedBool("floater",pAttributes,false);
    bool hot = getImpliedBool("hot",pAttributes,false);
	bool pops = getImpliedBool("pops",pAttributes,false);

    if(floater) cFlags |= FLOATER;
    if(hot) cFlags |= HOT;
	if(pops) cFlags |= POPS;

}


void Tile::handleElement(eElement element, Element * pElement)
{
    switch(element)
    {
    case ETILEMAP:
        mGraphic.asTilemap = dynamic_cast<Tilemap*>(pElement);
        break;
    case ESPRITEREF:
    {
        GraphicsManager * GM = GraphicsManager::getInstance();
        mGraphic.asSpriteRef = dynamic_cast<SpriteRef*>(pElement);
        cFlags |= SPRITE;

        // Actually create the ref'd sprite here.
        // And assign to mpSprite
        mpSprite = GM->createSprite( mGraphic.asSpriteRef->getRef() );
        break;
    }//ESPRITEREF
    case ECONDITION:
        mpCondition = dynamic_cast<Condition*>(pElement);
        break;
    case EATTRIBUTEMODIFIER:
        mpAM = dynamic_cast<AttributeModifier*>(pElement);
        break;
    case EDIRECTIONBLOCK:
    {
        DirectionBlock *block = dynamic_cast<DirectionBlock*>(pElement);

        int db = block->getDirectionBlock();

        // This is all done to make tile's take up less space in memory
            
        if(db & DIR_NORTH)
            cFlags |= BLK_NORTH;
        if(db & DIR_SOUTH)
            cFlags |= BLK_SOUTH;
        if(db & DIR_EAST)
            cFlags |= BLK_EAST;
        if(db & DIR_WEST)
            cFlags |= BLK_WEST;

        delete block;
        
        break;
    }
    }
}

bool StoneRing::Tile::hasAM() const
{
    return mpAM != NULL;
}

void Tile::activate() // Call any attributemodifier
{
    mpAM->invoke();   
}

Tile::~Tile()
{
    delete mpAM;
    delete mpCondition;
    delete mpSprite;

    if( isSprite() )
        delete mGraphic.asSpriteRef;
    else delete mGraphic.asTilemap;
}

ushort Tile::getZOrder() const
{
    return mZOrder;
}

bool Tile::evaluateCondition() const
{
    if( mpCondition ) return mpCondition->evaluate();

    else return true;
}


uint Tile::getX() const
{
    return mX;
}

uint Tile::getY() const
{
    return mY;
}


CL_Rect Tile::getRect()
{
    return CL_Rect(mX , mY , mX + 1, mY +1);
}

bool Tile::isSprite() const
{
    return cFlags & SPRITE;
}

bool Tile::isFloater() const
{
    return cFlags & FLOATER;
}


void Tile::draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)
{
    // Get the graphic guy
    // Get our tilemap or sprite
    // Blit it

    static GraphicsManager * GM = GraphicsManager::getInstance();

    if( !isSprite() )
    {
        CL_Surface * tilemap = mGraphic.asTilemap->getTileMap();

        //        void draw(  const CL_Rect& src, const CL_Rect& dest, CL_GraphicContext* context = 0);
        int mapx, mapy;
        mapx = mGraphic.asTilemap->getMapX();
        mapy = mGraphic.asTilemap->getMapY();
        CL_Rect srcRect((mapx << 5) + src.left, (mapy << 5) + src.top,
                        (mapx << 5) + src.right, (mapy << 5) + src.bottom);

        
        tilemap->draw(srcRect, dst, pGC);
        
    }
    else
    {
        update();
        mpSprite->draw( dst, pGC );
    }

}

void Tile::update()
{
    if(isSprite()) mpSprite->update();
}

int Tile::getDirectionBlock() const
{

    int block = 0;

    if( cFlags & BLK_NORTH)
        block |= DIR_NORTH;
    if( cFlags & BLK_SOUTH)
        block |= DIR_SOUTH;
    if( cFlags & BLK_EAST)
        block |= DIR_EAST;
    if( cFlags & BLK_WEST)
        block |= DIR_WEST;

    return block;
}

bool Tile::isTile() const
{
    return true;
}

