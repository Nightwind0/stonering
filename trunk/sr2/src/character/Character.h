#ifndef SR_CHARACTER_H
#define SR_CHARACTER_H

#include <ClanLib/core.h>
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

        virtual eGender GetGender() const;
        virtual std::string GetName() const { return m_name; }
        virtual eType GetType() const { return m_eType; }
        //  virtual ICharacterGroup * GetGroup() const;
        virtual uint   GetLevel(void)const;
        virtual void   SetLevel(uint);
        virtual double GetSpellResistance(Magic::eMagicType type) const;
        virtual double GetDamageCategoryResistance(eDamageCategory type) const;
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

        CL_Sprite * GetMapSprite() const { return m_pMapSprite; }
        CL_Sprite * GetCurrentSprite() const { return m_pCurrentSprite; }
        void SetCurrentSprite(CL_Sprite *pSprite) { m_pCurrentSprite = pSprite; }

        // Shortcuts to class data
        BattleMenu * GetBattleMenu() const;
        CharacterClass * GetClass() const { return m_pClass; }

        // Equipment. If theres equipment in this slot already,
        // this overwrites it.
        void Equip(Equipment::eSlot slot, Equipment *pEquip);
        // Returns a pointer to the equipment that was in that slot
        Equipment* Unequip(Equipment::eSlot);
        // Whether the slot is in use
        bool HasEquipment(Equipment::eSlot);

        Equipment* GetEquipment(Equipment::eSlot);

        // Includes permanent augments
        double GetBaseAttribute(eCharacterAttribute attr)const;

        // Element API
        virtual eElement WhichElement() const { return ECHARACTER; }
    private:
        typedef std::multimap<std::string,StatusEffect*> StatusEffectMap;
        typedef std::map<std::string,SpriteDefinition*> SpriteDefinitionMap;
        virtual bool handle_element(eElement, Element * );
        virtual void load_attributes(CL_DomNamedNodeMap *);
        virtual void load_finished();
        void set_toggle_defaults();

        std::string m_name;
        std::map<eCharacterAttribute,double> m_augments;
        std::map<eCharacterAttribute,bool> m_toggles;
        std::map<Equipment::eSlot,Equipment*> m_equipment;
        SpriteDefinitionMap m_sprite_definition_map;
        CharacterClass * m_pClass;
        uint m_nLevel;
        uint m_nInitiative;
        CL_Sprite *m_pMapSprite;
        CL_Sprite *m_pCurrentSprite;
        StatusEffectMap m_status_effects;
        eType m_eType;
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

}
#endif




