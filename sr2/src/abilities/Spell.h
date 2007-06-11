#ifndef SR_SPELL_H
#define SR_SPELL_H

#include "Element.h"
#include "ScriptElement.h"
#include "Magic.h"
#include <ClanLib/core.h>

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
        virtual eElement whichElement() const{ return EMAGICRESISTANCE; }
        float getResistance() const;
        Magic::eMagicType getType() const;
        bool resistAll() const;
        bool resistElemental() const;
        CL_DomElement  createDomElement(CL_DomDocument &doc) const { return CL_DomElement(doc,"magicResistance"); }

    private:
        virtual void loadAttributes(CL_DomNamedNodeMap *);
        Magic::eMagicType meType;
        float mfResistance;
    };


    class Spell : public Element
    {
    public:
        Spell();
        virtual ~Spell();

        virtual eElement whichElement() const{ return ESPELL; }
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
        CL_DomElement createDomElement( CL_DomDocument &doc ) const;
        MagicResistance * getMagicResistance() const;

    
    private:
        virtual bool handleElement(eElement, Element * );
        virtual void loadAttributes(CL_DomNamedNodeMap *);

        eUse getUseFromString ( const std::string &str);
        eTargetable getTargetableFromString ( const std::string &str);

        Magic::eMagicType meType;
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
};


#endif




