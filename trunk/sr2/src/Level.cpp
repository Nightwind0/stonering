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


typedef unsigned int uint;


using namespace StoneRing;
using std::string;


#ifndef _MSC_VER
using std::max;
using std::min;
using std::abs;
#endif




std::string IntToString(const int &i)
{
    std::ostringstream os;

    os << i;


    return os.str();
    
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
    uint p1value = p1.y  *  IApplication::getInstance()->getScreenHeight() + p1.x;
    uint p2value = p2.y  * IApplication::getInstance()->getScreenHeight() + p2.x;
	
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

    element.set_attribute("attribute",mAttribute );

    if(mAdd)
	element.set_attribute("add", IntToString ( mAdd )  );

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
    }

    switch ( meChangeTo )
    {
    case ADD:
	element.set_attribute("changeTo","add");
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
    mAttribute = getRequiredString("attribute",pAttributes);
    mAdd = getRequiredInt("add",pAttributes);

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
	if( pCharacter->getAttribute(mAttribute) < pCharacter->getMaxAttribute(mAttribute))
	{
	    return true;
	}
	else return false;
    case TO_MIN:
	if( pCharacter->getAttribute(mAttribute) > pCharacter->getMinAttribute(mAttribute))
	{
	    return true;
	}
	else return false;
    case ADD:
	if(mAdd > 0 )
	{
	    if ( pCharacter->getAttribute(mAttribute) < pCharacter->getMaxAttribute(mAttribute))
	    {
		return true;
	    }
	    else return false;
	}
	else // counts 0, but... that does nothing anyway
	{
	    if ( pCharacter->getAttribute(mAttribute) > pCharacter->getMinAttribute(mAttribute))
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
	add = pCharacter->getMaxAttribute(mAttribute) - 
	    pCharacter->getAttribute(mAttribute);
	break;
    case TO_MIN:
	add = 0 - (pCharacter->getAttribute(mAttribute ) -
		   pCharacter->getMinAttribute(mAttribute ));
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
    else	return (IApplication::getInstance()->getParty()->hasItem(mpItemRef, mCount )) ;
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

    if(floater) cFlags |= FLOATER;
    if(hot) cFlags |= HOT;

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

bool Tile::hasAM() const
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

	//		void draw(	const CL_Rect& src, const CL_Rect& dest, CL_GraphicContext* context = 0);
	int mapx, mapy;
	mapx = mGraphic.asTilemap->getMapX();
	mapy = mGraphic.asTilemap->getMapY();
	CL_Rect srcRect(mapx * 32 + src.left, mapy * 32 + src.top,
			(mapx * 32) + src.right, (mapy * 32) + src.bottom);

		
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


CL_DomElement  MappableObject::createDomElement(CL_DomDocument &doc) const
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

bool MappableObject::evaluateCondition() const
{
    if(mpCondition)
    {
	return mpCondition->evaluate();
    }

    return true;
}

MappableObject::eSize MappableObject::getSize() const
{
    return meSize;
} 

void MappableObject::move()
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

void MappableObject::loadAttributes(CL_DomNamedNodeMap * pAttributes)
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

void MappableObject::handleElement(eElement element, Element * pElement)
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

void MappableObject::loadFinished()
{
    setFrameForDirection();
}
 
MappableObject::MappableObject():meDirection(NONE),mpSprite(NULL),mpMovement(0),
                                 mpCondition(0),cFlags(0),mnCellsMoved(0),mnMovesSkipped(0), mnFrameMarks(0)
{
   
}


uint MappableObject::getCellHeight() const
{
    return calcCellDimensions(meSize).x;
}

uint MappableObject::getCellWidth() const
{
    return calcCellDimensions(meSize).y;
}
MappableObject::~MappableObject()
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




Movement* MappableObject::getMovement() const
{
    return mpMovement;
}




std::string MappableObject::getName() const
{
    return mName;
}


CL_Point MappableObject::calcCellDimensions(eSize size)
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

CL_Rect MappableObject::getPixelRect() const
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

bool MappableObject::isSprite() const
{
    return cFlags & SPRITE;
}

void MappableObject::draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)
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

void MappableObject::pickOppositeDirection()
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


void MappableObject::randomNewDirection()
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
    setFrameForDirection();
    	
}

void MappableObject::setFrameForDirection()
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


void MappableObject::update()
{

    // Don't bother with disabled MOs
    if(!evaluateCondition()) return;

    if(isSprite())
    {
	mbStep = mbStep? false:true;
      
    }
}





bool MappableObject::isSolid() const
{
    return cFlags & MappableObject::SOLID;
}

int MappableObject::getDirectionBlock() const
{
    if (isSolid()) return  (DIR_NORTH | DIR_SOUTH | DIR_EAST | DIR_WEST);
    else return 0;
}

bool MappableObject::isTile() const
{
    return false;
}

void MappableObject::prod()
{
    randomNewDirection();
}

bool MappableObject::onScreen()
{
    CL_Rect screen = IApplication::getInstance()->getLevelRect();
  
    return screen.is_overlapped(getPixelRect());
}


void MappableObject::CalculateEdgePoints(const CL_Point &topleft, eDirection dir, eSize size, std::list<CL_Point> *pList)
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

void MappableObject::provokeEvents ( Event::eTriggerType trigger )
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

int MappableObject::ConvertDirectionToDirectionBlock(eDirection dir) 
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

void MappableObject::movedOneCell()
{
    if(++mnCellsMoved) 
    {
	if(mnCellsMoved == 10) ///@todo: get from somewhere
	{
	    randomNewDirection();
	}
    }

}
bool MappableObject::isAligned() const
{
    return (mX /32 == 0 )&& (mY /32 == 0);
}

bool MappableObject::chanceToMove() 
{
    if(mpMovement)
    {
	switch( mpMovement->getMovementSpeed() )
	{
	case Movement::SLOW:
	    if(mnMovesSkipped > 4)
	    {
		mnMovesSkipped = 0;
		return true;
	    }
	    else
	    {
		++mnMovesSkipped ;
		return false;
	    }
	    break;
	case Movement::MEDIUM:
	    if(mnMovesSkipped)
	    {
		mnMovesSkipped = 0;
		return true;
	    }
	    else
	    {
		mnMovesSkipped = 1;
		return false;
	    }
	    break;
	case Movement::FAST:
	    return true;
	    break;
	}
    }
    else return false;
}

CL_Point MappableObject::getPositionAfterMove() const
{
    CL_Point point = getPosition();

    switch(meDirection)
    {
    case SOUTH:
	point.y += 1;
	break;
    case NORTH:
	point.y += 1;
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

Level::Level()
{
}

bool Level::containsMappableObjects(const CL_Point &point) const
{
    if(mMOMap.find(point) != mMOMap.end())
    {
	return true;
    }
    else return false;
}

bool activateSolidMappableObject(const MappableObject *pMO)
{
    return pMO->isSolid() && pMO->evaluateCondition();
}

bool Level::containsSolidMappableObject(const CL_Point &point) const
{

    MappableObjectMap::const_iterator  iList = mMOMap.find(point);

    if(iList == mMOMap.end()) return false;

    std::list<MappableObject*>::const_iterator iMO = 
	std::find_if((*iList).second.begin(),
		     (*iList).second.end(),&activateSolidMappableObject);

    if(iMO != (*iList).second.end()) return true;
    else return false;
}

void Level::setMappableObjectAt(const CL_Point &point, MappableObject*  pMO)
{
    // If the list doesn't exist, it will get created
    mMOMap[point].push_back(pMO);
}

void Level::removeMappableObjectFrom(const CL_Point &point, MappableObject *pMO)
{
    MappableObjectMap::iterator iList = mMOMap.find(point);

    if(iList != mMOMap.end())
    {
	iList->second.remove( pMO );

	// We emptied, so why even keep this list around?
	if(iList->second.size() == 0)
	{
	    mMOMap.erase(iList);
	}
    }
    else
    {
	cl_assert("Umm... I expected to find a list at a certain point, but there wasn't one.");
    }
}


CL_DomElement  Level::createDomElement(CL_DomDocument &doc) const
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
	    for( std::list<Tile*>::const_iterator i = mTileMap[x][y].begin();
		 i != mTileMap[x][y].end();
		 i++)
	    {
		CL_DomElement  tileEl = (*i)->createDomElement(doc);

		tiles.append_child ( tileEl );

	    }
	}
    }

    for(std::map<CL_Point,std::list<Tile*> >::const_iterator j = mFloaterMap.begin();
	j != mFloaterMap.end();
	j++)
    {
	for(std::list<Tile*>::const_iterator jj = j->second.begin();
	    jj != j->second.end();
	    jj++)
	{
	    CL_DomElement floaterEl = (*jj)->createDomElement(doc);

	    tiles.append_child ( floaterEl );

	}
	    
    }

    element.append_child(tiles);

    ++mnFrameCount; // Use to make sure we don't save each MO more than once. Wee.
    
    for(int x= 0; x < mLevelWidth; x++)
    {
	for(int y=0;y<mLevelHeight;y++)
	{
	    CL_Point point(x,y);
	    
	    MappableObjectMap::const_iterator iList = mMOMap.find(point);

	    if(iList != mMOMap.end())
	    {
		for(std::list<MappableObject*>::const_iterator iMo = (*iList).second.begin();
		    iMo != (*iList).second.end();iMo++)
		{
		    if(mnFrameCount > (*iMo)->getFrameMarks())
		    {
			(*iMo)->markFrame();
			CL_DomElement  moEl = (*iMo)->createDomElement ( doc );
			mappableObjects.append_child( moEl );
		    }
		}
	    }
	}
    }

    element.append_child(mappableObjects);
    

    return element;

}


Level::Level(const std::string &name,CL_ResourceManager * pResources): mpDocument(NULL)
{
    srand(time(0));
    // Get the level file name from resources

    std::string path = CL_String::load("Game/LevelPath", pResources);
    std::string filename = CL_String::load("Levels/" + name, pResources);

    // Load the level
    LoadLevel ( path + filename );
}

Level::Level(CL_DomDocument &document):mbAllowsRunning(false)
{
    LoadLevel ( document );
}

Level::~Level()
{
    for(int x=0;x<mLevelWidth;x++)
    {
	for(int y=0;y<mLevelHeight;y++)
	{
	    for(std::list<Tile*>::iterator i = mTileMap[x][y].begin();
		i != mTileMap[x][y].end();
		i++)
	    {
		delete *i;
	    }

	    
	}


    
    }
}

int Level::getCumulativeDirectionBlockAtPoint(const CL_Point &point) const
{
    int directionBlock =0;

    std::list<Tile*> tileList = mTileMap[point.x][point.y];

    for(std::list<Tile*>::iterator iter = tileList.begin();
	iter != tileList.end();
	iter++)
    {
	if((*iter)->evaluateCondition())
	    directionBlock |= (*iter)->getDirectionBlock();
    }

    return directionBlock;
}

bool Level::getCumulativeHotnessAtPoint(const CL_Point &point) const
{

    std::list<Tile*> tileList = mTileMap[point.x][point.y];

    for(std::list<Tile*>::iterator iter = tileList.begin();
	iter != tileList.end();
	iter++)
    {
	if((*iter)->evaluateCondition())
	    if((*iter)->isHot()) return true;
    }
        
    return false;

}
    

void Level::draw(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC, bool floaters,
                 bool highlightHot, bool indicateBlocks)
{
//	int maxSrcX = max( ceil(dst.get_width() / 32.0), mLevelWidth );
//	int maxSrcY = max( ceil(dst.get_height() / 32.0), mLevelHeight);
	
    int widthInTiles = (int)ceil((float)src.get_width() / 32.0 );
    int heightInTiles = (int)ceil((float)src.get_height() / 32.0 );

    int widthInPx = widthInTiles * 32;
    int heightInPx = heightInTiles * 32;

    if(src.left % 32)
	widthInTiles = max(widthInTiles, (src.get_width() / 32 + 1));
    if(src.top % 32)
	heightInTiles = max(heightInTiles, (src.get_height() /32 + 1));


    CL_Rect exDst = dst; // expanded Dest
	 
    // Make it as big as the full source tiles
    // (It maintains the top/left position when you do this)
    exDst.set_size(CL_Size( widthInPx, heightInPx ));
	
    // Move the top and left position out

    int newleftDelta = src.left - ((src.left / 32) * 32);
    int newtopDelta = src.top - ((src.top / 32) * 32);

    exDst.left -= newleftDelta;
    exDst.right -= newleftDelta;
    exDst.top -= newtopDelta;
    exDst.bottom -= newtopDelta;
	


    if(floaters)
    {
	
	for(std::map<CL_Point,std::list<Tile*> >::iterator f = mFloaterMap.begin();
	    f != mFloaterMap.end();
	    f++)
	{
	
	    // Possible optimization... instead of using is_overlapped, do a couple quick comparisons
	    CL_Rect floaterRect(f->first.x,f->first.y,f->first.x + 32, f->first.y + 32);
	    

	    // Is this floater even on screen
	    if(src.is_overlapped( floaterRect))
	    
		for(std::list<Tile*>::iterator i = f->second.begin();
		    i != f->second.end();
		    i++)
		{
		    CL_Rect tileSrc(0,0,32,32);
		    CL_Rect tileDst ( exDst.left  + f->first.x * 32,
				      exDst.top + f->first.y * 32,
				      exDst.left + f->first.x * 32 + 32,
				      exDst.top + f->first.y * 32 + 32);
					
		    Tile * pTile = *i;
		    if(pTile->evaluateCondition())
		    {
			pTile->draw(tileSrc, tileDst , pGC );

		    }
		    
		    
		}
	}
	
    }		
    else
    {
	// Regular tiles, not floaters
	
	for(int tileX = 0; tileX < widthInTiles; tileX++)
	{
	    for(int tileY =0; tileY < heightInTiles; tileY++)
	    {
		
		CL_Point p( src.left / 32 + tileX, src.top /32 + tileY);
		
		
		if(p.x >=0 && p.y >=0 && p.x < mLevelWidth && p.y < mLevelHeight)
		{
		    
		    
		    for(std::list<Tile*>::iterator i = mTileMap[p.x][p.y].begin();
			i != mTileMap[p.x][p.y].end();
			i++)
		    {
			CL_Rect tileSrc(0,0,32,32);
			CL_Rect tileDst ( exDst.left  + tileX * 32,
					  exDst.top + tileY * 32,
					  exDst.left + tileX * 32 + 32,
					  exDst.top + tileY * 32 + 32);
			
			Tile * pTile = *i;
			if(pTile->evaluateCondition())
			{
			    pTile->draw(tileSrc, tileDst , pGC );
			    
			    

			    // Extra code for level editing
			    if(highlightHot && pTile->isHot())
			    {
				pGC->fill_rect(tileDst, CL_Color(255,0,0,160));
			    }
			    if(indicateBlocks)
			    {
				int block = pTile->getDirectionBlock();
						    
				if(block & DIR_WEST)
				{
				    pGC->fill_rect(CL_Rect(tileDst.left,tileDst.top,tileDst.left + 8, tileDst.bottom), CL_Color(0,255,255,120));
				}
				if(block & DIR_EAST)
				{
				    pGC->fill_rect(CL_Rect(tileDst.right - 8, tileDst.top, tileDst.right,tileDst.bottom),CL_Color(0,255,255,120));
				}
				if(block & DIR_NORTH)
				{
				    pGC->fill_rect(CL_Rect(tileDst.left, tileDst.top, tileDst.right, tileDst.top +8), CL_Color(0,255,255,120));
				}
				if(block & DIR_SOUTH)
				{
				    pGC->fill_rect(CL_Rect(tileDst.left,tileDst.bottom -8, tileDst.right, tileDst.bottom), CL_Color(0,255,255,120));
				}
			    }
			}
					

  					

 					
 					
		    }
		}
			
 			
 			
 			
 			
	    }
	}
	
    }
    
    


	

}	    


void Level::drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC)
{
    int cornerx = static_cast<int>(src.left / 32);
    int cornery = static_cast<int>(src.top / 32);

    int width = static_cast<int>(ceil(src.right/32)) - cornerx;
    int height = static_cast<int>(ceil(src.bottom/32)) - cornery;
        
    ++mnFrameCount;

    for(int x = 0;x<width;x++)
    {
	for(int y=0;y<height;y++)
	{
	    CL_Point point(cornerx + x, cornery+ y);

	    MappableObjectMap::const_iterator iList = mMOMap.find(point);
	    if(iList != mMOMap.end())
	    {
		for(std::list<MappableObject*>::const_iterator iter = (*iList).second.begin();
		    iter != (*iList).second.end(); iter++)
		{
		    MappableObject * pMO = *iter;
		    cl_assert ( pMO != 0 );

		    if(mnFrameCount > pMO->getFrameMarks()
		       && pMO->evaluateCondition())
		    {
			(*iter)->markFrame();
			CL_Rect moRect = pMO->getPixelRect();
			CL_Rect dstRect( moRect.left - src.left + dst.left, moRect.top + dst.top - src.top,
					 moRect.left - src.left + dst.left +moRect.get_width(), moRect.top - src.top + dst.top + moRect.get_height());
			(*iter)->update();
			(*iter)->draw( moRect, dstRect, pGC );		    
		    }
		}
	    }
	}
    }

 	
}

void Level::putMappableObjectAtCurrentPosition(MappableObject *pMO)
{
    CL_Point cur_pos = pMO->getPosition();
    uint width = pMO->getCellWidth();
    uint height = pMO->getCellHeight();

    for(int x = cur_pos.x; x< width;x++)
	for(int y=cur_pos.y;y<height;y++)
	{
	    setMappableObjectAt(CL_Point(x,y),pMO);
	}
}


void Level::removeMappableObjectFromCurrentPosition(MappableObject *pMO)
{
    CL_Point cur_pos = pMO->getPosition();
    uint width = pMO->getCellWidth();
    uint height = pMO->getCellHeight();

    for(int x = cur_pos.x; x< width;x++)
	for(int y=cur_pos.y;y<height;y++)
	{
	    removeMappableObjectFrom(CL_Point(x,y),pMO);
	}
}



void Level::moveMappableObjects(const CL_Rect &src)
{

// They can't change direction unless they are aligned.
// They can't stop unless they are aligned. 
// Otherwise they can move as much as they want.
// The timer should go off at the lowest time (the fastest mover)
// And slower characters should activate say every other or every third time.
// So this should be called periodically, but not every MO will move when it's called.
        
    // Step one, check direction of mappable object.
    // Step two, find the set of points that the object is moving into
    // And, if the object is aligned (meaning that is about to be unaligned)
    // If the object is solid, then mark those points as occupied.
    // Then, tell the object to move.
    // If the object then becomes aligned, find the points that it previously occupied
    // (in other words, the points to the east if it's moving west)
    // And, if the object is solid,  mark them as unoccupied

    int cornerx = static_cast<int>(src.left / 32);
    int cornery = static_cast<int>(src.top / 32);

    int width = static_cast<int>(ceil(src.right/32)) - cornerx;
    int height = static_cast<int>(ceil(src.bottom/32)) - cornery;
        

    for(int x = 0;x<width;x++)
    {
	for(int y=0;y<height;y++)
	{
	    CL_Point point(cornerx + x, cornery+ y);

	    MappableObjectMap::iterator iList = mMOMap.find(point);

	    if(iList != mMOMap.end())
	    {
		for(std::list<MappableObject*>::iterator iMo = (*iList).second.begin();
		    iMo != (*iList).second.end(); iMo++)
		{

		    if(! (*iMo)->evaluateCondition()) continue; // Skip 'em

		    if((*iMo)->chanceToMove())
		    {

			if((*iMo)->getDirection() != MappableObject::NONE)
			{
			    // Aligned means we're about to become unaligned by moving
			    // So we get a list of points at the location we are moving into,
			    // And we mark those points as occupado.

			    bool bWasAligned = (*iMo)->isAligned();
			
			    if(bWasAligned)
			    {
				std::list<CL_Point> intoPoints ;
				CL_Point topleft = (*iMo)->getPositionAfterMove();
                                        
				MappableObject::CalculateEdgePoints(topleft, (*iMo)->getDirection(),
								    (*iMo)->getSize(), &intoPoints);
                                        
				// Make sure none of these points is occupied.
				// Break this into a method so I can if() off it
				for(std::list<CL_Point>::iterator iter = intoPoints.begin();
				    iter != intoPoints.end();
				    iter++)
				{
				    if(containsSolidMappableObject(*iter)
				       ||
				       (getCumulativeDirectionBlockAtPoint(*iter) & MappableObject::ConvertDirectionToDirectionBlock((*iMo)->getDirection()))
				       || getCumulativeHotnessAtPoint(*iter)
				       || (*iter).x <0
				       || (*iter).y <0
				       || (*iter).x > mLevelWidth
				       || (*iter).y > mLevelHeight
					)
				    {
					// No go. Change direction, so we can try again.
					(*iMo)->randomNewDirection();
					return;
				    }
				}                                
			    
				// Mark current points as occcupied by you

				for(std::list<CL_Point>::iterator i = intoPoints.begin();
				    i != intoPoints.end();
				    i++)
				{
				    setMappableObjectAt(*i, *iMo);
				}
			    }
                                
			    // Okay. We've made sure that we can move. 
			    (*iMo)->move();

			    // Theres a possability that we've become aligned after moving. In which case, we have to 
			    // mark where we've come from as unoccupied.
                                
			    if((*iMo)->isAligned())
			    {
				std::list<CL_Point> fromPoints ;
				CL_Point topleft = (*iMo)->getPosition();
                                    
				MappableObject::CalculateEdgePoints(topleft, MappableObject::OppositeDirection((*iMo)->getDirection()),
								    (*iMo)->getSize(), &fromPoints);
				    
				// Mark these points as unoccupied
				for(std::list<CL_Point>::iterator iter = fromPoints.begin();
				    iter != fromPoints.end();
				    iter++)
				{
				    removeMappableObjectFrom(*iter, *iMo);
				}
                                        
				(*iMo)->movedOneCell();
                                        
			    }
                                
			}
			else
			{
			    // Unaligned. Therefore, need to move, in order to become aligned
			    (*iMo)->move();
			}
		    }
		    else
		    {
			// Direction == NONE
			// If direction is none, then this method is called less frequently, but occasionally it is called
			// And to give the object a chance to change direction from NONE, we tell it that it moved
			// One cell. 
			(*iMo)->movedOneCell();
		    }
		}
	    }
	}
    }

        
}
 

void Level::drawFloaters(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext * pGC)
{
    draw(src,dst,pGC, true);
}


      
// Checks relevant tile and MO direction block information
// Rects are in cells
bool Level::tryMove(const CL_Point &currently, const CL_Point & destination)
{
    //deprecated very soon...
}

// All AM's from tiles fire, as do any step events
void Level::step(const CL_Point &target)
{

    // First, process any MO step events you may have triggered

    MappableObjectMap::const_iterator iList = mMOMap.find(target);
    
    if(iList != mMOMap.end())
    {
    
	for(std::list<MappableObject*>::const_iterator iMo = (*iList).second.begin();
	    iMo != (*iList).second.end();
	    iMo++)
	{
	    if((*iMo)->evaluateCondition())
	    {
		// This MO needs to be stepped on
		//(*i)->provokeEvents ( Event::STEP );
		(*iMo)->provokeEvents(Event::STEP);
	    }
	}
    }


    
    activateTilesAt(target.x,target.y);

}


void Level::activateTilesAt ( uint x, uint y )
{
    std::list<Tile*> tileList = mTileMap[x][y];

    for(std::list<Tile*>::const_iterator iter = tileList.begin();
	iter != tileList.end();
	iter++)
    {
	if ( (*iter)->hasAM() && (*iter)->evaluateCondition() )
	{
	    (*iter)->activate();
	}
    }
    
}
      
// Any talk events fire (assuming they meet conditions)
void Level::talk(const CL_Point &target, bool prod)
{


    MappableObjectMap::const_iterator iList = mMOMap.find(target);
    
    if(iList != mMOMap.end())
    {
    
	for(std::list<MappableObject*>::const_iterator iMo = (*iList).second.begin();
	    iMo != (*iList).second.end();
	    iMo++)
	{

	    if((*iMo)->evaluateCondition())
	    {
		if(!prod)
		{
		    // This MO needs to be talked to
		    (*iMo)->provokeEvents ( Event::TALK );
		}
		else
		{
		    // Prod!
		    // (Can't prod things that aren't solid. They aren't in your way anyways)
		    // And if it has no movement, prodding it does nothing.
		    if((*iMo)->isSolid() && (*iMo)->getMovement() != NULL)
		    {
			(*iMo)->prod();
		    }

		
		}
		
		return; // We only do the first one.
		
	    }
	}
	
    }
}

// Propagate updates to any MO's in view. Provides as a level coordinate based rectangle
void Level::update(const CL_Rect & updateRect)
{
}
      
 
       
// Sort MO's in order to bring close ones to the top
bool Level::moSortCriterion( const MappableObject *p1, const MappableObject * p2)
{
//	Party * pParty = Party::getInstance();
//	uint pX = pParty->getLevelX();
//	uint pY = pParty->getLevelY();
// Get the center point of the screen instead of the party.

    IApplication * pApp = IApplication::getInstance();

    int pX = pApp->getLevelRect().left + pApp->getLevelRect().get_width() / 2;
    int pY = pApp->getLevelRect().top + pApp->getLevelRect().get_height() / 2;

    uint p1Distance, p2Distance;

	
/*    p1Distance = max(abs( (long)pX - p1->getX()) , abs((long)pY - p1->getY()));
      p2Distance = max(abs( (long)pX - p2->getX()) , abs((long)pY - p2->getY()));
*/
    int dx1 = abs((long)pX - p1->getX());
    int dy1 = abs((long)pY - p1->getY());
    int dx2 = abs((long)pX - p2->getX());
    int dy2 = abs((long)pY - p2->getY());

    p1Distance = (dx1 * dx1) + (dy1 * dy1);
    p2Distance = (dx2 * dx2) + (dy2 * dy2);

    return p1Distance < p2Distance;


}

#if 0
void Level::dumpMappableObjects() const
{

    IApplication * pApp = IApplication::getInstance();

    CL_Rect levelRect = pApp->getLevelRect();
    int pX = pApp->getLevelRect().get_width() / 2;
    int pY = pApp->getLevelRect().get_height() / 2;


    std::cout << "=== Mappable Objects ===" << std::endl;

    for(std::list<MappableObject *>::const_iterator i = mMappableObjects.begin();
	i != mMappableObjects.end();
	i++)
    {
	MappableObject * pMO = *i;


	std::cout << pMO->getName();

	int dx1 = pX - pMO->getX();
	int dy1 = pY - pMO->getY();

	int distance = (int)sqrt((float)( dx1 * dx1 + dy1 * dy1 ));

	std::cout << " Dst: " << distance;

	if( levelRect.is_overlapped(pMO->getRect()) )
	{
	    std::cout << " is Onscreen ";
	}

	std::cout << pMO->getX() << ',' << pMO->getY();
	std::cout << std::endl;
			
    }


}
#endif

// Sort tiles on zOrder
bool Level::tileSortCriterion ( const Tile * p1, const Tile * p2)
{
    return p1->getZOrder() < p2->getZOrder();
}

void Level::load ( CL_DomDocument &document)
{
    LoadLevel ( document );
}


void Level::LoadLevel (CL_DomDocument &document )
{

    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();

    CL_DomElement levelNode = document.named_item("level").to_element(); 


    CL_DomNamedNodeMap levelAttributes = levelNode.get_attributes();

    mName = levelAttributes.get_named_item("name").get_node_value();
    std::cout << "LEVEL NAME = " << levelAttributes.get_named_item("name").get_node_value() << std::endl;

    CL_DomElement headerNode = levelNode.named_item("levelHeader").to_element();

    CL_DomElement tilesNode = levelNode.named_item("tiles").to_element();
	
    std::cout << "FOUND TILES TAG. " << tilesNode.get_tag_name() << std::endl;

	
    // Process Header

    CL_DomNamedNodeMap headerAttributes = headerNode.get_attributes();
	
    mMusic = headerAttributes.get_named_item("music").get_node_value();

    std::cout << "MUSIC TYPE = " << mMusic << std::endl;

    mLevelWidth = atoi(headerAttributes.get_named_item("width").get_node_value().c_str());
    mLevelHeight = atoi(headerAttributes.get_named_item("height").get_node_value().c_str());

    std::cout << "DIMENSIONS: " << mLevelWidth << " by " << mLevelHeight << std::endl;


    if(headerAttributes.get_named_item("allowsRunning").get_node_value() == "true")
    {
	mbAllowsRunning = true;
    }
    

    // Create tilemap

    mTileMap.resize( mLevelWidth );

    for(int x=0;x< mLevelWidth; x++)
    {
	mTileMap[x].resize ( mLevelHeight );
    }


    // Process tiles
	
    CL_DomElement moNode = tilesNode.get_next_sibling().to_element();

    std::cout << "Tiles sibling is: " << moNode.get_node_name() << std::endl;

    CL_DomElement currentTile = tilesNode.get_first_child().to_element();
    CL_DomElement *pTile = &currentTile;

    int tilecount = 0;

    while(!pTile->is_null())
    {
	tilecount++;
	loadTile( pTile );

	currentTile = currentTile.get_next_sibling().to_element();
	pTile = &currentTile;
    }




    std::cout << "FOUND " << tilecount << " TILES" << std::endl;

    CL_DomElement currentMo = moNode.get_first_child().to_element();

    int mocount = 0;

    while(!currentMo.is_null())
    {
	mocount++;

	loadMo ( &currentMo );

	currentMo = currentMo.get_next_sibling().to_element();

		
    }


    std:: cout << "FOUND " << mocount << " MAPPABLE OBJECTS" << std::endl;

    mnFrameCount = 0 ;
}

void Level::LoadLevel( const std::string & filename  )
{
    CL_InputSource_File file(filename);

    std::cout << "LOADING LEVEL: " << filename << std::endl;

    CL_DomDocument document;
    CL_DomDocument otherdoc;



	
    document.load(&file);


 
    LoadLevel ( document );


}

void Level::loadMo ( CL_DomElement * moElement )
{
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();
    MappableObject * mo = dynamic_cast<MappableObject*>(factory->createElement(Element::EMAPPABLEOBJECT));

    mo->load(moElement);

    putMappableObjectAtCurrentPosition(mo);

}


void Level::loadTile ( CL_DomElement * tileElement)
{
    LevelFactory * factory = IApplication::getInstance()->getLevelFactory();

    if(factory == NULL) throw CL_Error("Factory was null. My life is a lie!");
    CL_Point point;

    Tile * tile = dynamic_cast<Tile*>(factory->createElement ( Element::ETILE ));
    tile->load(tileElement);

    if(tile == NULL) throw CL_Error("Tile was null, it was all for naught!");

    if(tile->getX() >= mLevelWidth)
    {
	delete tile;
	throw CL_Error("Tile found beyond range of indicated level width.");
    }
    else if (tile->getY() >= mLevelHeight)
    {
	delete tile;
	throw CL_Error("Tile found beyond range of indicated level height.");
    }

    point.x = tile->getX();
    point.y = tile->getY();

   

	
    if( tile->isFloater() )
    {
	std::cout << "Placing floater at: " << point.x << ',' << point.y << std::endl;

	mFloaterMap[ point ].push_back ( tile );

#ifndef _MSC_VER
	mFloaterMap[ point ].sort( &tileSortCriterion );
#else
	mFloaterMap[ point ].sort( std::greater<Tile*>() );
#endif
    }
    else
    {


		
	mTileMap[ point.x ][point.y].push_back ( tile );

	// Sort by ZOrder, so that they display correctly
#ifndef _MSC_VER
	mTileMap[ point.x ][point.y].sort( &tileSortCriterion );
#else
	mTileMap[ point.x ][point.y].sort(std::greater<Tile*>() );
#endif
    }
}
