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

class CL_Sprite;

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
        virtual uint   GetSP()const;
        virtual void   SetSP(uint amount);
        virtual double GetSpellResistance(Magic::eMagicType type) const;
        virtual double GetDamageCategoryResistance(DamageCategory::eDamageCategory type) const;
        virtual double GetAttribute(eCharacterAttribute attr) const;
        virtual bool   GetToggle(eCharacterAttribute attr) const;
        virtual void   SetToggle(eCharacterAttribute attr, bool state);
        virtual void   PermanentAugment(eCharacterAttribute attr, double augment);
        virtual void   Attacked();
        virtual void   Kill();
        virtual void   AddStatusEffect(StatusEffect *);
        virtual void   RemoveEffects(const std::string &name);
        virtual void   StatusEffectRound();
        virtual void   RollInitiative(void);
        virtual uint   GetInitiative(void)const;
        virtual double GetEquippedWeaponAttribute(Weapon::eAttribute) const;
        virtual double GetEquippedArmorAttribute(Armor::eAttribute) const;
        // Includes permanent augments
        double         GetBaseAttribute(eCharacterAttribute attr)const;
	/***************************************************************************
	* Sprites and images
	***************************************************************************/
	CL_Sprite  GetPortrait(ePortrait portrait);
        CL_Sprite  GetMapSprite() const { return m_mapSprite; }
        CL_Sprite  GetCurrentSprite() const { return m_currentSprite; }
        void       SetCurrentSprite(CL_Sprite sprite) { m_currentSprite = sprite; }

	/**************************************************************************
	* Battle stuff
	***************************************************************************/
        virtual CL_Pointf      GetBattlePos()const;
        virtual void           SetBattlePos(CL_Pointf point);
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
        bool       HasEquipment(Equipment::eSlot);
        Equipment* GetEquipment(Equipment::eSlot);
        
        /*************************************************************************
         * Skills
         * ***********************************************************************/
        void LearnSkill(const SkillRef& skill);
        bool HasSkill(const SkillRef& skill);
        
        
        // Element API
        virtual eElement WhichElement() const { return ECHARACTER; }
    private:
        typedef std::multimap<std::string,StatusEffect*> StatusEffectMap;
        typedef std::map<std::string,SpriteDefinition*> SpriteDefinitionMap;
        virtual bool handle_element(eElement, Element *);
        virtual void load_attributes(CL_DomNamedNodeMap);
        virtual void load_finished();
        void set_toggle_defaults();

        std::string m_name;
        std::map<eCharacterAttribute,double> m_augments;
        std::map<eCharacterAttribute,bool> m_toggles;
        std::map<Equipment::eSlot,Equipment*> m_equipment;
        std::set<std::string> m_skillset;
	CL_Sprite m_portraits;
        SpriteDefinitionMap m_sprite_definition_map;
        CharacterClass * m_pClass;
	uint m_nLevel;
        uint m_nInitiative;
	uint m_nXP;
        uint m_nSP;
        CL_Sprite m_mapSprite;
        CL_Sprite m_currentSprite;
	CL_Sprite m_portrait;
        StatusEffectMap m_status_effects;
        eType m_eType;
        CL_Pointf m_battle_pos;
    };

    inline void Character::RollInitiative(void)
    {
        // 20% variance
        int init = static_cast<int>(normal_random(GetAttribute(CA_LCK), GetAttribute(CA_LCK) * 0.2));
        m_nInitiative = cl_max(0,init);
    }

    inline uint Character::GetInitiative(void)const
    {
        return m_nInitiative;
    }


    inline CL_Pointf  Character::GetBattlePos()const
    {
        return m_battle_pos;
    }

    inline void      Character::SetBattlePos(CL_Pointf point)
    {
        m_battle_pos = point;
#if 0
		std::cout << m_name << ' ' << point.x << ',' << point.y << std::endl;
#endif
    }

}
#endif




