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
    virtual uint GetLevel(void)const;
    virtual void SetLevel(uint);
    virtual double GetSpellResistance(Magic::eMagicType type) const;
    virtual double  GetAttribute(eCharacterAttribute attr) const;
    virtual bool GetToggle(eCharacterAttribute attr) const;
    virtual void SetToggle(eCharacterAttribute attr, bool state);
    virtual double GetEquippedWeaponAttribute(Weapon::eAttribute) const { return 0.0; }
    virtual double GetEquippedArmorAttribute(Armor::eAttribute) const { return 0.0; }
    virtual void PermanentAugment(eCharacterAttribute attr, double augment);
    virtual void AddStatusEffect(StatusEffect *);
    virtual void RemoveEffects(const std::string &name);
    virtual void StatusEffectRound();
    virtual void RollInitiative(void);
    virtual uint GetInitiative(void)const;
    virtual void Kill();
private:

    typedef std::multimap<std::string,StatusEffect*> StatusEffectMap;

    std::string m_name;
    uint m_nLevel;
    std::map<eCharacterAttribute,double> m_augments;
    StatusEffectMap m_status_effects;
    MonsterElement * m_pMonsterDefinition;
    uint m_nCellX;
    uint m_nCellY;
    uint m_nInitiative;
    CL_Sprite *m_pSprite;
};


inline void Monster::RollInitiative(void)
{
    int init = static_cast<int>(normal_random(GetAttribute(CA_LCK), GetAttribute(CA_LCK) * 0.2));
    m_nInitiative = std::max(0,init);

}
inline uint Monster::GetInitiative(void)const
{
    return m_nInitiative;
}

class MonsterParty : public ICharacterGroup
{
public:
    MonsterParty(){}
    virtual ~MonsterParty(){}
    void AddMonster(Monster * pMonster);
    virtual uint GetCharacterCount() const;
    virtual ICharacter * GetCharacter(uint index) const;
private:
    std::vector<Monster*> m_monsters;
};

}

#endif
