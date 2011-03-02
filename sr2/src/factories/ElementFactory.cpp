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
#include "MenuOption.h"

using namespace StoneRing;


Element * ElementFactory::createScriptElement()const
{
    return new ScriptElement(false);
}

Element * ElementFactory::createConditionScript()const
{
    return new ScriptElement(true);
}



template <class T>
Element * ElementFactory::CreateElement() const
{
    return new T();
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
    mCreateMethods["animation"] = &ElementFactory::CreateElement<Animation>;
    mCreateMethods["spriteDefinition"] = &ElementFactory::CreateElement<SpriteDefinition>;
    mCreateMethods["armorClass"] = &ElementFactory::CreateElement<ArmorClass>;
    mCreateMethods["armorClassRef"] = &ElementFactory::CreateElement<ArmorClassRef>;
    mCreateMethods["armorEnhancer"] = &ElementFactory::CreateElement<ArmorEnhancer>;
    mCreateMethods["armorRef"] = &ElementFactory::CreateElement<ArmorRef>;
    mCreateMethods["armorType"] = &ElementFactory::CreateElement<ArmorType>;
    mCreateMethods["armorTypeExclusionList"] = &ElementFactory::CreateElement<ArmorTypeExclusionList>;
    mCreateMethods["armorTypeRef"] = &ElementFactory::CreateElement<ArmorTypeRef>;
    mCreateMethods["attributeModifier"] = &ElementFactory::CreateElement<AttributeModifier>;
    mCreateMethods["battleMenu"] = &ElementFactory::CreateElement<BattleMenu>;
    mCreateMethods["battleMenuOption"] = &ElementFactory::CreateElement<BattleMenuOption>;
    mCreateMethods["battleSprite"] = &ElementFactory::CreateElement<BattleSprite>;
    mCreateMethods["character"] = &ElementFactory::CreateElement<Character>;
    mCreateMethods["characterClass"] = &ElementFactory::CreateElement<CharacterClass>;
    mCreateMethods["conditionScript"] = &ElementFactory::createConditionScript;
    mCreateMethods["directionBlock"] = &ElementFactory::CreateElement<DirectionBlock>;
    mCreateMethods["event"] = &ElementFactory::CreateElement<Event>;
    mCreateMethods["iconRef"] = &ElementFactory::CreateElement<IconRef>;
    mCreateMethods["itemRef"] = &ElementFactory::CreateElement<ItemRef>;
    mCreateMethods["level"] = &ElementFactory::CreateElement<Level>;
    mCreateMethods["levelHeader"] = &ElementFactory::CreateElement<LevelHeader>;
    mCreateMethods["magicResistance"] = &ElementFactory::CreateElement<MagicResistance>;
    mCreateMethods["mappableObjects"] = &ElementFactory::CreateElement<MappableObjects>;
    mCreateMethods["mo"] = &ElementFactory::CreateElement<MappableObject>;
    mCreateMethods["movement"] = &ElementFactory::CreateElement<Movement>;
    mCreateMethods["monster"] = &ElementFactory::CreateElement<MonsterElement>;
    mCreateMethods["monsterRegion"] = &ElementFactory::CreateElement<MonsterRegion>;
    mCreateMethods["monsterRegions"] = &ElementFactory::CreateElement<MonsterRegions>;
    mCreateMethods["monsterRef"] = &ElementFactory::CreateElement<MonsterRef>;
    mCreateMethods["monsterGroup"] = &ElementFactory::CreateElement<MonsterGroup>;
    mCreateMethods["menuOption"] = &ElementFactory::CreateElement<MenuOption>;
    mCreateMethods["namedItemRef"] = &ElementFactory::CreateElement<NamedItemRef>;
    mCreateMethods["onCountdown"] = &ElementFactory::CreateElement<OnCountdown>;
    mCreateMethods["onDeselect"] = &ElementFactory::CreateElement<OnDeselect>;
    mCreateMethods["onInvoke"] = &ElementFactory::CreateElement<OnInvoke>;
    mCreateMethods["onRemove"] = &ElementFactory::CreateElement<OnRemove>;
    mCreateMethods["onRound"] = &ElementFactory::CreateElement<OnRound>;
    mCreateMethods["onEquip"] = &ElementFactory::CreateElement<OnEquip>;
    mCreateMethods["onUnequip"] = &ElementFactory::CreateElement<OnUnequip>;
    mCreateMethods["onStep"] = &ElementFactory::CreateElement<OnStep>;
    mCreateMethods["onSelect"] = &ElementFactory::CreateElement<OnSelect>;
    mCreateMethods["phase"] = &ElementFactory::CreateElement<Phase>;
  //  mCreateMethods["playSound"] = &ElementFactory::createPlaySound;
//    mCreateMethods[Element::EPREREQSKILLREF] = &ElementFactory::createPreReqSkillRef;
    mCreateMethods["regularItem"] = &ElementFactory::CreateElement<RegularItem>;
    mCreateMethods["rune"] = &ElementFactory::CreateElement<Rune>;
    mCreateMethods["runeType"] = &ElementFactory::CreateElement<RuneType>;
    mCreateMethods["script"] = &ElementFactory::createScriptElement;
    mCreateMethods["skill"] = &ElementFactory::CreateElement<Skill>;
    mCreateMethods["skillRef"] = &ElementFactory::CreateElement<SkillRef>;
    mCreateMethods["specialItem"] = &ElementFactory::CreateElement<SpecialItem>;
    mCreateMethods["spell"] = &ElementFactory::CreateElement<Spell>;
    mCreateMethods["spellRef"] = &ElementFactory::CreateElement<SpellRef>;
    mCreateMethods["spriteAnimation"] = &ElementFactory::CreateElement<SpriteAnimation>;
    mCreateMethods["spriteMovement"] = &ElementFactory::CreateElement<SpriteMovement>;
    mCreateMethods["spriteMovementScript"] = &ElementFactory::CreateElement<SpriteMovementScript>;
    mCreateMethods["spriteRef"] = &ElementFactory::CreateElement<SpriteRef>;
    mCreateMethods["spriteStub"] = &ElementFactory::CreateElement<SpriteStub>;
    mCreateMethods["stat"] = &ElementFactory::CreateElement<Stat>;
    mCreateMethods["statScript"] = &ElementFactory::CreateElement<StatScript>;
    mCreateMethods["statusEffect"] = &ElementFactory::CreateElement<StatusEffect>;
    mCreateMethods["statusEffectModifier"] = &ElementFactory::CreateElement<StatusEffectModifier>;
    mCreateMethods["systemItem"] = &ElementFactory::CreateElement<SystemItem>;
    mCreateMethods["tile"] = &ElementFactory::CreateElement<Tile>;
    mCreateMethods["tilemap"] = &ElementFactory::CreateElement<Tilemap>;
    mCreateMethods["tiles"] = &ElementFactory::CreateElement<Tiles>;
    mCreateMethods["uniqueArmor"] = &ElementFactory::CreateElement<UniqueArmor>;
    mCreateMethods["uniqueWeapon"] = &ElementFactory::CreateElement<UniqueWeapon>;
    mCreateMethods["weaponClass"] = &ElementFactory::CreateElement<WeaponClass>;
    mCreateMethods["weaponClassRef"] = &ElementFactory::CreateElement<WeaponClassRef>;
    mCreateMethods["weaponEnhancer"] = &ElementFactory::CreateElement<WeaponEnhancer>;
    mCreateMethods["weaponRef"] = &ElementFactory::CreateElement<WeaponRef>;
    mCreateMethods["weaponType"] = &ElementFactory::CreateElement<WeaponType>;
    mCreateMethods["weaponTypeExclusionList"] = &ElementFactory::CreateElement<WeaponTypeExclusionList>;
    mCreateMethods["weaponTypeRef"] = &ElementFactory::CreateElement<WeaponTypeRef>;
    mCreateMethods["alterSprite"] = &ElementFactory::CreateElement<AlterSprite>;
   // mCreateMethods["alterSprite"] = &CreateElement<AlterSprite>();
}




