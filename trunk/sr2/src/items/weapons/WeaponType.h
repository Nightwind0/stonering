#ifndef SR_WEAPON_TYPE_H
#define SR_WEAPON_TYPE_H



#include "Element.h"
#include "DamageCategory.h"

namespace StoneRing{
    class WeaponType : public Element
    {
    public:
        WeaponType();
        WeaponType(CL_DomElement * pElement );
        ~WeaponType();

        virtual eElement WhichElement() const{ return EWEAPONTYPE; }    

        std::string GetName() const;
        std::string GetIconRef() const;

        uint GetBaseAttack() const;
        float GetBaseHit() const;
        float GetBaseCritical() const;
        uint GetBasePrice() const;
        bool IsRanged() const;
        bool IsTwoHanded() const;

        DamageCategory *GetDamageCategory () const { return m_pDamageCategory; }

        bool operator==(const WeaponType &lhs);

    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes) ;
        DamageCategory * m_pDamageCategory;
        std::string m_name;
        std::string m_icon_ref;
        uint m_nBasePrice;
        uint m_nBaseAttack;
        float m_fBaseHit;
        float m_fBaseCritical;
        bool m_bRanged;
        bool m_bTwoHanded;
        
    };
};

#endif



