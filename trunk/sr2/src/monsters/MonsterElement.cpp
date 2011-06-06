#include "MonsterElement.h"
#include "IApplication.h"
#include "CharacterManager.h"
#include "NamedScript.h"
#include "Stat.h"
#include "SpriteDefinition.h"
#include "DamageCategory.h"

using StoneRing::MonsterElement;
using StoneRing::ItemRef;
using StoneRing::SpriteDefinition;
using StoneRing::Element;

MonsterElement::MonsterElement()
:m_pOnInvoke(NULL),m_pOnRound(NULL),m_pOnRemove(NULL),m_nLevel(1)
{
}

MonsterElement::~MonsterElement()
{
    delete m_pOnRound;
    delete m_pOnInvoke;
    delete m_pOnRemove;
}

void MonsterElement::Invoke()
{
    if(m_pOnInvoke)
        m_pOnInvoke->ExecuteScript();
}

void MonsterElement::Invoke(const ParameterList& params)
{
    if(m_pOnInvoke)
        m_pOnInvoke->ExecuteScript(params);
}

void MonsterElement::Round()
{
    if(m_pOnRound)
        m_pOnRound->ExecuteScript();
}

void MonsterElement::Round(const ParameterList& params)
{
    if(m_pOnRound)
        m_pOnRound->ExecuteScript(params);
}

void MonsterElement::Die()
{
    if(m_pOnRemove)
        m_pOnRemove->ExecuteScript();
}

void MonsterElement::Die(const ParameterList& params)
{
    if(m_pOnRemove)
        m_pOnRemove->ExecuteScript(params);
}

std::list<ItemRef*>::const_iterator MonsterElement::GetItemRefsBegin() const
{
    return m_items.begin();
}

std::list<ItemRef*>::const_iterator MonsterElement::GetItemRefsEnd() const
{
    return m_items.end();
}

#if 0
SpriteDefinition * MonsterElement::getSpriteDefinition(const std::string &name) const
{
    // Throw error if missing?
    std::map<std::string,SpriteDefinition*>::const_iterator it = mSpriteDefinitionsMap.find(name);

    if(it == mSpriteDefinitionsMap.end()) throw CL_Exception("Sprite definition: " + name + " missing");
    return it->second;
}
#endif

/** Element Stuff **/
bool MonsterElement::handle_element(Element::eElement element, StoneRing::Element * pElement)
{
    switch(element)
    {
    case ESTAT:
        {
            Stat * pStat = dynamic_cast<Stat*>(pElement);
            m_stat_map[pStat->GetAttribute()] = pStat;
            break;
        }
    case EITEMREF:
        m_items.push_back ( dynamic_cast<ItemRef*>(pElement) );
        break;
    case EONROUND:
        m_pOnRound = dynamic_cast<NamedScript*>(pElement);
        assert(m_pOnRound != NULL);
        break;
    case EONINVOKE:
        m_pOnInvoke = dynamic_cast<NamedScript*>(pElement);
        break;
    case EONREMOVE:
        m_pOnRemove = dynamic_cast<NamedScript*>(pElement);
        break;
    case ESPRITEDEFINITION:
        {
            SpriteDefinition * pSpriteDef = dynamic_cast<SpriteDefinition*>(pElement);
            m_sprite_definitions_map[pSpriteDef->GetName()] = pSpriteDef;
            break;
        }
    default:
        return false;
    }

    return true;
}

void MonsterElement::load_attributes(CL_DomNamedNodeMap attr)
{
    m_name = get_required_string("name",attr);
    m_sprite_resources = get_required_string("spriteResources",attr);
    std::string mode = get_required_string("mode",attr);
    m_nLevel = get_required_int("level",attr);
    std::string type = get_implied_string("type",attr,"living");

    std::string damageCategory = get_implied_string("damageCategory",attr,"bash"); // they punch by default

    m_eDamageCategory = DamageCategory::DamageCategoryFromString(damageCategory);

    if(mode == "manual")
    {
        m_bClass = false;
    }
    else if(mode=="class")
    {
        m_bClass = true;
    }
    else throw CL_Exception("Unknown monster mode");

    if(type == "living")
        m_eType = ICharacter::LIVING;
    else if(type == "nonliving")
        m_eType = ICharacter::NONLIVING;
    else if(type == "magical")
        m_eType = ICharacter::MAGICAL;

    if(m_bClass)
    {
        std::string classname = get_required_string("class",attr);
        m_pClass = IApplication::GetInstance()->GetCharacterManager()->GetClass(classname);
    }

}

void MonsterElement::handle_text(const std::string &)
{
}

void MonsterElement::load_finished()
{
    if(!m_pOnRound) throw CL_Exception("Missing onRound on monster " + m_name);

    // Check for all the required stats
    if(!m_bClass)
    {
    }

    // TODO: Make sure the required sprites are available
#ifndef NDEBUG
    std::cout << '\t' << m_name << std::endl;
#endif

}

std::string MonsterElement::GetName() const
{
    return m_name;
}
