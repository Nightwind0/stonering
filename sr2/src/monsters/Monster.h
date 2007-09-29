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
    virtual void attributeMultiply(eCharacterAttribute attr, double mult);
    virtual void attributeAdd(eCharacterAttribute attr, double add);
    // For boolean values.
    virtual void toggleAttribute(eCharacterAttribute attr, bool state);
    virtual double getMaxAttribute(eCharacterAttribute attr) const ;
    virtual double getMinAttribute(eCharacterAttribute attr) const;
    virtual double getSpellResistance(Magic::eMagicType type) const;
    virtual double getAttribute(eCharacterAttribute attr) const ;
    virtual int getAttributeInt(eCharacterAttribute attr) const;
    virtual bool getToggle(eCharacterAttribute attr) const;

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
};

};

#endif