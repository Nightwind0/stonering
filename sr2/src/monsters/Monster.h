#ifndef _SR_MONSTER_H
#define _SR_MONSTER_H

#include "Character.h"
#include "Stat.h"
#include "ScriptElement.h"
#include "StatusEffect.h"
#include "SpriteDefinition.h"
#include <map>
#include <list>
#include "DamageCategory.h"




namespace StoneRing
{

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
        void Invoke(const ParameterList& params);
        void Round();
        void Round(const ParameterList& params);
        void Die();
        void Die(const ParameterList& params);

        void SetCellX(uint cellX)
        {
            m_nCellX = cellX;
        }
        void SetCellY(uint cellY)
        {
            m_nCellY = cellY;
        }
        uint GetCellX() const
        {
            return m_nCellX;
        }
        uint GetCellY() const
        {
            return m_nCellY;
        }

        int GetOffsetX() const
        {
            return m_offsetx;
        }
        int GetOffsetY() const
        {
            return m_offsety;
        }

        void SetOffsetX(int x)
        {
            m_offsetx = x;
        }
        void SetOffsetY(int y)
        {
            m_offsety = y;
        }

        void SetCurrentSprite(CL_Sprite sprite)
        {
            m_sprite = sprite;
        }
        CL_Sprite GetCurrentSprite() const
        {
            return m_sprite;
        }

        /**
        * ICharacter interface
        */
        virtual eGender GetGender() const;
        virtual std::string GetName() const
        {
            return m_name;
        }
        virtual eType GetType() const;
        //virtual ICharacterGroup* GetGroup() const { return NULL; }
        virtual uint GetLevel(void)const;
        virtual void SetLevel(uint);
        virtual double GetSpellResistance(Magic::eMagicType type) const;
        virtual double GetDamageCategoryResistance(eDamageCategory type) const;
        virtual double  GetAttribute(eCharacterAttribute attr) const;
        virtual bool GetToggle(eCharacterAttribute attr) const;
        virtual void SetToggle(eCharacterAttribute attr, bool state);
        virtual double GetEquippedWeaponAttribute(Weapon::eAttribute) const
        {
            return 0.0;
        }
        virtual double GetEquippedArmorAttribute(Armor::eAttribute) const
        {
            return 0.0;
        }
        virtual void PermanentAugment(eCharacterAttribute attr, double augment);
        virtual void AddStatusEffect(StatusEffect *);
        virtual void RemoveEffects(const std::string &name);
        virtual void StatusEffectRound();
        virtual void RollInitiative(void);
        virtual uint GetInitiative(void)const;
        virtual void Kill();
        virtual void Attacked();

        virtual CL_Point GetBattlePos() const;
        virtual void     SetBattlePos(CL_Point pos);

        eDamageCategory GetDefaultDamageCategory(void)const;

        void MarkDeathAnimated()
        {
            m_bDeathAnimated = true;
        }
        void ClearDeathAnimated()
        {
            m_bDeathAnimated = false;
        }
        bool DeathAnimated()const
        {
            return m_bDeathAnimated;
        }
    private:

        typedef std::multimap<std::string,StatusEffect*> StatusEffectMap;
        void set_toggle_defaults();
        void set_transients();

        int m_offsetx;
        int m_offsety;
        std::string m_name;
        uint m_nLevel;
        std::map<eCharacterAttribute,double> m_augments;
        std::map<eCharacterAttribute,bool> m_toggles;
        StatusEffectMap m_status_effects;
        MonsterElement * m_pMonsterDefinition;
        uint m_nCellX;
        uint m_nCellY;
        uint m_nInitiative;
        CL_Sprite m_sprite;
        bool m_bDeathAnimated;
        CL_Point m_battle_pos;
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

    inline CL_Point Monster::GetBattlePos() const
    {
        return m_battle_pos;
    }

    inline void     Monster::SetBattlePos(CL_Point pos)
    {
        m_battle_pos = pos;
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
