#ifndef SR_SPELL_H
#define SR_SPELL_H

#include "Element.h"
#include "ScriptElement.h"
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
        eMagicType getType() const;
        bool resistAll() const;
        bool resistElemental() const;
        CL_DomElement  createDomElement(CL_DomDocument &doc) const { return CL_DomElement(doc,"magicResistance"); }

    private:
        virtual void loadAttributes(CL_DomNamedNodeMap *);
        eMagicType meType;
        bool mbResistAll;
        bool mbResistElemental;
        float mfResistance;
    };


    class Spell : public Element
    {
    public:
        Spell();
        virtual ~Spell();

        virtual eElement whichElement() const{ return ESPELL; }
        std::string getName() const;

        enum eType { ELEMENTAL, WHITE, STATUS, OTHER };
        enum eUse { BATTLE, WORLD, BOTH };
        enum eTargetable { ALL, SINGLE, EITHER, SELF_ONLY };
        eType getType() const;
        eUse getUse() const;
        eTargetable getTargetable() const;

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

        eType getTypeFromString(const std::string &str);
        eUse getUseFromString ( const std::string &str);
        eTargetable getTargetableFromString ( const std::string &str);

        eType meType;
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




/*
  class WeaponDamageCategory : public Element
  {
  public:
  WeaponDamageCategory();
  WeaponDamageCategory();
  virtual ~WeaponDamageCategory();
    
  enum eType { SLASH, BASH, JAB };
  eType getType() const;

  CL_DomElement createDomElement( CL_DomDocument &doc ) const;
    
  private:
  eType meType;
  };

  class MagicDamageCategory : public Element
  {
  public:
  MagicDamageCategory();
  MagicDamageCategory(CL_DomElement * pElement);
  virtual ~MagicDamageCategory();
    
  enum eType { FIRE, WATER, EARTH, WIND, HOLY, OTHER };
  eType getType() const;

  CL_DomElement createDomElement( CL_DomDocument &doc ) const;
  private:
  eType meType;
  };


*/

};


#endif




