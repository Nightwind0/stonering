#include "EditorElementFactory.h"
#include "EditorElements.h"

using Editor::EditorElementFactory;

Element * EditorElementFactory::createAnimation() const
{
    return new Editor::Animation();
}

Element * EditorElementFactory::createSpriteDefinition() const
{
  return new Editor::SpriteDefinition();
}

Element * EditorElementFactory::createArmorClass()const
{
  return new Editor::ArmorClass();
}

Element * EditorElementFactory::createArmorClassRef()const
{
  return new Editor::ArmorClassRef();
}

Element * EditorElementFactory::createArmorEnhancer()const
{
  return new Editor::ArmorEnhancer();
}

Element * EditorElementFactory::createArmorRef()const
{
  return new Editor::ArmorRef();
}

Element * EditorElementFactory::createArmorType()const
{
  return new Editor::ArmorType();
}

Element * EditorElementFactory::createArmorTypeExclusionList() const
{
  return new Editor::ArmorTypeExclusionList();
}

Element * EditorElementFactory::createArmorTypeRef()const
{
  return new Editor::ArmorTypeRef();
}

Element * EditorElementFactory::createAttributeEnhancer()const
{
  return new Editor::attributeModifier();
}

Element * EditorElementFactory::createBattleMenu() const
{
  return new Editor::BattleMenu();
}

Element * EditorElementFactory::createBattleMenuOption() const
{
  return new Editor::BattleMenuOption();
}

Element * EditorElementFactory::createCharacterClass() const
{
  return new Editor::CharacterClass();
}

Element * EditorElementFactory::createCharacterDefinition() const
{
  return new Editor::CharacterDefinition();
}

Element * EditorElementFactory::createConditionScript() const
{
  return new Editor::ConditionScript();
}

Element * EditorElementFactory::createDirectionBlock()const
{
  return new Editor::DirectionBlock();
}

Element * EditorElementFactory::createEvent()const
{
  return new Editor::Event();
}

Element * EditorElementFactory::createIconRef() const
{
  return new Editor::IconRef();
}

Element * EditorElementFactory::createItemRef()const
{
  return new Editor::ItemRef();
}

Element * EditorElementFactory::createLevel()const
{
  return new Editor::Level();
}

Element * EditorElementFactory::createLevelHeader()const
{
  return new Editor::LevelHeader();
}

Element * EditorElementFactory::createMagicDamageCategory() const
{
  return new Editor::MagicDamageCategory();
}

Element * EditorElementFactory::createMagicResistance ( ) const
{
  return new Editor::MagicResistance();
}

Element * EditorElementFactory::createMappableObject()const
{
  return new Editor::MappableObject();
}

Element * EditorElementFactory::createMappableObjects()const
{
  return new Editor::MappableObjects();
}

Element * EditorElementFactory::createMonster() const
{
    return new Editor::Monster();
}

Element * EditorElementFactory::createMovement()const
{
  return new Editor::Movement();
}

Element * EditorElementFactory::createNamedItemElement() const
{
  return new Editor::NamedItemElement();
}

Element * EditorElementFactory::createNamedItemRef()const
{
  return new Editor::NamedItemRef();
}

Element * EditorElementFactory::createOnEquip() const
{
  return new Editor::OnEquip();
}

Element * EditorElementFactory::createOnUnequip() const
{
  return new Editor::OnUnequip();
}

Element * EditorElementFactory::createOnRound() const
{
  return new Editor::OnRound();
}

Element * EditorElementFactory::createOnStep()const
{
  return new Editor::OnStep();
}

Element * EditorElementFactory::createOnCountdown()const
{
  return new Editor::OnCountdown();
}

Element * EditorElementFactory::createOnInvoke()const
{
  return new Editor::OnInvoke();
}

Element * EditorElementFactory::createOnRemove()const
{
  return new Editor::OnRemove();
}

Element * EditorElementFactory::createPhase() const
{
  return new Editor::Phase();
}

Element * EditorElementFactory::createRegularItem()const 
{
  return new Editor::RegularItem();
}

Element * EditorElementFactory::createRune()const
{
  return new Editor::Rune();
}

Element * EditorElementFactory::createRuneType()const
{
  return new Editor::RuneType();
}

Element * EditorElementFactory::createScriptElement() const
{
  return new Editor::ScriptElement();
}

Element * EditorElementFactory::createSkill() const
{
  return new Editor::Skill();
}

Element * EditorElementFactory::createSkillRef() const
{
  return new Editor::SkillRef();
}

Element * EditorElementFactory::createSpecialItem()const
{
  return new Editor::SpecialItem();
}

Element * EditorElementFactory::createSpell() const
{
  return new Editor::Spell();
}

Element * EditorElementFactory::createSpellRef()const
{
  return new Editor::SpellRef();
}

Element * EditorElementFactory::createSpriteRef()const
{
  return new Editor::SpriteRef();
}

Element * EditorElementFactory::createStatScript() const
{
  return new Editor::StatScript();
}

Element * EditorElementFactory::createStatusEffect() const
{
  return new Editor::StatusEffect();
}

Element * EditorElementFactory::createStatusEffectModifier() const
{
  return new Editor::StatusEffectModifier();
}

Element * EditorElementFactory::createSystemItem()const
{
  return new Editor::SystemItem();
}

Element * EditorElementFactory::createTile()const
{
  return new Editor::Tile();
}

Element * EditorElementFactory::createTiles() const
{
  return new Editor::Tiles();
}

Element * EditorElementFactory::createTilemap()const
{
  return new Editor::Tilemap();
}

Element * EditorElementFactory::createUniqueArmor()const
{
  return new Editor::UniqueArmor();
}

Element * EditorElementFactory::createUniqueWeapon()const
{
  return new Editor::UniqueWeapon();
}

Element * EditorElementFactory::createWeaponClass()const
{
  return new Editor::WeaponClass();
}

Element * EditorElementFactory::createWeaponClassRef()const
{
  return new Editor::WeaponClassRef();
}

Element * EditorElementFactory::createWeaponDamageCategory() const
{
  return new Editor::WeaponDamageCategory();
}

Element * EditorElementFactory::createWeaponEnhancer()const
{
  return new Editor::WeaponEnhancer();
}

Element * EditorElementFactory::createWeaponRef()const
{
  return new Editor::WeaponRef();
}

Element * EditorElementFactory::createWeaponType()const
{
  return new Editor::WeaponType();
}

Element * EditorElementFactory::createWeaponTypeExclusionList() const
{
  return new Editor::WeaponTypeExclusionList();
}

Element * EditorElementFactory::createWeaponTypeRef()const
{
  return new Editor::WeaponTypeRef();
}


Element * EditorElementFactory::createSpriteAnimation()const
{
    return new Editor::SpriteAnimation();
}

Element * EditorElementFactory::createSpriteStub()const
{
    return new Editor::SpriteStub();
}

Element * EditorElementFactory::createSpriteMovement() const
{
    return new Editor::SpriteMovement();
}

Element * EditorElementFactory::createAlterSprite() const
{
    return new Editor::AlterSprite();
}

