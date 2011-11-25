
AUTOMAKE_OPTIONS = subdir-objects foreign 1.4

bin_PROGRAMS = stonering

stonering_LDADD = \
		-lsteel \
		$(CLANLIB_LIBS)

stonering_SOURCES =	src/Application.cpp \
			src/Application.h \
			src/abilities/AbilityManager.cpp \
			src/abilities/AbilityManager.h \
			src/abilities/Animation.cpp \
			src/abilities/Animation.h \
			src/abilities/Magic.cpp \
			src/abilities/Magic.h \
			src/abilities/Skill.cpp \
			src/abilities/Skill.h \
			src/abilities/StatusEffect.cpp \
			src/abilities/StatusEffect.h \
			src/battle/ActionQueue.h \
			src/battle/ActionQueue.cpp \
			src/battle/AnimationState.h \
			src/battle/AnimationState.cpp \
			src/battle/TargetingState.cpp \
			src/battle/TargetingState.h \
			src/battle/BattleConfig.h \
			src/battle/BattleConfig.cpp \
			src/character/BattleMenu.cpp \
			src/character/BattleMenuOption.cpp \
			src/character/Character.cpp \
			src/character/CharacterClass.cpp \
			src/character/CharacterManager.cpp \
			src/character/SpriteDefinition.cpp \
			src/character/BattleMenu.h \
			src/character/BattleMenuOption.h \
			src/character/Character.h \
			src/character/CharacterClass.h \
			src/character/CharacterManager.h \
			src/character/ICharacter.h \
			src/character/SpriteDefinition.h \
			src/core/AppUtils.cpp \
			src/core/AttributeModifier.cpp \
			src/core/Element.cpp \
			src/core/GraphicsManager.cpp \
			src/core/NamedScript.cpp \
			src/core/SpriteRef.cpp \
			src/core/Utilities.cpp \
			src/core/UtilityScripts.cpp \
			src/core/UtilityScripts.h \
			src/core/AppUtils.h \
			src/core/AttributeModifier.h \
			src/core/Element.h \
			src/core/GraphicsManager.h \
			src/core/IApplication.h \
			src/core/NamedScript.h \
			src/core/Menu.h \
			src/core/Menu.cpp \
			src/core/SpriteRef.h \
			src/core/sr_defines.h \
			src/factories/ElementFactory.cpp \
			src/factories/ElementFactory.h \
			src/factories/IFactory.h \
			src/items/DamageCategory.cpp \
			src/items/Equipment.cpp \
			src/items/IconRef.cpp \
			src/items/Item.cpp \
			src/items/ItemManager.cpp \
			src/items/ItemRef.cpp \
			src/items/NamedItem.cpp \
			src/items/RegularItem.cpp \
			src/items/Rune.cpp \
			src/items/RuneType.cpp \
			src/items/SpecialItem.cpp \
			src/items/StatusEffectModifier.cpp \
			src/items/SystemItem.cpp \
			src/items/DamageCategory.h \
			src/items/Equipment.h \
			src/items/IconRef.h \
			src/items/Item.h \
			src/items/ItemManager.h \
			src/items/ItemRef.h \
			src/items/NamedItem.h \
			src/items/RegularItem.h \
			src/items/Rune.h \
			src/items/RuneType.h \
			src/items/SpecialItem.h \
			src/items/StatusEffectModifier.h \
			src/items/SystemItem.h \
			src/items/weapons/GeneratedWeapon.cpp \
			src/items/weapons/UniqueWeapon.cpp \
			src/items/weapons/Weapon.cpp \
			src/items/weapons/WeaponClass.cpp \
			src/items/weapons/WeaponClassRef.cpp \
			src/items/weapons/WeaponEnhancer.cpp \
			src/items/weapons/WeaponRef.cpp \
			src/items/weapons/WeaponType.cpp \
			src/items/weapons/WeaponTypeExclusionList.cpp \
			src/items/weapons/WeaponTypeRef.cpp \
			src/items/weapons/GeneratedWeapon.h \
			src/items/weapons/UniqueWeapon.h \
			src/items/weapons/Weapon.h \
			src/items/weapons/WeaponClass.h \
			src/items/weapons/WeaponClassRef.h \
			src/items/weapons/WeaponEnhancer.h \
			src/items/weapons/WeaponRef.h \
			src/items/armor/Armor.cpp \
			src/items/armor/ArmorClass.cpp \
			src/items/armor/ArmorClassRef.cpp \
			src/items/armor/ArmorEnhancer.cpp \
			src/items/armor/ArmorRef.cpp \
			src/items/armor/ArmorType.cpp \
			src/items/armor/ArmorTypeExclusionList.cpp \
			src/items/armor/ArmorTypeRef.cpp \
			src/items/armor/GeneratedArmor.cpp \
			src/items/armor/UniqueArmor.cpp \
			src/items/armor/Armor.h \
			src/items/armor/ArmorClass.h \
			src/items/armor/ArmorClassRef.h \
			src/items/armor/ArmorEnhancer.h \
			src/items/armor/ArmorRef.h \
			src/items/armor/ArmorType.h \
			src/items/armor/ArmorTypeExclusionList.h \
			src/items/armor/ArmorTypeRef.h \
			src/items/armor/GeneratedArmor.h \
			src/items/armor/UniqueArmor.h \
			src/items/weapons/WeaponType.h \
			src/items/weapons/WeaponTypeExclusionList.h \
			src/items/weapons/WeaponTypeRef.h \
			src/level/Event.cpp \
			src/level/Level.cpp \
			src/level/LevelElements.cpp \
			src/level/MappableObject.cpp \
			src/level/ScriptElement.cpp \
			src/level/Event.h \
			src/level/Graphic.h \
			src/level/Level.h \
			src/level/MappableObject.h \
			src/level/ScriptElement.h \
			src/level/Direction.h \
			src/level/Direction.cpp \
			src/monsters/Monster.cpp \
			src/monsters/MonsterElement.cpp \
			src/monsters/MonsterGroup.cpp \
			src/monsters/MonsterRef.cpp \
			src/monsters/MonsterRegion.cpp \
			src/monsters/Stat.cpp \
			src/monsters/Monster.h \
			src/monsters/MonsterElement.h \
			src/monsters/MonsterGroup.h \
			src/monsters/MonsterRef.h \
			src/monsters/MonsterRegion.h \
			src/monsters/Stat.h \
			src/party/Party.cpp \
			src/party/IParty.h \
			src/party/Party.h \
			src/states/BattleState.cpp \
			src/states/ChoiceState.cpp \
			src/states/MapState.cpp \
			src/states/SayState.cpp \
			src/states/BattleState.h \
			src/states/ChoiceState.h \
			src/states/MapState.h \
			src/states/SayState.h \
			src/states/State.h \
			src/states/ExperienceState.cpp \
			src/states/ExperienceState.h \
			src/states/MainMenuState.cpp \
			src/states/MainMenuState.h \
			src/core/MenuOption.h \
			src/core/MenuOption.cpp \
			src/core/Font.h \
			src/core/Font.cpp \
			src/states/ItemSelectState.h \
			src/states/ItemSelectState.cpp \
			src/states/MainMenuTargetingState.cpp \
			src/states/MainMenuTargetingState.h \
			src/states/DynamicMenuState.h \
			src/states/DynamicMenuState.cpp \
			src/states/SkillTreeState.cpp \
			src/states/SkilltreeState.h \
			src/core/Description.cpp \
			src/core/Description.h \
			src/core/MenuBox.cpp \
			src/core/MenuBox.h \
			src/states/EquipState.cpp \
			src/states/EquipState.h \
			src/core/StatusBox.h \
			src/core/StatusBox.cpp \
			src/states/ShopState.cpp \
			src/states/ShopState.h \
			src/core/SoundManager.cpp \
			src/core/SoundManager.h \
			src/abilities/SoundPlay.cpp \
			src/abilities/SoundPlay.h \
			src/level/Navigator.h \
			src/level/Navigator.cpp \
			src/states/CutSceneState.cpp \
			src/states/CutSceneState.h




stonering_CPPFLAGS = $(CPPFLAGS) \
		$(CLANLIB_CFLAGS) 

stonering_CFLAGS = $(CPPFLAGS) \
		$(CLANLIB_CFLAGS)


INCLUDES = \
		-I@top_srcdir@/src/states/ \
		-I@top_srcdir@/src/core/ \
		-I@top_srcdir@/src/level/  \
		-I@top_srcdir@/src/level/actions  \
		-I@top_srcdir@/src/level/checks  \
		-I@top_srcdir@/src/party/ \
		-I@top_srcdir@/src/factories \
		-I@top_srcdir@/src/items \
		-I@top_srcdir@/src/items/weapons \
		-I@top_srcdir@/src/items/armor \
		-I@top_srcdir@/src/character \
		-I@top_srcdir@/src/monsters \
		-I@top_srcdir@/src/abilities \
		-I@top_srcdir@/src/battle

 
