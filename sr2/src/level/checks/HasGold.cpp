#include "HasGold.h"
#include "IParty.h"
#include "IApplication.h"

using namespace StoneRing;

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

bool HasGold::handleElement(eElement element, Element * pElement)
{
    return false;
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


