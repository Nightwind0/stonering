
#include "AttributeModifier.h"
#include "CharacterDefinition.h" // for CA stuff
#include "Condition.h"
#include "IApplication.h"

using namespace StoneRing;

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

bool AttributeModifier::handleElement(eElement element, Element *pElement)
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
        return false;
    }
    return true;
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


