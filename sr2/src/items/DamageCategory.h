#ifndef SR_DAMAGE_CATEGORY_H
#define SR_DAMAGE_CATEGORY_H

#include "Element.h"
#include "Magic.h"

namespace StoneRing{
    class DamageCategory
    {
    public:
        DamageCategory(){}
        virtual ~DamageCategory(){}

        enum eClass { WEAPON, MAGIC };

        virtual eClass GetClass() const=0;
    private:
    };

    class WeaponDamageCategory : public Element, public DamageCategory
    {
    public:
        WeaponDamageCategory();
        WeaponDamageCategory(CL_DomElement *pElement);
        virtual ~WeaponDamageCategory();
        virtual eElement WhichElement() const{ return EWEAPONDAMAGECATEGORY; }  
        virtual eClass GetClass() const { return WEAPON; }

        enum eType { SLASH, BASH, JAB };

        eType GetType() const;
    private:
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes) ;
        static eType TypeFromString( const std::string &str );
        eType m_eType;
    };

    class MagicDamageCategory : public Element, public DamageCategory
    {
    public:
        MagicDamageCategory();
        MagicDamageCategory(CL_DomElement *pElement);
        virtual eElement WhichElement() const{ return EMAGICDAMAGECATEGORY; }   
        virtual ~MagicDamageCategory();
        virtual eClass GetClass() const { return MAGIC; }
        Magic::eMagicType GetType() const;
    private:
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes) ;
        Magic::eMagicType m_eType;
    };
};

#endif



