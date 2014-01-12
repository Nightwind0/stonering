#ifndef SR_WEAPON_H
#define SR_WEAPON_H

#include "Equipment.h"
#include "DamageCategory.h"

namespace StoneRing{
    
	class Animation;
    
    class Weapon : public Equipment
    {
    public:
        Weapon();
        virtual ~Weapon();
        virtual WeaponType * GetWeaponType() const = 0;
        virtual bool IsRanged() const = 0;
        virtual bool IsTwoHanded() const = 0;
        
        virtual eSlot GetSlot() const;

        virtual bool IsArmor() const { return false; }
        // TODO Plumb the weapon type animation through here, so that UniqueWeapons can have their own animations
        /*
        virtual void Equip(ICharacter *);
        virtual void Unequip(ICharacter *);
        */
#if 1 // Disabling weapon attribute, why not just have them modify the characters attributes?
        enum eAttribute
        {
            _FIRST_ATTR,
            ATTACK,
            HIT,
            CRITICAL,
            _LAST_ATTR
        };

        enum eScriptMode{
            ATTACK_BEFORE = 1,
            ATTACK_AFTER = 2,
            FORGO_ATTACK = 4,
            WORLD_ONLY = 8
        };


        static eAttribute AttributeForString(const std::string& str);
        static eScriptMode ScriptModeForString(const std::string& str);
        static std::string StringForAttribute(eAttribute attr);
		static bool       AttributeIsInteger(eAttribute attr);
        
        virtual void Invoke(eScriptMode invokeTime, const Steel::ParameterList& params)=0;
        bool ForgoAttack() const;
		
		virtual Animation* GetAnimation() const;
		
		virtual DamageCategory::eDamageCategory GetDamageCategory() const;
		
		
		virtual clan::Sprite GetSprite() const=0;
        

        /*
        * These are based on the weapon type, and any enhancers added by the class
        * or the unique enhancements.
        * example, if the WeaponType's baseAttack is 100, and this particular weapon
        * has a 1.5 multiplier and +7 add, this will return 157
        */
        double GetWeaponAttribute ( eAttribute attr );

        bool ScriptModeApplies(eScriptMode mode) const { return m_eScriptMode & mode; }

#endif
        static std::string CreateWeaponName(WeaponType *pType, WeaponClass *pClass,
            WeaponClass* pImbuement, RuneType *pRune);

        // Getters for weapon enhancers. need 'em.
    protected:
        void Clear_Weapon_Enhancers();
        void Add_Weapon_Enhancer (WeaponEnhancer * pEnhancer);
        void Add_Script_Mode(eScriptMode script_mode);
		void SetAnimation(Animation* anim);
		void SetDamageCategory(DamageCategory::eDamageCategory dmgCategory);
    private:
		Animation* m_animation;
		DamageCategory::eDamageCategory m_dmgCategory;
		bool m_bHasDmgCategory;
        uint m_eScriptMode;
        std::list<WeaponEnhancer*> m_weapon_enhancers;
    };

    inline void Weapon::Add_Script_Mode(eScriptMode mode){
        m_eScriptMode |= mode;
    }

}

#endif



