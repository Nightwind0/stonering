#ifndef __ICHARACTER_H_INCLUDED
#define __ICHARACTER_H_INCLUDED

#include "sr_defines.h"
#include "sr_defines.h"
#include "Equipment.h"
#include "Magic.h"
#include "SpriteRef.h"
#include "Weapon.h"
#include "Armor.h"
#include "DamageCategory.h"
#include "ICharacter.h"
#ifndef WIN32
#include "steel/SteelType.h"
#else
#include "SteelType.h"
#endif

namespace StoneRing{
    class StatusEffect;

    class ICharacter: public Steel::SteelType::IHandle
    {
    public:
        enum eGender { NEUTER = 0, MALE=0x1, FEMALE=0x2, HERMAPHRODITE = MALE | FEMALE };
        enum eType   { NONLIVING, LIVING, MAGICAL };
        enum eCharacterAttribute
        {
            CA_INVALID=0,

            _START_OF_TRANSIENTS,
            CA_HP,
            CA_MP,
            CA_BP,
            _END_OF_TRANSIENTS,
            _START_OF_INTS,
            CA_STR,              // Part of determining dmg of physical attack.
            CA_DEF,              // Physical defense
            CA_MAG,              // Magic power
            CA_RST,              // Magic resistance
            CA_LCK,              // Similar to initiative. Also helps in other aspects of the game...
            _END_OF_INTS,
            _START_OF_REALS,
            CA_JOY,              // Increases experience gained (Multiplier)
            CA_DEX,              // Chances of a hit connecting (0-1)
            CA_EVD,              // Chances of evading an attack(0-1)
            CA_MP_COST,
			CA_BP_COST,
            _START_OF_DAMAGE_CATEGORIES,
            CA_BASH_DEF,
            CA_PIERCE_DEF,
            CA_SLASH_DEF,
            CA_HOLY_RST,
            CA_DARK_RST,
            CA_FIRE_RST,
            CA_WATER_RST,
            CA_WIND_RST,
            CA_EARTH_RST,
            CA_GRAVITY_RST,
            CA_ELECTRIC_RST,
            _END_OF_DAMAGE_CATEGORIES,
            _END_OF_REALS,
            _START_OF_TOGGLES,
            CA_DRAW_ILL,
            CA_DRAW_STONE,
            CA_DRAW_BERSERK,
            CA_DRAW_WEAK,
            CA_DRAW_PARALYZED,
            CA_DRAW_TRANSLUCENT,
            CA_DRAW_MINI,
            CA_DRAW_FLIPPED,
            CA_DRAW_STILL,
            CA_VISIBLE,
            CA_CAN_ACT,
            CA_CAN_FIGHT,
            CA_CAN_CAST,
            CA_CAN_SKILL,
            CA_CAN_ITEM,
            CA_CAN_RUN,
            CA_ALIVE,
            _END_OF_TOGGLES,
            _MAXIMA_BASE,
            CA_MAXHP = _MAXIMA_BASE + CA_HP,
            CA_MAXMP = _MAXIMA_BASE + CA_MP,
            CA_MAXBP = _MAXIMA_BASE + CA_BP,

            _LAST_CHARACTER_ATTR_
        };

        enum eCommonAttribute
        {
            CA_ENCOUNTER_RATE = _LAST_CHARACTER_ATTR_ + 1,
            CA_GOLD_DROP_RATE,
            CA_ITEM_DROP_RATE,
            CA_PRICE_MULTIPLIER,
            CA_EXP_MULTIPLIER,
            CA_IDOL_SLOTS,
            _LAST_COMMON_ATTR_
        };

        virtual eGender GetGender() const=0;
        virtual eType GetType() const=0;
        virtual std::string GetName() const=0;
        //   virtual ICharacterGroup* GetGroup() const=0;
        virtual uint GetLevel()const=0;
        virtual void SetLevel(uint level)=0;
		virtual bool IsMonster() const { return false; }
        virtual double GetAttribute    (eCharacterAttribute attr) const = 0;
		virtual double GetLerpAttribute (eCharacterAttribute attr) const = 0;
        virtual double GetDamageCategoryResistance(DamageCategory::eDamageCategory type) const = 0;

        virtual bool   GetToggle(eCharacterAttribute attr) const = 0;
        virtual void   SetToggle(eCharacterAttribute attr, bool toggle) = 0;
        virtual void   AddStatusEffect(StatusEffect *)=0;
        virtual void   RemoveEffect(StatusEffect *)=0; // Removes by name
        virtual double StatusEffectChance(StatusEffect *)const=0;
        virtual double GetEquippedWeaponAttribute(Weapon::eAttribute, Equipment::eSlot slot) const = 0;
        virtual double GetEquippedArmorAttribute(Armor::eAttribute) const = 0;
        virtual void   StatusEffectRound()=0;
        virtual void   PermanentAugment(eCharacterAttribute attr, double augment)=0;
        virtual void   RollInitiative(void)=0;
        virtual uint   GetInitiative(void)const=0;
        virtual void   Kill()=0;
        virtual void   Raise()=0;
        virtual void   Attacked(ICharacter* pAttacker, DamageCategory::eDamageCategory category, bool melee, int amount)=0;
        virtual CL_Pointf  GetBattlePos()const=0;
        virtual void       SetBattlePos(CL_Pointf point)=0;
		virtual CL_Sprite  GetCurrentSprite(bool pure=true)=0;
        
        virtual void   IterateStatusEffects(Visitor<StatusEffect*> &)=0;

        // Static API
        static eCharacterAttribute CharAttributeFromString(const std::string &str);
        static eCommonAttribute CommonAttributeFromString(const std::string &str);
        static uint CAFromString(const std::string &str);
        static std::string CAToString(uint);
        static std::string CAToLabel(uint);
        static bool IsDamageCategoryAttribute(eCharacterAttribute attr);
		static bool IsCostMultiplier(eCharacterAttribute attr);
        static bool IsInteger(eCharacterAttribute attr);
        static bool IsReal(eCharacterAttribute attr);
        static bool IsToggle(eCharacterAttribute attr);
        static bool IsTransient(eCharacterAttribute attr);
        static bool ToggleDefaultTrue(eCharacterAttribute attr);
        static eCharacterAttribute GetMaximumAttribute(eCharacterAttribute attr);


        ///@todo API for different battle animations TBD
    private:
    };


}
#endif // __ICHARACTER_H_INCLUDED
