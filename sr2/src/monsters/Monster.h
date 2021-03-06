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
#include "ICharacterGroup.h"



namespace StoneRing
{

    class SpriteDefinition;
    class MonsterElement;

    class Monster : public ICharacter
    {
    public:
        Monster(MonsterElement *pDefintion);
        virtual ~Monster();
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

        void SetCurrentSprite(clan::Sprite sprite);

        clan::Sprite GetCurrentSprite(bool pure=true);

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
		virtual bool IsMonster()const { return true; }
        virtual double GetDamageCategoryResistance(DamageCategory::eDamageCategory type) const;
        virtual double GetAttribute(eCharacterAttribute attr) const;
		virtual double GetLerpAttribute(eCharacterAttribute attr) const;
        virtual bool GetToggle(eCharacterAttribute attr) const;
        virtual void SetToggle(eCharacterAttribute attr, bool state);
        virtual double GetEquippedWeaponAttribute(Weapon::eAttribute attr, Equipment::eSlot) const
        {
			if(attr == Weapon::ATTACK)
				return 0.75 * GetAttribute(ICharacter::CA_STR); // Weapon equal to  75% of str is considered built-in
				
			return 0.0;
        }
        virtual double GetEquippedArmorAttribute(Armor::eAttribute attr) const
        {
			if(attr == Armor::AC)
				return 0.75 * GetAttribute(ICharacter::CA_DEF); // Monsters have built-in armor equal 75% of def
			return 0.0;
        }
        virtual void PermanentAugment(eCharacterAttribute attr, double augment);
        virtual void AddStatusEffect(StatusEffect *);
        virtual void RemoveEffect(StatusEffect *);
        virtual double StatusEffectChance(StatusEffect *)const;
        virtual void StatusEffectRound();
        virtual void RollInitiative(void);
        virtual uint GetInitiative(void)const;
        virtual void Kill();
        virtual void Raise();
        virtual void Attacked(ICharacter *pAttacker, DamageCategory::eDamageCategory category, bool melee, int amount);

        
        virtual void   IterateStatusEffects(Visitor<StatusEffect*> &);        

        DamageCategory::eDamageCategory GetDefaultDamageCategory(void)const;

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

        typedef std::map<std::string,StatusEffect*> StatusEffectMap;
        void set_toggle_defaults();
        void set_transients();

        int m_offsetx;
        int m_offsety;
        std::string m_name;
        uint m_nLevel;
        std::map<eCharacterAttribute,double> m_augments;
        std::map<eCharacterAttribute,bool> m_toggles;
        StatusEffectMap m_status_effects;
        std::map<std::string,uint> m_status_effect_rounds;
        MonsterElement * m_pMonsterDefinition;
        uint m_nCellX;
        uint m_nCellY;
        uint m_nInitiative;
        clan::Sprite m_sprite;
        bool m_bDeathAnimated;
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
