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
    
    int GetWorthPoints() const;
    std::list<ItemRef*>::const_iterator GetDropsBegin() const;
    std::list<ItemRef*>::const_iterator GetDropsEnd() const;
    void Invoke();
    void Round();
    void Die();

    void SetCellX(uint cellX) { m_nCellX = cellX; }
    void SetCellY(uint cellY) { m_nCellY = cellY; }
    uint GetCellX() const { return m_nCellX; }
    uint GetCellY() const { return m_nCellY; }

    void SetCurrentSprite(CL_Sprite *pSprite){m_pSprite = pSprite;}
    CL_Sprite* GetCurrentSprite() const { return m_pSprite; }

    /** 
    * ICharacter interface
    */
    virtual eGender GetGender() const;
    virtual std::string GetName() const { return m_name;}
    virtual eType GetType() const;
	//virtual ICharacterGroup* GetGroup() const { return NULL; }

    virtual double GetSpellResistance(Magic::eMagicType type) const;
    virtual double GetAttribute(eCharacterAttribute attr) const ;
    virtual bool GetToggle(eCharacterAttribute attr) const;
    virtual void FixAttribute(eCharacterAttribute attr, double value);
    virtual void FixAttribute(eCharacterAttribute attr, bool state);
    virtual void AttachMultiplication(eCharacterAttribute attr, double factor);
    virtual void DetachMultiplication(eCharacterAttribute attr, double factor);
    virtual void AttachAddition(eCharacterAttribute attr, double value);
    virtual void DetachAddition(eCharacterAttribute attr, double value);
    virtual void AddStatusEffect(StatusEffect *);
    virtual void RemoveEffects(const std::string &name);
    virtual void StatusEffectRound();
private:

    typedef std::multimap<std::string,StatusEffect*> StatusEffectMap;

    std::string m_name;
    AttributeFile m_attributes;
    StatusEffectMap m_status_effects;
    MonsterElement * m_pMonsterDefinition;
    uint m_nCellX;
    uint m_nCellY;
    CL_Sprite *m_pSprite;
};

};

#endif