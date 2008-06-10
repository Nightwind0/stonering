#ifndef SR_MONSTER_ELEMENT_H
#define SR_MONSTER_ELEMENT_H

#include "Element.h"
#include "Character.h"

using StoneRing::ICharacter;

namespace StoneRing{

class CharacterClass;
class ItemRef;
class NamedScript;
class Stat;

class MonsterElement : public Element
{
public:
    MonsterElement();
    virtual ~MonsterElement();

    std::string GetName() const ;
    ICharacter::eType GetType() const { return m_eType; }

    void Invoke();
    void Round();
    void Die();

    bool HasClass() const { return m_bClass; }
    CharacterClass *GetClass() const { return m_pClass; }
   // std::string getSpriteResources() const { return mSpriteResources; }
    uint GetLevel() const { return m_nLevel; }

    std::list<ItemRef*>::const_iterator GetItemRefsBegin() const;
    std::list<ItemRef*>::const_iterator GetItemRefsEnd() const;

    // SpriteDefinition * getSpriteDefinition(const std::string &name)const;

    /** 
    * Element interface
    */
    virtual eElement WhichElement() const { return EMONSTER; }
private:
    /* Element stuff */
    virtual bool handle_element(eElement, Element * );
    virtual void load_attributes(CL_DomNamedNodeMap *);
    virtual void handle_text(const std::string &);
    virtual void load_finished();

    std::list<ItemRef*> m_items;

    NamedScript *m_pOnInvoke;
    NamedScript *m_pOnRound;
    NamedScript *m_pOnRemove;

    std::map<ICharacter::eCharacterAttribute,Stat*> m_stat_map;
    std::map<std::string,SpriteDefinition*> m_sprite_definitions_map;
    std::string m_sprite_resources;
    ICharacter::eType m_eType;
    bool m_bClass;
    CharacterClass *m_pClass;
    Character::eType m_eCharacterType;
    uint m_nLevel;
    std::string m_name;

};

};

#endif