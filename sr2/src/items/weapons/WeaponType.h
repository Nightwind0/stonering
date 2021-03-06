#ifndef SR_WEAPON_TYPE_H
#define SR_WEAPON_TYPE_H



#include "Element.h"
#include "DamageCategory.h"
#include "ScriptElement.h"

using Steel::SteelType;
namespace clan { 
  class Sprite;
}

namespace StoneRing{
    
    class WeaponType : public Element, public SteelType::IHandle
    {
    public:
        WeaponType();
        WeaponType(clan::DomElement * pElement );
        virtual ~WeaponType();

        virtual eElement WhichElement() const{ return EWEAPONTYPE; }

		clan::Sprite GetSprite() const;
        std::string GetName() const;
        std::string GetIconRef() const;

        uint GetBaseAttack() const;
        float GetBaseHit() const;
        float GetBaseCritical() const;
        uint GetBasePrice() const;
        bool IsRanged() const;
        bool IsTwoHanded() const;
		ScriptElement* GetAnimationScript() const;

        DamageCategory::eDamageCategory GetDamageCategory () const { return m_damageCategory; }

        bool operator==(const WeaponType &lhs);
		virtual std::string GetDebugId() const { return m_name; }				

    private:
		virtual void load_finished();
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(clan::DomNamedNodeMap attributes) ;
        DamageCategory::eDamageCategory m_damageCategory;
        std::string m_name;
        std::string m_icon_ref;
		clan::Sprite m_sprite;
        uint m_nBasePrice;
        uint m_nBaseAttack;
        float m_fBaseHit;
        float m_fBaseCritical;
        bool m_bRanged;
        bool m_bTwoHanded;
		ScriptElement* m_pAnimScript;
    };
};

#endif



