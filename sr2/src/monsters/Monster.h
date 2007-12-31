#ifndef _SR_MONSTER_H
#define _SR_MONSTER_H

#include "Character.h"
#include "Stat.h"
#include "ScriptElement.h"
#include <map>
#include <list>

namespace StoneRing{

class Monster : public ICharacter, public Element
{
public:
    Monster();
    virtual ~Monster();
    
    int getWorthPoints() const;
    std::list<ItemRef*> getDrops() const;
    /** 
    * Element interface
    */
    virtual eElement whichElement() const { return EMONSTER; }

    /** 
    * ICharacter interface
    */
    virtual eGender getGender() const;
    virtual std::string getName() const;
    virtual eType getType() const;

    // For boolean values.
    virtual double getSpellResistance(Magic::eMagicType type) const;
    virtual double getAttribute(eCharacterAttribute attr) const ;
    virtual bool getToggle(eCharacterAttribute attr) const;
    virtual void fixAttribute(eCharacterAttribute attr, double value);
    virtual void fixAttribute(eCharacterAttribute attr, bool state);
    virtual void attachMultiplication(eCharacterAttribute attr, double factor);
    virtual void detachMultiplication(eCharacterAttribute attr, double factor);
    virtual void attachAddition(eCharacterAttribute attr, double value);
    virtual void detachAddition(eCharacterAttribute attr, double value);
    virtual void addStatusEffect(StatusEffect *);
    virtual void removeEffects(const std::string &name);
    virtual void statusEffectRound();

private:
    /* Element stuff */
    virtual bool handleElement(eElement, Element * );
    virtual void loadAttributes(CL_DomNamedNodeMap *);
    virtual void handleText(const std::string &);
    virtual void loadFinished();

    std::map<eCharacterAttribute,Stat*> mStatMap;
    std::list<ItemRef*> mItems;
    ScriptElement *mpScript;
    std::string mName;
    std::string mSpriteResources;
    bool mbClass;
    CharacterClass *mpClass;
    uint mnLevel;
    eType meType;
    AttributeFile mAttributes;
};

};

#endif
