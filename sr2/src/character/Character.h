#ifndef SR_CHARACTER_H
#define SR_CHARACTER_H

#include <ClanLib/core.h>
#include "sr_defines.h"
#include "Equipment.h"
#include "Magic.h"
#include "SpriteRef.h"

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

    class ICharacter
    {
    public:
        enum eGender { NEUTER = 0, MALE=0x1, FEMALE=0x2, HERMAPHRODITE = MALE | FEMALE };
        enum eType   { NONLIVING, LIVING, MAGICAL }; 
        enum eCharacterAttribute
        {
            CA_INVALID,
            _START_OF_TRANSIENTS,
            CA_HP,
            CA_MP,            
            _START_OF_INTS,
            CA_STR,              // Part of determining dmg of physical attack. 
            CA_DEF,              // Physical defense
            CA_DEX,              // Chances of a hit connecting (0-1)
            CA_EVD,              // Chances of evading an attack(0-1)
            CA_MAG,              // Magic power
            CA_RST,              // Magic resistance
            CA_LCK,              // Similar to initiative. Also helps in other aspects of the game...
            _START_OF_REALS,
            CA_JOY,              // Increases experience gained (Multiplier)
            _START_OF_TOGGLES,
            CA_DRAW_ILL,
            CA_DRAW_STONE,
            CA_DRAW_BERSERK,
            CA_DRAW_WEAK,
            CA_DRAW_PARALYZED,
            CA_DRAW_TRANSLUCENT,
            CA_DRAW_MINI,
            CA_CAN_ACT,
            CA_CAN_FIGHT,
            CA_CAN_CAST,
            CA_CAN_SKILL,
            CA_CAN_ITEM,
            CA_CAN_RUN,
            CA_ALIVE,
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
        
        virtual eGender GetGender() const=0;
        virtual eType GetType() const=0;
        virtual std::string GetName() const=0;
        //   virtual ICharacterGroup* GetGroup() const=0;
        virtual uint GetLevel()const=0;
        virtual void SetLevel(uint level)=0;

        virtual double GetAttributeReal(eCharacterAttribute attr) const = 0;
        virtual int    GetAttribute    (eCharacterAttribute attr) const = 0;
        virtual double GetSpellResistance(Magic::eMagicType type) const = 0;
        virtual bool GetToggle(eCharacterAttribute attr) const = 0;
        virtual void SetToggle(eCharacterAttribute attr, bool toggle) = 0;
        virtual void AddStatusEffect(StatusEffect *)=0;
        virtual void RemoveEffects(const std::string &name)=0;
        virtual void StatusEffectRound()=0;
        virtual void PermanentAugment(eCharacterAttribute attr, int augment)=0;
        virtual void PermanentAugment(eCharacterAttribute attr, double augment)=0;
        virtual void RollInitiative(void)=0;
        virtual uint GetInitiative(void)const=0;
        virtual void Kill()=0;
 
        // Static API
        static eCharacterAttribute CharAttributeFromString(const std::string &str); 
        static eCommonAttribute CommonAttributeFromString(const std::string &str); 
        static uint CAFromString(const std::string &str);
        static std::string CAToString(uint);
        static bool IsInteger(eCharacterAttribute attr);
        static bool IsReal(eCharacterAttribute attr);
        static bool IsToggle(eCharacterAttribute attr);
        static bool IsTransient(eCharacterAttribute attr);
        static uint GetMaximumAttribute(eCharacterAttribute attr);
    
        ///@todo API for different battle animations TBD
    private:
    };


    class ICharacterGroup
    {
    public:
        virtual uint GetCharacterCount() const = 0;
        virtual ICharacter * GetCharacter(uint index) const = 0;
    private:
    };

    class Character : public ICharacter, public Element
    {
    public:
        Character();

        virtual eGender GetGender() const;
        virtual std::string GetName() const { return m_name; }
        virtual eType GetType() const { return m_eType; }
        //  virtual ICharacterGroup * GetGroup() const;
        virtual uint GetLevel(void)const;
        virtual void SetLevel(uint);
        virtual double GetSpellResistance(Magic::eMagicType type) const;
        virtual double GetAttributeReal(eCharacterAttribute attr) const;
        virtual int  GetAttribute(eCharacterAttribute attr) const;
        virtual bool GetToggle(eCharacterAttribute attr) const;
        virtual void SetToggle(eCharacterAttribute attr, bool state);
        virtual void PermanentAugment(eCharacterAttribute attr, int augment);
        virtual void PermanentAugment(eCharacterAttribute attr, double augment);
        virtual void Kill();
        virtual void AddStatusEffect(StatusEffect *);
        virtual void RemoveEffects(const std::string &name);
        virtual void StatusEffectRound();
        virtual void RollInitiative(void);
        virtual uint GetInitiative(void)const;

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

        // Element API
        virtual eElement WhichElement() const { return ECHARACTER; }
    private:
        typedef std::multimap<std::string,StatusEffect*> StatusEffectMap;
        typedef std::map<std::string,SpriteDefinition*> SpriteDefinitionMap;
        virtual bool handle_element(eElement, Element * );
        virtual void load_attributes(CL_DomNamedNodeMap *);
        virtual void load_finished();

        std::string m_name;
        std::map<eCharacterAttribute,double> m_real_augments;
        std::map<eCharacterAttribute,int> m_augments;
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
        int init = static_cast<int>(random_distribution(GetAttribute(CA_LCK),0.2));
        m_nInitiative = max(0,init);
    }

    inline uint Character::GetInitiative(void)const
    {
        return m_nInitiative;
    }

};
#endif




