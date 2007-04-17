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

#include "CharacterDefinition.h"

using namespace StoneRing;

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


Element * ElementFactory::createAttributeEnhancer()const
{
    return new AttributeEnhancer();
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

Element * ElementFactory::createWeaponDamageCategory() const
{
    return new WeaponDamageCategory();
}


Element * ElementFactory::createMagicDamageCategory() const
{
    return new MagicDamageCategory();
}


Element *ElementFactory::createStatusEffectModifier() const
{
    return new StatusEffectModifier();
}

Element * ElementFactory::createCharacterClass() const
{
    return new CharacterClass();
}

Element * ElementFactory::createCharacterDefinition() const
{
    return new CharacterDefinition();
}

Element * ElementFactory::createAnimationDefinition() const
{
    return new AnimationDefinition();
}

Element * ElementFactory::createWeaponTypeSprite() const
{
    return new WeaponTypeSprite();
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


Element * ElementFactory::createStatIncrease() const
{
    return new StatIncrease();
}


Element * ElementFactory::createAnimationSpriteRef() const
{
    return new AnimationSpriteRef();
}

Element * ElementFactory::createPar() const
{
    return new Par;
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


Element * ElementFactory::createElement( const std::string & element )
{
    MethodMap::iterator it = mCreateMethods.find(element);
    if( it == mCreateMethods.end() )
    {
        throw CL_Error ( "Element '" + element + "' is not recognized" );
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
    mCreateMethods["animationDefinition"] = &ElementFactory::createAnimationDefinition;
    mCreateMethods["animationSpriteRef"] = &ElementFactory::createAnimationSpriteRef;
    mCreateMethods["armorClass"] = &ElementFactory::createArmorClass;
    mCreateMethods["armorClassRef"] = &ElementFactory::createArmorClassRef;
    mCreateMethods["armorEnhancer"] = &ElementFactory::createArmorEnhancer;
    mCreateMethods["armorRef"] = &ElementFactory::createArmorRef;
    mCreateMethods["armorType"] = &ElementFactory::createArmorType;
    mCreateMethods["armorTypeExclusionList"] = &ElementFactory::createArmorTypeExclusionList;
    mCreateMethods["armorTypeRef"] = &ElementFactory::createArmorTypeRef;
    mCreateMethods["attributeEnhancer"] = &ElementFactory::createAttributeEnhancer;
    mCreateMethods["battleMenu"] = &ElementFactory::createBattleMenu;
    mCreateMethods["battleMenuOption"] = &ElementFactory::createBattleMenuOption;
//    mCreateMethods[Element::ECHARACTER] = &ElementFactory::createCharacter;
    mCreateMethods["characterClass"] = &ElementFactory::createCharacterClass;
    mCreateMethods["conditionScript"] = &ElementFactory::createConditionScript;
    mCreateMethods["directionBlock"] = &ElementFactory::createDirectionBlock;
    mCreateMethods["event"] = &ElementFactory::createEvent;
    mCreateMethods["iconRef"] = &ElementFactory::createIconRef;
    mCreateMethods["itemRef"] = &ElementFactory::createItemRef;
    mCreateMethods["level"] = &ElementFactory::createLevel;
    mCreateMethods["levelHeader"] = &ElementFactory::createLevelHeader;
    mCreateMethods["magicDamageCategory"] = &ElementFactory::createMagicDamageCategory;
    mCreateMethods["magicResistance"] = &ElementFactory::createMagicResistance;
    mCreateMethods["mappableObjects"] = &ElementFactory::createMappableObjects;
    mCreateMethods["mo"] = &ElementFactory::createMappableObject;
    mCreateMethods["movement"] = &ElementFactory::createMovement;
    mCreateMethods["namedItemElement"] = &ElementFactory::createNamedItemElement;
    mCreateMethods["namedItemRef"] = &ElementFactory::createNamedItemRef;
    mCreateMethods["onCountdown"] = &ElementFactory::createOnCountdown;
    mCreateMethods["onInvoke"] = &ElementFactory::createOnInvoke;
    mCreateMethods["onRemove"] = &ElementFactory::createOnRemove;
    mCreateMethods["onRound"] = &ElementFactory::createOnRound;
    mCreateMethods["onEquip"] = &ElementFactory::createOnEquip;
    mCreateMethods["onUnequip"] = &ElementFactory::createOnUnequip;
    mCreateMethods["onStep"] = &ElementFactory::createOnStep;
    mCreateMethods["par"] = &ElementFactory::createPar;
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
    mCreateMethods["spriteRef"] = &ElementFactory::createSpriteRef;
    mCreateMethods["statIncrease"] = &ElementFactory::createStatIncrease;
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
    mCreateMethods["weaponDamageCategory"] = &ElementFactory::createWeaponDamageCategory;
    mCreateMethods["weaponEnhancer"] = &ElementFactory::createWeaponEnhancer;
    mCreateMethods["weaponRef"] = &ElementFactory::createWeaponRef;
    mCreateMethods["weaponType"] = &ElementFactory::createWeaponType;
    mCreateMethods["weaponTypeExclusionList"] = &ElementFactory::createWeaponTypeExclusionList;
    mCreateMethods["weaponTypeRef"] = &ElementFactory::createWeaponTypeRef;
    mCreateMethods["weaponTypeSprite"] = &ElementFactory::createWeaponTypeSprite;
}
