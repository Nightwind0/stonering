#ifndef SR_CHARACTER_H
#define SR_CHARACTER_H

#include "sr_defines.h"
#include "Equipment.h"
#include "Magic.h"

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
        CA_INVALID, // Invalid attribute
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
        _END_OF_TOGGLES,
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
        enum eGender { NEUTER = 0, MALE=0x1, FEMALE=0x2, HERMAPHRODITE = MALE | FEMALE };
        enum eType   { NONLIVING, LIVING, MAGICAL }; 

        virtual eGender getGender() const=0;
        virtual eType getType() const=0;
        virtual std::string getName() const=0;

        virtual double getAttribute(eCharacterAttribute attr) const = 0;
        virtual double getSpellResistance(Magic::eMagicType type) const = 0;
        virtual bool getToggle(eCharacterAttribute attr) const = 0;
        virtual void fixAttribute(eCharacterAttribute attr, double value) = 0;
        virtual void fixAttribute(eCharacterAttribute attr, bool toggle) =0;
        virtual void attachMultiplication(eCharacterAttribute attr, double factor) = 0;
        virtual void detachMultiplication(eCharacterAttribute attr, double factor) =0;
        virtual void attachAddition(eCharacterAttribute attr, double value) = 0;
        virtual void detachAddition(eCharacterAttribute attr, double value)=0;
        virtual void addStatusEffect(StatusEffect *)=0;
        virtual void removeEffects(const std::string &name)=0;
        virtual void statusEffectRound()=0;
        ///@todo API for different battle animations TBD
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

    class AttributeFile
    {
    public:
        double getAttribute(eCharacterAttribute)const;
        bool getToggle(eCharacterAttribute)const;
        void attachMultiplication(eCharacterAttribute,double);
        void attachAddition(eCharacterAttribute,double);  
        void detachMultiplication(eCharacterAttribute,double);
        void detachAddition(eCharacterAttribute,double);  
        void fixAttribute(eCharacterAttribute attr, double value);
        void fixAttribute(eCharacterAttribute attr, bool toggle);
    private:
        typedef std::map<eCharacterAttribute,double> attr_doubles;
        typedef std::map<eCharacterAttribute,bool> attr_bools;
        attr_doubles::const_iterator find_attr(eCharacterAttribute)const;
        double find_multiplier(eCharacterAttribute)const;
        double find_addition(eCharacterAttribute)const;
        attr_bools::const_iterator find_toggle(eCharacterAttribute)const;
        static void assert_real(eCharacterAttribute);
        static void assert_bool(eCharacterAttribute);


        attr_doubles mRealAttributes;
        attr_bools mToggles;
        attr_doubles mAttrMultipliers;
        attr_doubles mAttrAdditions;

    };

    class Character : public ICharacter
    {
    public:
        Character();

        virtual eGender getGender() const;
        virtual std::string getName() const { return mName; }

        // For boolean values.
        virtual double getSpellResistance(Magic::eMagicType type) const;
        virtual double getAttribute(eCharacterAttribute attr) const;
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
        // Shortcuts to class data
        BattleMenu * getBattleMenu() const;
        std::string getClassName() const;
        // Equipment
        void equip(Equipment::eSlot slot, Equipment *pEquip);
        void unequip(Equipment::eSlot);

    private:
        typedef std::multimap<std::string,StatusEffect*> StatusEffectMap;
        AttributeFile mAttributes;
        std::string mName;
        CharacterClass * mpClass;
        StatusEffectMap mStatusEffects;
    };

};
#endif




