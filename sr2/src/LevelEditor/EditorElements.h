#ifndef _H_EDITOR_ELEMENTS_H
#define _H_EDITOR_ELEMENTS_H

#include "Animation.h"
#include "Armor.h"
#include "ArmorClass.h"
#include "ArmorClassRef.h"
#include "ArmorEnhancer.h"
#include "ArmorType.h"
#include "CharacterDefinition.h"
#include "CharacterClass.h"
#include "ArmorRef.h"
#include "ArmorTypeExclusionList.h"
#include "BattleMenu.h"
#include "BattleMenuOption.h"
#include "ScriptElement.h"
#include "IconRef.h"
#include "Level.h"
#include "DamageCategory.h"
#include "NamedItem.h"
#include "NamedScript.h"
#include "RegularItem.h"
#include "Rune.h"
#include "SpecialItem.h"
#include "SystemItem.h"
#include "ScriptElement.h"
#include "UniqueArmor.h"
#include "UniqueWeapon.h"
#include "WeaponClass.h"
#include "WeaponClassRef.h"
#include "WeaponRef.h"
#include "WeaponType.h"
#include "WeaponTypeExclusionList.h"
#include "WeaponTypeRef.h"
#include "SpriteRef.h"
#include "Monster.h"

#define WRITE_CHILD(element,x,doc) (element.append_child(               \
                                        dynamic_cast<WriteableElement*>(x) \
                                        ->createDomElement(doc))        \
        )

namespace Editor {

class WriteableElement 
{
public:
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const=0;
protected:
};

class Animation : public StoneRing::Animation, public WriteableElement
{
public:
    Animation();
    virtual ~Animation();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
private:
};

class SpriteDefinition : public StoneRing::SpriteDefinition, public WriteableElement
{
public:
    SpriteDefinition();
    virtual ~SpriteDefinition();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
private:
};

class SpriteAnimation : public StoneRing::SpriteAnimation, public WriteableElement
{
 public:
	SpriteAnimation();
	virtual ~SpriteAnimation();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};

class AlterSprite : public StoneRing::AlterSprite, public WriteableElement
{
 public:
	AlterSprite();
	virtual ~AlterSprite();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};


class SpriteMovement : public StoneRing::SpriteMovement, public WriteableElement
{
 public:
	SpriteMovement();
	virtual ~SpriteMovement();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};


class ArmorClass : public StoneRing::ArmorClass, public WriteableElement
{
 public:
	ArmorClass();
	virtual ~ArmorClass();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};

class ArmorClassRef : public StoneRing::ArmorClassRef, public WriteableElement
{
 public:
	ArmorClassRef();
	virtual ~ArmorClassRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class ArmorEnhancer : public StoneRing::ArmorEnhancer, public WriteableElement
{
 public:
	ArmorEnhancer();
	virtual ~ArmorEnhancer();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class ArmorRef : public StoneRing::ArmorRef, public WriteableElement
{
 public:
	ArmorRef();
	virtual ~ArmorRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc) const;
 private:
};
class ArmorType : public StoneRing::ArmorType, public WriteableElement
{
 public:
	ArmorType();
	virtual ~ArmorType();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc) const;
 private:
};
class ArmorTypeExclusionList : public StoneRing::ArmorTypeExclusionList, public WriteableElement
{
 public:
	ArmorTypeExclusionList();
	virtual ~ArmorTypeExclusionList();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class ArmorTypeRef : public StoneRing::ArmorTypeRef, public WriteableElement
{
 public:
	ArmorTypeRef();
	virtual ~ArmorTypeRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};


class attributeModifier : public StoneRing::attributeModifier, public WriteableElement
{
 public:
	attributeModifier();
	virtual ~attributeModifier();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class BattleMenu : public StoneRing::BattleMenu, public WriteableElement
{
 public:
	BattleMenu();
	virtual ~BattleMenu();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class BattleMenuOption : public StoneRing::BattleMenuOption, public WriteableElement
{
 public:
	BattleMenuOption();
	virtual ~BattleMenuOption();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class CharacterClass : public StoneRing::CharacterClass, public WriteableElement
{
 public:
	CharacterClass();
	virtual ~CharacterClass();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class CharacterDefinition : public StoneRing::CharacterDefinition, public WriteableElement
{
 public:
	CharacterDefinition();
	virtual ~CharacterDefinition();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class ConditionScript : public StoneRing::ScriptElement, public WriteableElement
{
 public:
	ConditionScript();
	virtual ~ConditionScript();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class SideBlock : public StoneRing::SideBlock, public WriteableElement
{
 public:
	SideBlock();
    SideBlock(int);
	virtual ~SideBlock();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class Event : public StoneRing::Event, public WriteableElement
{
 public:
	Event();
	virtual ~Event();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
    void setScript(StoneRing::ScriptElement *pScript){ mpScript = pScript; }
 private:
};
class IconRef : public StoneRing::IconRef, public WriteableElement
{
 public:
	IconRef();
	virtual ~IconRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class ItemRef : public StoneRing::ItemRef, public WriteableElement
{
 public:
	ItemRef();
	virtual ~ItemRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class Level : public StoneRing::Level, public WriteableElement
{
 public:
	Level();
	virtual ~Level();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
    virtual void drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC);
    std::list<StoneRing::Tile*> getTilesAt(uint levelX, uint levelY)const;
    // Operates on ALL tiles at a location. For finer control, one must operate on the tiles individually.
    // bOn of true turns the direction block on for the specified direction,
    // false will turn it off.
    void setSideBlockAt(uint levelX, uint levelY, StoneRing::eSideBlock dir, bool bOn);
    void setHotAt(uint levelX, uint levelY, bool bHot);
    void addTile ( StoneRing::Tile * pTile );
    void removeTile ( StoneRing::Tile * pTile );

    void addRows(int rows);
    void addColumns(int columns);

    void setName(const std::string &name);
    void setMusic(const std::string &music);
 private:
};
class LevelHeader : public StoneRing::LevelHeader, public WriteableElement
{
 public:
	LevelHeader();
	virtual ~LevelHeader();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class MagicDamageCategory : public StoneRing::MagicDamageCategory, public WriteableElement
{
 public:
	MagicDamageCategory();
	virtual ~MagicDamageCategory();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class MagicResistance  : public StoneRing::MagicResistance , public WriteableElement
{
 public:
	MagicResistance();
	virtual ~MagicResistance();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class MappableObject : public StoneRing::MappableObject, public WriteableElement
{
 public:
	MappableObject();
	virtual ~MappableObject();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class MappableObjects : public StoneRing::MappableObjects, public WriteableElement
{
 public:
	MappableObjects();
	virtual ~MappableObjects();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};

class Monster : public StoneRing::Monster, public WriteableElement
{
public:
    Monster();
    virtual ~Monster();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
};
class Movement : public StoneRing::Movement, public WriteableElement
{
 public:
	Movement();
	virtual ~Movement();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class NamedItemElement : public StoneRing::NamedItemElement, public WriteableElement
{
 public:
	NamedItemElement();
	virtual ~NamedItemElement();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class NamedItemRef : public StoneRing::NamedItemRef, public WriteableElement
{
 public:
	NamedItemRef();
	virtual ~NamedItemRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class OnEquip : public StoneRing::OnEquip, public WriteableElement
{
 public:
	OnEquip();
	virtual ~OnEquip();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class OnUnequip : public StoneRing::OnUnequip, public WriteableElement
{
 public:
	OnUnequip();
	virtual ~OnUnequip();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class OnRound : public StoneRing::OnRound, public WriteableElement
{
 public:
	OnRound();
	virtual ~OnRound();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class OnStep : public StoneRing::OnStep, public WriteableElement
{
 public:
	OnStep();
	virtual ~OnStep();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class OnCountdown : public StoneRing::OnCountdown, public WriteableElement
{
 public:
	OnCountdown();
	virtual ~OnCountdown();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class OnInvoke : public StoneRing::OnInvoke, public WriteableElement
{
 public:
	OnInvoke();
	virtual ~OnInvoke();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class OnRemove : public StoneRing::OnRemove, public WriteableElement
{
 public:
	OnRemove();
	virtual ~OnRemove();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class Phase : public StoneRing::Phase, public WriteableElement
{
 public:
	Phase();
	virtual ~Phase();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class RegularItem : public StoneRing::RegularItem, public WriteableElement
{
 public:
	RegularItem();
	virtual ~RegularItem();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class Rune : public StoneRing::Rune, public WriteableElement
{
 public:
	Rune();
	virtual ~Rune();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class RuneType : public StoneRing::RuneType, public WriteableElement
{
 public:
	RuneType();
	virtual ~RuneType();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class ScriptElement : public StoneRing::ScriptElement, public WriteableElement
{
 public:
	ScriptElement();
	virtual ~ScriptElement();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
    void setScript(const std::string &);

    // May throw SteelException. Please catch and present
    void parse();
private:
    virtual void handleText(const std::string &);
    std::string mScript;
};
class Skill : public StoneRing::Skill, public WriteableElement
{
 public:
	Skill();
	virtual ~Skill();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class SkillRef : public StoneRing::SkillRef, public WriteableElement
{
 public:
	SkillRef();
	virtual ~SkillRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class SpecialItem : public StoneRing::SpecialItem, public WriteableElement
{
 public:
	SpecialItem();
	virtual ~SpecialItem();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class Spell : public StoneRing::Spell, public WriteableElement
{
 public:
	Spell();
	virtual ~Spell();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class SpellRef : public StoneRing::SpellRef, public WriteableElement
{
 public:
	SpellRef();
	virtual ~SpellRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class SpriteRef : public StoneRing::SpriteRef, public WriteableElement
{
 public:
	SpriteRef();
	virtual ~SpriteRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
    void setSpriteRef( const std::string &ref);
    void setType( StoneRing::SpriteRef::eType dir);
 private:
};

class SpriteStub : public StoneRing::SpriteStub, public WriteableElement
{
public:
    SpriteStub();
    virtual ~SpriteStub();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
private:
};

class StatScript : public StoneRing::StatScript, public WriteableElement
{
 public:
	StatScript();
	virtual ~StatScript();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class StatusEffect : public StoneRing::StatusEffect, public WriteableElement
{
 public:
	StatusEffect();
	virtual ~StatusEffect();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class StatusEffectModifier : public StoneRing::StatusEffectModifier, public WriteableElement
{
 public:
	StatusEffectModifier();
	virtual ~StatusEffectModifier();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class SystemItem : public StoneRing::SystemItem, public WriteableElement
{
 public:
	SystemItem();
	virtual ~SystemItem();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class Tile : public StoneRing::Tile, public WriteableElement
{
 public:
	Tile();
	virtual ~Tile();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
    void setLevelX(int x);
    void setLevelY(int y);
    void setZOrder(int z);

    // Has to have one of these if it's new
    void setTilemap( const std::string &mapname, uint mapX, uint mapY);
    void setSpriteRef ( const std::string &spriteRef, StoneRing::SpriteRef::eType direction );
    void setIsFloater();
    void setIsHot();
    void setSideBlock (int dirBlock );

    void setNorthBlock(bool bOn);
    void setSouthBlock(bool bOn);
    void setEastBlock(bool bOn);
    void setWestBlock(bool bOn);
    void setNotHot();
 private:
};
class Tiles : public StoneRing::Tiles, public WriteableElement
{
 public:
	Tiles();
	virtual ~Tiles();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class Tilemap : public StoneRing::Tilemap, public WriteableElement
{
 public:
	Tilemap();
	virtual ~Tilemap();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
    void setMapName(const std::string & mapname);
    void setMapX(int x);
    void setMapY(int y);
 private:

};
class UniqueArmor : public StoneRing::UniqueArmor, public WriteableElement
{
 public:
	UniqueArmor();
	virtual ~UniqueArmor();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class UniqueWeapon : public StoneRing::UniqueWeapon, public WriteableElement
{
 public:
	UniqueWeapon();
	virtual ~UniqueWeapon();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class WeaponClass : public StoneRing::WeaponClass, public WriteableElement
{
 public:
	WeaponClass();
	virtual ~WeaponClass();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class WeaponClassRef : public StoneRing::WeaponClassRef, public WriteableElement
{
 public:
	WeaponClassRef();
	virtual ~WeaponClassRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class WeaponDamageCategory : public StoneRing::WeaponDamageCategory, public WriteableElement
{
 public:
	WeaponDamageCategory();
	virtual ~WeaponDamageCategory();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class WeaponEnhancer : public StoneRing::WeaponEnhancer, public WriteableElement
{
 public:
	WeaponEnhancer();
	virtual ~WeaponEnhancer();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class WeaponRef : public StoneRing::WeaponRef, public WriteableElement
{
 public:
	WeaponRef();
	virtual ~WeaponRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class WeaponType : public StoneRing::WeaponType, public WriteableElement
{
 public:
	WeaponType();
	virtual ~WeaponType();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class WeaponTypeExclusionList : public StoneRing::WeaponTypeExclusionList, public WriteableElement
{
 public:
	WeaponTypeExclusionList();
	virtual ~WeaponTypeExclusionList();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};
class WeaponTypeRef : public StoneRing::WeaponTypeRef, public WriteableElement
{
 public:
	WeaponTypeRef();
	virtual ~WeaponTypeRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)const;
 private:
};


}
#endif


