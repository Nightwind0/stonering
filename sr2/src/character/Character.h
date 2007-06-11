#ifndef SR_CHARACTER_H
#define SR_CHARACTER_H

#include "sr_defines.h"
#include "Equipment.h"

namespace StoneRing{

    class CharacterClass; //fwd
    class SkillRef;
    class Animation;
    class WeaponTypeRef;
    class AnimationDefinition;
    class WeaponTypeSprite;
    class BattleMenu;

    //str|dex|evd|mag|rst|spr
    enum eCharacterAttribute
    {
        CA_HP,
        CA_MP,
        CA_STR,              // Part of determining dmg of physical attack. 
        CA_DEF,              // Physical defense
        CA_DEX,              // Chances of a hit connecting (0-1)
        CA_EVD,              // Chances of evading an attack(0-1)
        CA_MAG,              // Magic power
        CA_RST,              // Magic resistance
        CA_LCK,              // Similar to initiative. Also helps in other aspects of the game...
        CA_JOY,              // Increases experience gained (Multiplier)
        CA_LEVEL,
        _START_OF_TOGGLES,
        CA_DRAW_ILL,
        CA_DRAW_STONE,
        CA_DRAW_BERSERK,
        CA_DRAW_WEAK,
        CA_DRAW_PARALYZED,
        CA_DRAW_TRANSLUCENT,
        CA_CAN_ACT,
        CA_CAN_FIGHT,
        CA_CAN_CAST,
        CA_CAN_SKILL,
        CA_CAN_ITEM,
        CA_CAN_RUN,
        CA_ALIVE,
        _LAST_TOGGLE,
        _MAXIMA_BASE,
        CA_MAXHP = _MAXIMA_BASE + CA_HP,
        CA_MAXMP = _MAXIMA_BASE + CA_MP,

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


    class ICharacter
    {
    public:
        enum eGender { MALE, FEMALE, NEUTER };

        virtual eGender getGender() const=0;
        virtual std::string getName() const=0;

        virtual void attributeMultiply(eCharacterAttribute attr, double mult) = 0;
        virtual void attributeAdd(eCharacterAttribute attr, double add)=0;
        // For boolean values.
        virtual void toggleAttribute(eCharacterAttribute attr, bool state)  = 0;
        virtual double getMaxAttribute(eCharacterAttribute attr) const = 0;
        virtual double getMinAttribute(eCharacterAttribute attr) const = 0;
        virtual double getAttribute(eCharacterAttribute attr) const = 0;
        virtual int getAttributeInt(eCharacterAttribute attr) const = 0;
        virtual bool getToggle(eCharacterAttribute attr) const =0;

        ///@todo
        // API for different battle animations TBD
        
    private:
    };


    class ICharacterGroup
    {
    public:
        virtual uint getCharacterCount() const = 0;
        virtual uint getSelectedCharacterIndex() const = 0;
        virtual uint getCasterCharacterIndex() const = 0;
        virtual ICharacter * getCharacter(uint index) const = 0;
        virtual ICharacter * getSelectedCharacter() const = 0;
        virtual ICharacter * getCasterCharacter() const = 0;
    private:
    };

    class Character : public ICharacter
    {
    public:
        Character();

        virtual eGender getGender() const;
        virtual std::string getName() const { return mName; }
        virtual void attributeMultiply(eCharacterAttribute attr, double mult);
        virtual void attributeAdd(eCharacterAttribute attr, double add);
        // For boolean values.
        virtual void toggleAttribute(eCharacterAttribute attr, bool state);
        virtual double getMaxAttribute(eCharacterAttribute attr) const;
        virtual double getMinAttribute(eCharacterAttribute attr) const;
        virtual double getAttribute(eCharacterAttribute attr) const;
        virtual bool getToggle(eCharacterAttribute attr) const;
        virtual int getAttributeInt(eCharacterAttribute attr) const;
        // Shortcuts to class data
        BattleMenu * getBattleMenu() const;
        std::string getClassName() const;
        // Equipment
        void equip(Equipment::eSlot slot, Equipment *pEquip);
        void unequip(Equipment::eSlot);

    private:
        std::string mName;
        CharacterClass * mpClass;
    };

};
#endif




