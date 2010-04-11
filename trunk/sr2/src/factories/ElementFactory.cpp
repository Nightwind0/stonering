#include "ElementFactory.h"
#include "Level.h"
#include "ScriptElement.h"
#include "Level.h"
#include "CharacterClass.h"
#include "IconRef.h"
#include "Item.h"
#include "ItemRef.h"
#include "NamedItem.h"
#include "WeaponTypeExclusionList.h"
#include "WeaponType.h"
#include "WeaponTypeRef.h"
#include "WeaponClassRef.h"
#include "WeaponClass.h"
#include "UniqueArmor.h"
#include "UniqueWeapon.h"
#include "RegularItem.h"
#include "SpecialItem.h"
#include "SystemItem.h"
#include "Rune.h"
#include "RuneType.h"
#include "ArmorTypeExclusionList.h"
#include "ArmorType.h"
#include "ArmorTypeRef.h"
#include "ArmorClassRef.h"
#include "ArmorRef.h"
#include "WeaponRef.h"
#include "Animation.h"
#include "BattleMenu.h"
#include "BattleMenuOption.h"
#include "MonsterElement.h"
#include "Stat.h"
#include "SpriteDefinition.h"
#include "Character.h"
#include "MonsterRegion.h"
#include "MonsterGroup.h"
#include "MonsterRef.h"
#include "AttributeModifier.h"

using namespace StoneRing;


Element * ElementFactory::createBattleSprite() const
{
    return new BattleSprite();
}

Element * ElementFactory::createDirectionBlock()const
{
    return new DirectionBlock();

}

Element * ElementFactory::createTilemap()const
{
    return new Tilemap();
}

Element * ElementFactory::createSpriteRef()const
{
    return new SpriteRef();
}


Element * ElementFactory::createMovement()const
{
    return new Movement();
}


Element * ElementFactory::createItemRef()const
{
    return new ItemRef();
}


Element * ElementFactory::createEvent()const
{
    return new Event();
}


Element * ElementFactory::createTile()const
{
    return new Tile();
}


Element * ElementFactory::createMappableObject()const
{
    return new MappableObject();
}


Element * ElementFactory::createNamedItemRef()const
{
    return new NamedItemRef();
}

Element * ElementFactory::createScriptElement()const
{
    return new ScriptElement(false);
}

Element * ElementFactory::createConditionScript()const
{
    return new ScriptElement(true);
}


Element * ElementFactory::createIconRef() const
{
    return new IconRef();
}


Element * ElementFactory::createNamedItemElement() const
{
    return new NamedItemElement();
}


Element * ElementFactory::createRegularItem()const
{
    return new RegularItem();
}

Element * ElementFactory::createWeaponTypeExclusionList() const
{
    return new WeaponTypeExclusionList;
}
Element * ElementFactory::createArmorTypeExclusionList() const
{
    return new ArmorTypeExclusionList;
}

Element * ElementFactory::createSpecialItem()const
{
    return new SpecialItem();
}


Element * ElementFactory::createSystemItem()const
{
    return new SystemItem();
}


Element * ElementFactory::createRune()const
{
    return new Rune();
}

Element * ElementFactory::createStat()const
{
    return new Stat();
}


Element * ElementFactory::createUniqueWeapon()const
{
    return new UniqueWeapon();
}


Element * ElementFactory::createUniqueArmor()const
{
    return new UniqueArmor();
}


Element * ElementFactory::createWeaponTypeRef()const
{
    return new WeaponTypeRef();
}


Element * ElementFactory::createWeaponClassRef()const
{
    return new WeaponClassRef();
}


Element * ElementFactory::createArmorTypeRef()const
{
    return new ArmorTypeRef();
}


Element * ElementFactory::createArmorClassRef()const
{
    return new ArmorClassRef();
}


Element * ElementFactory::createWeaponRef()const
{
    return new WeaponRef();
}

Element * ElementFactory::createArmorRef()const
{
    return new ArmorRef();
}


Element * ElementFactory::createRuneType()const
{
    return new RuneType();
}


Element * ElementFactory::createSpellRef()const
{
    return new SpellRef();
}


Element * ElementFactory::createWeaponEnhancer()const
{
    return new WeaponEnhancer();
}


Element * ElementFactory::createArmorEnhancer()const
{
    return new ArmorEnhancer();
}


Element * ElementFactory::createAttributeModifier()const
{
    return new AttributeModifier();
}


Element * ElementFactory::createWeaponClass()const
{
    return new WeaponClass();
}

Element * ElementFactory::createWeaponType()const
{
    return new WeaponType();
}

Element * ElementFactory::createArmorClass()const
{
    return new ArmorClass();
}

Element * ElementFactory::createArmorType()const
{
    return new ArmorType();
}


Element *ElementFactory::createStatusEffectModifier() const
{
    return new StatusEffectModifier();
}

Element * ElementFactory::createCharacterClass() const
{
    return new CharacterClass();
}

Element * ElementFactory::createCharacter() const
{
    return new Character();
}


Element * ElementFactory::createLevel() const
{
    return new Level();
}

Element * ElementFactory::createLevelHeader() const
{
    return new LevelHeader();
}

Element          * ElementFactory::createSpell() const
{
    return new Spell ();
}

Element            * ElementFactory::createAnimation() const
{
    return new Animation();
}

Element      * ElementFactory::createMagicResistance ( ) const
{
    return new MagicResistance ();
}

Element * ElementFactory::createStatusEffect() const
{
    return new StatusEffect();
}

Element * ElementFactory::createOnEquip() const
{
    return new OnEquip();
}

Element * ElementFactory::createOnUnequip() const
{
    return new OnUnequip();
}

Element * ElementFactory::createOnRound() const
{
    return new OnRound();
}

Element * ElementFactory::createOnStep()const
{
    return new OnStep();
}

Element * ElementFactory::createOnCountdown()const
{
    return new OnCountdown();
}

Element * ElementFactory::createOnInvoke()const
{
    return new OnInvoke();
}

Element * ElementFactory::createOnSelect()const
{
    return new OnSelect();
}

Element * ElementFactory::createOnDeselect()const
{
    return new OnDeselect();
}



Element * ElementFactory::createOnRemove()const
{
    return new OnRemove();
}

Element * ElementFactory::createTiles() const
{
    return new Tiles();
}

Element * ElementFactory::createMappableObjects() const
{
    return new MappableObjects();
}

Element * ElementFactory::createMonster() const
{
    return new MonsterElement();
}

Element * ElementFactory::createMonsterGroup() const
{
    return new MonsterGroup();
}

Element * ElementFactory::createMonsterRef() const
{
    return new MonsterRef();
}

Element * ElementFactory::createMonsterRegion() const
{
    return new MonsterRegion();
}

Element * ElementFactory::createMonsterRegions() const
{
    return new MonsterRegions();
}

Element * ElementFactory::createStatScript() const
{
    return new StatScript();
}

Element * ElementFactory::createPhase() const
{
    return new Phase();
}

Element * ElementFactory::createSkillRef() const
{
    return new SkillRef();
}

Element * ElementFactory::createSkill() const
{
    return new Skill();
}

Element * ElementFactory::createBattleMenu() const
{
    return new BattleMenu();
}

Element * ElementFactory::createBattleMenuOption() const
{
    return new BattleMenuOption();
}


Element * ElementFactory::createSpriteMovement() const
{
    return new SpriteMovement();
}

Element * ElementFactory::createSpriteAnimation() const
{
    return new SpriteAnimation();
}

Element * ElementFactory::createSpriteStub() const
{
    return new SpriteStub();
}

Element * ElementFactory::createSpriteDefinition() const
{
    return new SpriteDefinition();
}

Element * ElementFactory::createAlterSprite() const
{
    return new AlterSprite();
}

Element * ElementFactory::createElement( const std::string & element )
{
    MethodMap::iterator it = mCreateMethods.find(element);
    if( it == mCreateMethods.end() )
    {
        throw CL_Exception ( "Element '" + element + "' is not recognized" );
    }

    CreateMethod method = it->second;

    return (this->*method)();

}

ElementFactory::ElementFactory()
{
    registerCreateMethods();
}

void ElementFactory::registerCreateMethods()
{
    mCreateMethods["animation"] = &StoneRing::ElementFactory::createAnimation;
    mCreateMethods["spriteDefinition"] = &ElementFactory::createSpriteDefinition;
    mCreateMethods["armorClass"] = &ElementFactory::createArmorClass;
    mCreateMethods["armorClassRef"] = &ElementFactory::createArmorClassRef;
    mCreateMethods["armorEnhancer"] = &ElementFactory::createArmorEnhancer;
    mCreateMethods["armorRef"] = &ElementFactory::createArmorRef;
    mCreateMethods["armorType"] = &ElementFactory::createArmorType;
    mCreateMethods["armorTypeExclusionList"] = &ElementFactory::createArmorTypeExclusionList;
    mCreateMethods["armorTypeRef"] = &ElementFactory::createArmorTypeRef;
    mCreateMethods["attributeModifier"] = &ElementFactory::createAttributeModifier;
    mCreateMethods["battleMenu"] = &ElementFactory::createBattleMenu;
    mCreateMethods["battleMenuOption"] = &ElementFactory::createBattleMenuOption;
    mCreateMethods["battleSprite"] = &ElementFactory::createBattleSprite;
    mCreateMethods["character"] = &ElementFactory::createCharacter;
    mCreateMethods["characterClass"] = &ElementFactory::createCharacterClass;
    mCreateMethods["conditionScript"] = &ElementFactory::createConditionScript;
    mCreateMethods["directionBlock"] = &ElementFactory::createDirectionBlock;
    mCreateMethods["event"] = &ElementFactory::createEvent;
    mCreateMethods["iconRef"] = &ElementFactory::createIconRef;
    mCreateMethods["itemRef"] = &ElementFactory::createItemRef;
    mCreateMethods["level"] = &ElementFactory::createLevel;
    mCreateMethods["levelHeader"] = &ElementFactory::createLevelHeader;
    mCreateMethods["magicResistance"] = &ElementFactory::createMagicResistance;
    mCreateMethods["mappableObjects"] = &ElementFactory::createMappableObjects;
    mCreateMethods["mo"] = &ElementFactory::createMappableObject;
    mCreateMethods["movement"] = &ElementFactory::createMovement;
    mCreateMethods["monster"] = &ElementFactory::createMonster;
    mCreateMethods["monsterRegion"] = &ElementFactory::createMonsterRegion;
    mCreateMethods["monsterRegions"] = &ElementFactory::createMonsterRegions;
    mCreateMethods["monsterRef"] = &ElementFactory::createMonsterRef;
    mCreateMethods["monsterGroup"] = &ElementFactory::createMonsterGroup;
    mCreateMethods["namedItemElement"] = &ElementFactory::createNamedItemElement;
    mCreateMethods["namedItemRef"] = &ElementFactory::createNamedItemRef;
    mCreateMethods["onCountdown"] = &ElementFactory::createOnCountdown;
    mCreateMethods["onDeselect"] = &ElementFactory::createOnDeselect;
    mCreateMethods["onInvoke"] = &ElementFactory::createOnInvoke;
    mCreateMethods["onRemove"] = &ElementFactory::createOnRemove;
    mCreateMethods["onRound"] = &ElementFactory::createOnRound;
    mCreateMethods["onEquip"] = &ElementFactory::createOnEquip;
    mCreateMethods["onUnequip"] = &ElementFactory::createOnUnequip;
    mCreateMethods["onStep"] = &ElementFactory::createOnStep;
    mCreateMethods["onSelect"] = &ElementFactory::createOnSelect;
    mCreateMethods["phase"] = &ElementFactory::createPhase;
  //  mCreateMethods["playSound"] = &ElementFactory::createPlaySound;
//    mCreateMethods[Element::EPREREQSKILLREF] = &ElementFactory::createPreReqSkillRef;
    mCreateMethods["regularItem"] = &ElementFactory::createRegularItem;
    mCreateMethods["rune"] = &ElementFactory::createRune;
    mCreateMethods["runeType"] = &ElementFactory::createRuneType;
    mCreateMethods["script"] = &ElementFactory::createScriptElement;
    mCreateMethods["skill"] = &ElementFactory::createSkill;
    mCreateMethods["skillRef"] = &ElementFactory::createSkillRef;
    mCreateMethods["specialItem"] = &ElementFactory::createSpecialItem;
    mCreateMethods["spell"] = &ElementFactory::createSpell;
    mCreateMethods["spellRef"] = &ElementFactory::createSpellRef;
    mCreateMethods["spriteAnimation"] = &ElementFactory::createSpriteAnimation;
    mCreateMethods["spriteMovement"] = &ElementFactory::createSpriteMovement;
    mCreateMethods["spriteRef"] = &ElementFactory::createSpriteRef;
    mCreateMethods["spriteStub"] = &ElementFactory::createSpriteStub;
    mCreateMethods["stat"] = &ElementFactory::createStat;
    mCreateMethods["statScript"] = &ElementFactory::createStatScript;
    mCreateMethods["statusEffect"] = &ElementFactory::createStatusEffect;
    mCreateMethods["statusEffectModifier"] = &ElementFactory::createStatusEffectModifier;
    mCreateMethods["systemItem"] = &ElementFactory::createSystemItem;
    mCreateMethods["tile"] = &ElementFactory::createTile;
    mCreateMethods["tilemap"] = &ElementFactory::createTilemap;
    mCreateMethods["tiles"] = &ElementFactory::createTiles;
    mCreateMethods["uniqueArmor"] = &ElementFactory::createUniqueArmor;
    mCreateMethods["uniqueWeapon"] = &ElementFactory::createUniqueWeapon;
    mCreateMethods["weaponClass"] = &ElementFactory::createWeaponClass;
    mCreateMethods["weaponClassRef"] = &ElementFactory::createWeaponClassRef;
    mCreateMethods["weaponEnhancer"] = &ElementFactory::createWeaponEnhancer;
    mCreateMethods["weaponRef"] = &ElementFactory::createWeaponRef;
    mCreateMethods["weaponType"] = &ElementFactory::createWeaponType;
    mCreateMethods["weaponTypeExclusionList"] = &ElementFactory::createWeaponTypeExclusionList;
    mCreateMethods["weaponTypeRef"] = &ElementFactory::createWeaponTypeRef;
    mCreateMethods["alterSprite"] = &ElementFactory::createAlterSprite;
}




