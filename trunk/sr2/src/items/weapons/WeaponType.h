#ifndef SR_WEAPON_TYPE_H
#define SR_WEAPON_TYPE_H



#include "Element.h"
#include "DamageCategory.h"


class CL_Sprite;

namespace StoneRing{
    
    class Animation;
    class WeaponType : public Element, public SteelType::IHandle
    {
    public:
        WeaponType();
        WeaponType(CL_DomElement * pElement );
        virtual ~WeaponType();

        virtual eElement WhichElement() const{ return EWEAPONTYPE; }

	CL_Sprite GetSprite() const;
        std::string GetName() const;
        std::string GetIconRef() const;

        uint GetBaseAttack() const;
        float GetBaseHit() const;
        float GetBaseCritical() const;
        uint GetBasePrice() const;
        bool IsRanged() const;
        bool IsTwoHanded() const;
	Animation* GetAnimation() const;

        eDamageCategory GetDamageCategory () const { return m_damageCategory; }

        bool operator==(const WeaponType &lhs);

    private:
	virtual void load_finished();
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap attributes) ;
        eDamageCategory m_damageCategory;
        std::string m_name;
        std::string m_icon_ref;
	CL_Sprite m_sprite;
        uint m_nBasePrice;
        uint m_nBaseAttack;
        float m_fBaseHit;
        float m_fBaseCritical;
        bool m_bRanged;
        bool m_bTwoHanded;
	Animation* m_pAnimation;

    };
};

#endif



