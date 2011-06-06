#ifndef SR_SPELL_H
#define SR_SPELL_H

#include "Element.h"
#include "ScriptElement.h"
#include "Magic.h"
#include <ClanLib/core.h>
#include "DamageCategory.h"
#ifndef WIN32
#include "steel/SteelType.h"
#else
#include "SteelType.h"
#endif

namespace StoneRing
{

    class WeaponDamageCategory;
    class MagicDamageCategory;
    class SpellRef;
    class StatusEffect;

    class MagicResistance : public Element
    {
    public:
        MagicResistance();
        virtual ~MagicResistance();
        virtual eElement WhichElement() const{ return EMAGICRESISTANCE; }
        float GetResistance() const;
        Magic::eMagicType GetType() const;
        bool ResistAll() const;
        bool ResistElemental() const;
        CL_DomElement  CreateDomElement(CL_DomDocument &doc) const { return CL_DomElement(doc,"magicResistance"); }

    private:
        virtual void load_attributes(CL_DomNamedNodeMap );
        Magic::eMagicType meType;
        float m_fResistance;
    };


    class Spell : public Element, public SteelType::IHandle
    {
    public:
        Spell();
        virtual ~Spell();

        virtual eElement WhichElement() const{ return ESPELL; }
        std::string getName() const;
        enum eUse { BATTLE, WORLD, BOTH };
        enum eTargetable { ALL, SINGLE, EITHER, SELF_ONLY };
        eUse getUse() const;
        eTargetable getTargetable() const;
        Magic::eMagicType getMagicType() const;

        bool appliesToWeapons() const;
        bool appliesToArmor() const;

        uint getMP() const;

        SpellRef * createSpellRef() const;
        uint getValue() const;
        DamageCategory::eDamageCategory GetDamageCategory() const { return m_damageCategory; }
        CL_DomElement createDomElement( CL_DomDocument &doc ) const;
        MagicResistance * getMagicResistance() const;


    private:
        virtual bool handleElement(eElement, Element * );
        virtual void loadAttributes(CL_DomNamedNodeMap);

        eUse getUseFromString ( const std::string &str);
        eTargetable getTargetableFromString ( const std::string &str);

        Magic::eMagicType meType;
        DamageCategory::eDamageCategory m_damageCategory;
        eUse meUse;
        eTargetable meTargetable;
        std::string mName;
        bool mbAppliesToWeapons;
        bool mbAppliesToArmor;
        uint mnMP;
        uint mnValue;
        MagicResistance * mpMagicResistance;
        ScriptElement *mpScript;
    };
}


#endif




