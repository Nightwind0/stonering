#ifndef _SR_MONSTER_H
#define _SR_MONSTER_H

#include "Character.h"
#include "Stat.h"
#include "ScriptElement.h"
#include "StatusEffect.h"
#include "SpriteDefinition.h"
#include <map>
#include <list>

namespace StoneRing{

class SpriteDefinition;
class MonsterElement;

class Monster : public ICharacter
{
public:
    Monster(MonsterElement *pDefintion);
    virtual ~Monster();
    
    int getWorthPoints() const;
    std::list<ItemRef*>::const_iterator getDropsBegin() const;
    std::list<ItemRef*>::const_iterator getDropsEnd() const;
    void invoke();
    void round();
    void die();

    void setCellX(uint cellX) { mnCellX = cellX; }
    void setCellY(uint cellY) { mnCellY = cellY; }
    uint getCellX() const { return mnCellX; }
    uint getCellY() const { return mnCellY; }

    void setCurrentSprite(CL_Sprite *pSprite){mpSprite = pSprite;}
    CL_Sprite* getCurrentSprite() const { return mpSprite; }

    /** 
    * ICharacter interface
    */
    virtual eGender getGender() const;
    virtual std::string getName() const { return mName;}
    virtual eType getType() const;

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

    typedef std::multimap<std::string,StatusEffect*> StatusEffectMap;

    std::string mName;
    AttributeFile mAttributes;
    StatusEffectMap mStatusEffects;
    MonsterElement * mpMonsterDefinition;
    uint mnCellX;
    uint mnCellY;
    CL_Sprite *mpSprite;
};

};

#endif
