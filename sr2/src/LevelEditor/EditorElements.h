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


namespace Editor {

class WriteableElement 
{
public:
    virtual CL_DomElement createDomElement(CL_DomDocument &doc)=0;
private:
};

class Animation : public StoneRing::Animation, public WriteableElement
{
public:
    Animation();
    virtual ~Animation();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
private:
};

class AnimationDefinition : public StoneRing::AnimationDefinition 
{
public:
    AnimationDefinition();
    virtual ~AnimationDefinition();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
private:
};

class AnimationSpriteRef : public StoneRing::AnimationSpriteRef
{
 public:
	AnimationSpriteRef();
	virtual ~AnimationSpriteRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};


class ArmorClass : public StoneRing::ArmorClass
{
 public:
	ArmorClass();
	virtual ~ArmorClass();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};

class ArmorClassRef : public StoneRing::ArmorClassRef
{
 public:
	ArmorClassRef();
	virtual ~ArmorClassRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class ArmorEnhancer : public StoneRing::ArmorEnhancer
{
 public:
	ArmorEnhancer();
	virtual ~ArmorEnhancer();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class ArmorRef : public StoneRing::ArmorRef
{
 public:
	ArmorRef();
	virtual ~ArmorRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class ArmorType : public StoneRing::ArmorType
{
 public:
	ArmorType();
	virtual ~ArmorType();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class ArmorTypeExclusionList : public StoneRing::ArmorTypeExclusionList
{
 public:
	ArmorTypeExclusionList();
	virtual ~ArmorTypeExclusionList();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class ArmorTypeRef : public StoneRing::ArmorTypeRef
{
 public:
	ArmorTypeRef();
	virtual ~ArmorTypeRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};


class AttributeEnhancer : public StoneRing::AttributeEnhancer
{
 public:
	AttributeEnhancer();
	virtual ~AttributeEnhancer();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class BattleMenu : public StoneRing::BattleMenu
{
 public:
	BattleMenu();
	virtual ~BattleMenu();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class BattleMenuOption : public StoneRing::BattleMenuOption
{
 public:
	BattleMenuOption();
	virtual ~BattleMenuOption();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class CharacterClass : public StoneRing::CharacterClass
{
 public:
	CharacterClass();
	virtual ~CharacterClass();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class CharacterDefinition : public StoneRing::CharacterDefinition
{
 public:
	CharacterDefinition();
	virtual ~CharacterDefinition();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class ConditionScript : public StoneRing::ScriptElement
{
 public:
	ConditionScript();
	virtual ~ConditionScript();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class DirectionBlock : public StoneRing::DirectionBlock
{
 public:
	DirectionBlock();
    DirectionBlock(int);
	virtual ~DirectionBlock();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class Event : public StoneRing::Event
{
 public:
	Event();
	virtual ~Event();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
    void setScript(StoneRing::ScriptElement *pScript){ mpScript = pScript; }
 private:
};
class IconRef : public StoneRing::IconRef
{
 public:
	IconRef();
	virtual ~IconRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class ItemRef : public StoneRing::ItemRef
{
 public:
	ItemRef();
	virtual ~ItemRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class Level : public StoneRing::Level
{
 public:
	Level();
	virtual ~Level();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
    virtual void drawMappableObjects(const CL_Rect &src, const CL_Rect &dst, CL_GraphicContext *pGC);
    std::list<StoneRing::Tile*> getTilesAt(uint levelX, uint levelY) const;
    // Operates on ALL tiles at a location. For finer control, one must operate on the tiles individually.
    // bOn of true turns the direction block on for the specified direction,
    // false will turn it off.
    void setDirectionBlockAt(uint levelX, uint levelY, StoneRing::eDirectionBlock dir, bool bOn);

    void setHotAt(uint levelX, uint levelY, bool bHot);
        
    void addTile ( StoneRing::Tile * pTile );
    void removeTile ( StoneRing::Tile * pTile );

    void addRows(int rows);
    void addColumns(int columns);

    void setName(const std::string &name);
    void setMusic(const std::string &music);
 private:
};
class LevelHeader : public StoneRing::LevelHeader
{
 public:
	LevelHeader();
	virtual ~LevelHeader();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class MagicDamageCategory : public StoneRing::MagicDamageCategory
{
 public:
	MagicDamageCategory();
	virtual ~MagicDamageCategory();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class MagicResistance  : public StoneRing::MagicResistance 
{
 public:
	MagicResistance();
	virtual ~MagicResistance();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class MappableObject : public StoneRing::MappableObject
{
 public:
	MappableObject();
	virtual ~MappableObject();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class MappableObjects : public StoneRing::MappableObjects
{
 public:
	MappableObjects();
	virtual ~MappableObjects();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class Movement : public StoneRing::Movement
{
 public:
	Movement();
	virtual ~Movement();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class NamedItemElement : public StoneRing::NamedItemElement
{
 public:
	NamedItemElement();
	virtual ~NamedItemElement();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class NamedItemRef : public StoneRing::NamedItemRef
{
 public:
	NamedItemRef();
	virtual ~NamedItemRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class OnEquip : public StoneRing::OnEquip
{
 public:
	OnEquip();
	virtual ~OnEquip();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class OnUnequip : public StoneRing::OnUnequip
{
 public:
	OnUnequip();
	virtual ~OnUnequip();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class OnRound : public StoneRing::OnRound
{
 public:
	OnRound();
	virtual ~OnRound();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class OnStep : public StoneRing::OnStep
{
 public:
	OnStep();
	virtual ~OnStep();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class OnCountdown : public StoneRing::OnCountdown
{
 public:
	OnCountdown();
	virtual ~OnCountdown();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class OnInvoke : public StoneRing::OnInvoke
{
 public:
	OnInvoke();
	virtual ~OnInvoke();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class OnRemove : public StoneRing::OnRemove
{
 public:
	OnRemove();
	virtual ~OnRemove();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class Par : public StoneRing::Par
{
 public:
	Par();
	virtual ~Par();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class RegularItem : public StoneRing::RegularItem
{
 public:
	RegularItem();
	virtual ~RegularItem();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class Rune : public StoneRing::Rune
{
 public:
	Rune();
	virtual ~Rune();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class RuneType : public StoneRing::RuneType
{
 public:
	RuneType();
	virtual ~RuneType();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class ScriptElement : public StoneRing::ScriptElement
{
 public:
	ScriptElement();
	virtual ~ScriptElement();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
    void setScript(const std::string &);

    // May throw SteelException. Please catch and present
    void parse();
private:
    virtual void handleText(const std::string &);
    std::string mScript;
};
class Skill : public StoneRing::Skill
{
 public:
	Skill();
	virtual ~Skill();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class SkillRef : public StoneRing::SkillRef
{
 public:
	SkillRef();
	virtual ~SkillRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class SpecialItem : public StoneRing::SpecialItem
{
 public:
	SpecialItem();
	virtual ~SpecialItem();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class Spell : public StoneRing::Spell
{
 public:
	Spell();
	virtual ~Spell();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class SpellRef : public StoneRing::SpellRef
{
 public:
	SpellRef();
	virtual ~SpellRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class SpriteRef : public StoneRing::SpriteRef
{
 public:
	SpriteRef();
	virtual ~SpriteRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
    void setSpriteRef( const std::string &ref);
    void setType( StoneRing::SpriteRef::eType dir);
 private:
};
class StatIncrease : public StoneRing::StatIncrease
{
 public:
	StatIncrease();
	virtual ~StatIncrease();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class StatusEffect : public StoneRing::StatusEffect
{
 public:
	StatusEffect();
	virtual ~StatusEffect();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class StatusEffectModifier : public StoneRing::StatusEffectModifier
{
 public:
	StatusEffectModifier();
	virtual ~StatusEffectModifier();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class SystemItem : public StoneRing::SystemItem
{
 public:
	SystemItem();
	virtual ~SystemItem();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class Tile : public StoneRing::Tile
{
 public:
	Tile();
	virtual ~Tile();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
    void setLevelX(int x);
    void setLevelY(int y);
    void setZOrder(int z);

    // Has to have one of these if it's new
    void setTilemap( const std::string &mapname, uint mapX, uint mapY);
    void setSpriteRef ( const std::string &spriteRef, StoneRing::SpriteRef::eType direction );
    void setIsFloater();
    void setIsHot();
    void setDirectionBlock (int dirBlock );

    void setNorthBlock(bool bOn);
    void setSouthBlock(bool bOn);
    void setEastBlock(bool bOn);
    void setWestBlock(bool bOn);
    void setNotHot();
 private:
};
class Tiles : public StoneRing::Tiles
{
 public:
	Tiles();
	virtual ~Tiles();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class Tilemap : public StoneRing::Tilemap
{
 public:
	Tilemap();
	virtual ~Tilemap();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
    void setMapName(const std::string & mapname);
    void setMapX(int x);
    void setMapY(int y);
 private:
};
class UniqueArmor : public StoneRing::UniqueArmor
{
 public:
	UniqueArmor();
	virtual ~UniqueArmor();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class UniqueWeapon : public StoneRing::UniqueWeapon
{
 public:
	UniqueWeapon();
	virtual ~UniqueWeapon();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class WeaponClass : public StoneRing::WeaponClass
{
 public:
	WeaponClass();
	virtual ~WeaponClass();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class WeaponClassRef : public StoneRing::WeaponClassRef
{
 public:
	WeaponClassRef();
	virtual ~WeaponClassRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class WeaponDamageCategory : public StoneRing::WeaponDamageCategory
{
 public:
	WeaponDamageCategory();
	virtual ~WeaponDamageCategory();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class WeaponEnhancer : public StoneRing::WeaponEnhancer
{
 public:
	WeaponEnhancer();
	virtual ~WeaponEnhancer();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class WeaponRef : public StoneRing::WeaponRef
{
 public:
	WeaponRef();
	virtual ~WeaponRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class WeaponType : public StoneRing::WeaponType
{
 public:
	WeaponType();
	virtual ~WeaponType();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class WeaponTypeExclusionList : public StoneRing::WeaponTypeExclusionList
{
 public:
	WeaponTypeExclusionList();
	virtual ~WeaponTypeExclusionList();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class WeaponTypeRef : public StoneRing::WeaponTypeRef
{
 public:
	WeaponTypeRef();
	virtual ~WeaponTypeRef();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};
class WeaponTypeSprite : public StoneRing::WeaponTypeSprite
{
 public:
	WeaponTypeSprite();
	virtual ~WeaponTypeSprite();
    virtual CL_DomElement createDomElement(CL_DomDocument &doc);
 private:
};

}
#endif
