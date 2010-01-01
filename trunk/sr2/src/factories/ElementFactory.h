#ifndef SR_ELEMENT_FACTORY_H
#define SR_ELEMENT_FACTORY_H

#include <ClanLib/core.h>
#include <map>
#include "IFactory.h"

namespace StoneRing
{

    class ElementFactory : public IFactory
    {
    public:
        ElementFactory();
        virtual ~ElementFactory(){}

        virtual Element * createElement( const std::string &element_name );
    protected:
        virtual Element * createAlterSprite() const;
        virtual Element * createAnimation() const;
        virtual Element * createArmorClass()const;
        virtual Element * createArmorClassRef()const;
        virtual Element * createArmorEnhancer()const;
        virtual Element * createArmorRef()const;
        virtual Element * createArmorType()const;
        virtual Element * createArmorTypeExclusionList() const;
        virtual Element * createArmorTypeRef()const;
        virtual Element * createAttributeModifier()const;
        virtual Element * createBattleMenu() const;
        virtual Element * createBattleMenuOption() const;
        virtual Element * createCharacterClass() const;
        virtual Element * createCharacter() const;
        virtual Element * createConditionScript() const;
        virtual Element * createDirectionBlock()const;
        virtual Element * createEvent()const;
        virtual Element * createIconRef() const;
        virtual Element * createItemRef()const;
        virtual Element * createLevel()const;
        virtual Element * createLevelHeader()const;
        virtual Element * createMagicDamageCategory() const;
        virtual Element * createMagicResistance ( ) const;
        virtual Element * createMappableObject()const;
        virtual Element * createMappableObjects()const;
        virtual Element * createMonster()const;
        virtual Element * createMonsterRef()const;
        virtual Element * createMonsterGroup()const;
        virtual Element * createMonsterRegion()const;
        virtual Element * createMonsterRegions()const;
        virtual Element * createMovement()const;
        virtual Element * createNamedItemElement() const;
        virtual Element * createNamedItemRef()const;
        virtual Element * createOnEquip() const;
        virtual Element * createOnUnequip() const;
        virtual Element * createOnRound() const;
        virtual Element * createOnStep()const;
        virtual Element * createOnCountdown()const;
        virtual Element * createOnDeselect()const;
        virtual Element * createOnInvoke()const;
        virtual Element * createOnRemove()const;
        virtual Element * createOnSelect()const;
        virtual Element * createPhase() const;
        virtual Element * createRegularItem()const;
        virtual Element * createRune()const;
        virtual Element * createRuneType()const;
        virtual Element * createScriptElement() const;
        virtual Element * createSkill() const;
        virtual Element * createSkillRef() const;
        virtual Element * createSpecialItem()const;
        virtual Element * createSpell() const;
        virtual Element * createSpellRef()const;
        virtual Element * createSpriteAnimation()const;
        virtual Element * createSpriteDefinition()const;
        virtual Element * createSpriteMovement()const;
        virtual Element * createSpriteRef()const;
        virtual Element * createSpriteStub()const;
        virtual Element * createStat() const;
        virtual Element * createStatScript() const;
        virtual Element * createStatusEffect() const;
        virtual Element * createStatusEffectModifier() const;
        virtual Element * createSystemItem()const;
        virtual Element * createTile()const;
        virtual Element * createTiles() const;
        virtual Element * createTilemap()const;
        virtual Element * createUniqueArmor()const;
        virtual Element * createUniqueWeapon()const;
        virtual Element * createWeaponClass()const;
        virtual Element * createWeaponClassRef()const;
        virtual Element * createWeaponDamageCategory() const;
        virtual Element * createWeaponEnhancer()const;
        virtual Element * createWeaponRef()const;
        virtual Element * createWeaponType()const;
        virtual Element * createWeaponTypeExclusionList() const;
        virtual Element * createWeaponTypeRef()const;
    private:

        typedef Element * (ElementFactory::* CreateMethod)() const;
        typedef std::map<std::string,CreateMethod> MethodMap;

        void registerCreateMethods();
        MethodMap mCreateMethods;

    };
}

#endif



