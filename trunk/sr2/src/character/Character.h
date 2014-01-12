#ifndef SR_CHARACTER_H
#define SR_CHARACTER_H

#include <ClanLib/core.h>
#include <set>
#include "sr_defines.h"
#include "Equipment.h"
#include "Magic.h"
#include "SpriteRef.h"
#include "Weapon.h"
#include "Armor.h"
#include "DamageCategory.h"
#include "ICharacter.h"
#include "TimedInterpolator.h"

namespace clan { 
  class Sprite;
}

namespace StoneRing{

    class CharacterClass; //fwd
    class SkillRef;
    class Animation;
    class WeaponTypeRef;
    class AnimationDefinition;
    class WeaponTypeSprite;
    class BattleMenu;
    class SpriteDefinition;
    class ICharacterGroup;


    class Character : public ICharacter, public Element
    {
    public:
        Character();
	
		enum ePortrait {
			PORTRAIT_DEFAULT=0,
			PORTRAIT_HAPPY=1,
			PORTRAIT_SAD=2,
			PORTRAIT_ANGRY=3,
			PORTRAIT_SCARED=4,
			PORTRAIT_SURPRISED=5
		};

        virtual eGender GetGender() const;
        virtual std::string GetName() const { return m_name; }
        virtual eType GetType() const { return m_eType; }
        //  virtual ICharacterGroup * GetGroup() const;
        virtual uint   GetLevel(void)const;
        virtual void   SetLevel(uint);
		virtual uint   GetXP()const;
		virtual void   SetXP(uint amount);
        virtual double GetDamageCategoryResistance(DamageCategory::eDamageCategory type) const;
        virtual double GetAttribute(eCharacterAttribute attr) const;
		virtual double GetLerpAttribute(eCharacterAttribute attr) const;
        virtual bool   GetToggle(eCharacterAttribute attr) const;
        virtual void   SetToggle(eCharacterAttribute attr, bool state);
        virtual void   PermanentAugment(eCharacterAttribute attr, double augment);
        virtual void   Attacked(ICharacter* pAttack, DamageCategory::eDamageCategory category, bool melee, int amount);
        virtual void   Kill();
        virtual void   Raise();
        virtual void   AddStatusEffect(StatusEffect *);
        virtual void   RemoveEffect(StatusEffect *);
        virtual double StatusEffectChance(StatusEffect *)const;
        virtual void   StatusEffectRound();
        virtual void   RemoveBattleStatusEffects();
        virtual void   RollInitiative(void);
        virtual uint   GetInitiative(void)const;
        virtual double GetEquippedWeaponAttribute(Weapon::eAttribute, Equipment::eSlot slot) const;
        virtual double GetEquippedArmorAttribute(Armor::eAttribute) const;
        virtual void   IterateStatusEffects(Visitor<StatusEffect*> &);
        // Includes permanent augments
        double         GetBaseAttribute(eCharacterAttribute attr)const;
        double         GetAttributeWithoutEquipment(eCharacterAttribute attr, Equipment * pExclude) const;
        uint   GetSP()const;
        void   SetSP(uint amount);  
		uint   GetLerpSP()const;
		/***************************************************************************
		* Sprites and images
		***************************************************************************/
		clan::Sprite  GetPortrait(ePortrait portrait);
        clan::Sprite  GetMapSprite() const { return m_mapSprite; }
        clan::Sprite  GetCurrentSprite(bool pure=true);
        void       SetCurrentSprite(clan::Sprite sprite) { m_currentSprite = sprite; }

		/**************************************************************************
		* Battle stuff
		***************************************************************************/
        virtual clan::Pointf      GetBattlePos()const;
        virtual void           SetBattlePos(clan::Pointf point);
        // Shortcuts to class data
        BattleMenu *           GetBattleMenu() const;
        CharacterClass *       GetClass() const { return m_pClass; }

		/**************************************************************************
		* Equipment
		***************************************************************************/
        // Equipment. If theres equipment in this slot already,
        // this overwrites it.
        void       Equip(Equipment::eSlot slot, Equipment *pEquip);
        // Returns a pointer to the equipment that was in that slot
        Equipment* Unequip(Equipment::eSlot);
        // Whether the slot is in use
        bool       HasEquipment(Equipment::eSlot)const;
        Equipment* GetEquipment(Equipment::eSlot)const;
		bool       CanEquip(Equipment * pEquip)const;
        
        /*************************************************************************
         * Skills
         * ***********************************************************************/
        void LearnSkill(const std::string& skill);
        bool HasSkill(const std::string& skill);
        
        // This will unequip anything thats not applicable
        void ChangeClass(CharacterClass * pClass);
        
        
        /*************************************************************************
         *  Save & Load
         * **********************************************************************/
        void Serialize(std::ostream& out);
        void Deserialize(std::istream& in);
        
        // Element API
        virtual eElement WhichElement() const { return ECHARACTER; }
		virtual std::string GetDebugId() const { return m_name; }		        
    private:
        typedef std::map<std::string,StatusEffect*> StatusEffectMap;
        typedef std::map<std::string,SpriteDefinition*> SpriteDefinitionMap;
        typedef std::map<std::string,uint> StatusEffectRounds;

        virtual bool handle_element(eElement, Element *);
        virtual void load_attributes(clan::DomNamedNodeMap);
        virtual void load_finished();
        void set_toggle_defaults();

        std::string m_name;
        std::map<eCharacterAttribute,double> m_augments;
        std::map<eCharacterAttribute,bool> m_toggles;
        std::map<Equipment::eSlot,Equipment*> m_equipment;
		std::map<eCharacterAttribute,TimedInterpolator<double> > m_lerped_attrs;
        std::set<std::string> m_skillset;
		clan::Sprite m_portraits;
        SpriteDefinitionMap m_sprite_definition_map;
        CharacterClass * m_pClass;
		uint m_nLevel;
        uint m_nInitiative;
		uint m_nXP;
        uint m_nSP;
		TimedInterpolator<int> m_lerped_sp;
        clan::Sprite m_mapSprite;
        clan::Sprite m_currentSprite;
		clan::Sprite m_portrait;
        StatusEffectMap m_status_effects;
        StatusEffectRounds m_status_effect_rounds;
        eType m_eType;
        clan::Pointf m_battle_pos;
    };

    inline void Character::RollInitiative(void)
    {
        // 20% variance
        int init = static_cast<int>(normal_random(GetAttribute(CA_LCK), GetAttribute(CA_LCK) * 0.2));
        m_nInitiative = std::max(0,init);
    }

    inline uint Character::GetInitiative(void)const
    {
        return m_nInitiative;
    }


    inline clan::Pointf  Character::GetBattlePos()const
    {
        return m_battle_pos;
    }

    inline void      Character::SetBattlePos(clan::Pointf point)
    {
        m_battle_pos = point;
#if 0
		std::cout << m_name << ' ' << point.x << ',' << point.y << std::endl;
#endif
    }

}
#endif




