#include "RegularItem.h"

using namespace StoneRing;


RegularItem::RegularItem()
:m_pScript(NULL)
{
}

RegularItem::~RegularItem()
{
    delete m_pScript;
}

// Execute all actions.
void RegularItem::Invoke()
{
    if(m_pScript)
        m_pScript->ExecuteScript();
}

RegularItem::eUseType 
RegularItem::GetUseType() const
{
    return m_eUseType;
}

RegularItem::eTargetable
RegularItem::GetTargetable() const
{
    return m_eTargetable;
}

RegularItem::eDefaultTarget 
RegularItem::GetDefaultTarget() const
{
    return m_eDefaultTarget;
}

bool RegularItem::IsReusable() const
{
    return m_bReusable;
}


uint RegularItem::GetValue() const
{
    return m_nValue;
}

uint RegularItem::GetSellValue() const
{
    return m_nSellValue;
}

RegularItem::eUseType 
RegularItem::UseTypeFromString ( const std::string &str )
{

    eUseType type = WORLD;

    if(str == "battle") type = BATTLE;
    else if (str == "world") type = WORLD;
    else if (str == "both") type = BOTH;
    else throw CL_Error("Bad targetable on regular item. " + str);

    return type;

}



RegularItem::eTargetable 
RegularItem::TargetableFromString ( const std::string &str )
{

    eTargetable targetable = SINGLE;

    if(str == "all") targetable = ALL;
    else if (str == "single") targetable = SINGLE;
    else if (str == "either") targetable = EITHER;
    else if (str == "self_only") targetable = SELF_ONLY;
    else throw CL_Error("Bad targetable on regular item. " + str);


    return targetable;
}

void RegularItem::load_attributes(CL_DomNamedNodeMap *pAttributes)
{
    m_nValue = get_required_int("value",pAttributes);

    m_nSellValue = m_nValue / 2;

    std::string useType = get_required_string("use",pAttributes);
    m_eUseType = UseTypeFromString ( useType );    

    std::string targetable = get_required_string("targetable",pAttributes); 
    m_eTargetable = TargetableFromString ( targetable );    

    if(has_attribute("sellValueMultiplier", pAttributes))
    {
        float multiplier = get_float("sellValueMultiplier",pAttributes);
        m_nSellValue = (int)(m_nValue * multiplier);
    }

    m_bReusable = get_required_bool("reusable",pAttributes);

    if(has_attribute("defaultTarget",pAttributes))
    {
        std::string str = get_string("defaultTarget",pAttributes);

        if( str == "party" )
            m_eDefaultTarget = PARTY;
        else if (str == "monsters")
            m_eDefaultTarget = MONSTERS;
        else throw CL_Error("Bogus default target on regular item.");

    }
    else
    {
        m_eDefaultTarget = PARTY;
    }

}

bool RegularItem::handle_element(eElement element, Element * pElement)
{
    if(element == Element::ESCRIPT)
    {
        m_pScript = dynamic_cast<ScriptElement*>(pElement);
        return true;
    }
    else return false;
}

void RegularItem::LoadItem ( CL_DomElement * pElement )
{

}





