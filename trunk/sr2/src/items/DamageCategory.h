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

        virtual eClass getClass() const=0;
    private:
    };

    class WeaponDamageCategory : public Element, public DamageCategory
    {
    public:
        WeaponDamageCategory();
        WeaponDamageCategory(CL_DomElement *pElement);
        virtual ~WeaponDamageCategory();
        virtual eElement whichElement() const{ return EWEAPONDAMAGECATEGORY; }  
        virtual eClass getClass() const { return WEAPON; }

        enum eType { SLASH, BASH, JAB };

        eType getType() const;
    private:
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
        eType TypeFromString( const std::string &str );
        eType meType;
    };

    class MagicDamageCategory : public Element, public DamageCategory
    {
    public:
        MagicDamageCategory();
        MagicDamageCategory(CL_DomElement *pElement);
        virtual eElement whichElement() const{ return EMAGICDAMAGECATEGORY; }   
        virtual ~MagicDamageCategory();
        virtual eClass getClass() const { return MAGIC; }
        Magic::eMagicType getType() const;
    private:
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
        Magic::eMagicType meType;
    };
};

#endif



