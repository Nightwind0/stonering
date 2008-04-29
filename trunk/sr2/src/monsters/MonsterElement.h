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

    std::string getName() const ;
    ICharacter::eType getType() const { return meType; }

    void invoke();
    void round();
    void die();

    bool hasClass() const { return mbClass; }
    CharacterClass *getClass() const { return mpClass; }
   // std::string getSpriteResources() const { return mSpriteResources; }
    uint getLevel() const { return mnLevel; }

    std::list<ItemRef*>::const_iterator getItemRefsBegin() const;
    std::list<ItemRef*>::const_iterator getItemRefsEnd() const;

    // SpriteDefinition * getSpriteDefinition(const std::string &name)const;

    /** 
    * Element interface
    */
    virtual eElement whichElement() const { return EMONSTER; }
private:
    /* Element stuff */
    virtual bool handleElement(eElement, Element * );
    virtual void loadAttributes(CL_DomNamedNodeMap *);
    virtual void handleText(const std::string &);
    virtual void loadFinished();

    std::list<ItemRef*> mItems;

    NamedScript *mpOnInvoke;
    NamedScript *mpOnRound;
    NamedScript *mpOnRemove;

    std::map<ICharacter::eCharacterAttribute,Stat*> mStatMap;
    std::map<std::string,SpriteDefinition*> mSpriteDefinitionsMap;
    std::string mSpriteResources;
    ICharacter::eType meType;
    bool mbClass;
    CharacterClass *mpClass;
    Character::eType meCharacterType;
    uint mnLevel;
    std::string mName;

};

};

#endif