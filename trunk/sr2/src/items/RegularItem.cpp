#include "RegularItem.h"
#include "IconRef.h"

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
void RegularItem::Invoke(const SteelType& array)
{
    ParameterList params;
    params.push_back ( ParameterListItem("@_Targets",array) );
    if(m_pScript)
        m_pScript->ExecuteScript(params);
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
    else throw CL_Exception("Bad targetable on regular item. " + str);

    return type;

}



RegularItem::eTargetable
RegularItem::TargetableFromString ( const std::string &str )
{

    eTargetable targetable = SINGLE;

    if(str == "all") targetable = ALL;
    else if (str == "single") targetable = SINGLE;
    else if (str == "group") targetable = GROUP;
    else if (str == "self_only") targetable = SELF_ONLY;
    else if (str == "no_target") targetable = NO_TARGET;
    else throw CL_Exception("Bad targetable on regular item. " + str);


    return targetable;
}

void RegularItem::load_attributes(CL_DomNamedNodeMap attributes)
{
    NamedItemElement::load_attributes(attributes);

    m_nValue = get_required_int("value",attributes);

    m_nSellValue = m_nValue / 2;

    std::string useType = get_required_string("use",attributes);
    m_eUseType = UseTypeFromString ( useType );

    std::string targetable = get_required_string("targetable",attributes);
    m_eTargetable = TargetableFromString ( targetable );

    if(has_attribute("sellValueMultiplier", attributes))
    {
        float multiplier = get_float("sellValueMultiplier",attributes);
        m_nSellValue = (int)(m_nValue * multiplier);
    }

    m_bReusable = get_required_bool("reusable",attributes);

    if(has_attribute("defaultTarget",attributes))
    {
        std::string str = get_string("defaultTarget",attributes);

        if( str == "party" )
            m_eDefaultTarget = PARTY;
        else if (str == "monsters")
            m_eDefaultTarget = MONSTERS;
        else throw CL_Exception("Bogus default target on regular item.");

    }
    else
    {
        m_eDefaultTarget = PARTY;
    }

}

bool RegularItem::handle_element(eElement element, Element * pElement)
{
    if(NamedItemElement::handle_element(element,pElement))
	return true;
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


std::string RegularItem::GetName() const
{
    return NamedItemElement::GetName();
}
uint RegularItem::GetMaxInventory() const
{
    return NamedItemElement::GetMaxInventory();
}
Item::eDropRarity RegularItem::GetDropRarity() const
{
    return NamedItemElement::GetDropRarity();
}
CL_Image RegularItem::GetIcon() const
{
    return NamedItemElement::GetIcon();
}

bool RegularItem::operator == ( const ItemRef &ref )
{
    if(ref.GetType() == ItemRef::NAMED_ITEM) return false;
    
    if(ref.GetItemName() != GetName()) return false;
    
    return true;
}





