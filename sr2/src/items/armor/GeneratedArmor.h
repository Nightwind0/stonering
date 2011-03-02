#ifndef SR_GENERATED_ARMOR_H
#define SR_GENERATED_ARMOR_H

#include "Item.h"
#include "Armor.h"


namespace StoneRing{
    class GeneratedArmor : public virtual Item, public Armor
    {
    public:
        GeneratedArmor();
        virtual ~GeneratedArmor();

        ArmorRef GenerateArmorRef() const;

        virtual CL_Image GetIcon() const;
        virtual std::string GetName() const;
        virtual uint GetMaxInventory() const ;
        virtual eDropRarity GetDropRarity() const;
        virtual uint GetValue() const ;
        virtual uint GetSellValue() const ;
        virtual eItemType GetItemType() const { return ARMOR ; }

        ArmorType * GetArmorType() const ;
        ArmorClass * GetArmorClass() const { return m_pClass; }

        virtual void Invoke();
        virtual bool EquipCondition();

        virtual bool operator== ( const ItemRef &ref );

        void generate( ArmorType * pType, ArmorClass * pClass,
                       SpellRef *pSpell = NULL, RuneType *pRune = NULL);

    private:
        virtual void OnEquipScript();
        virtual void OnUnequipScript();
        std::string m_name;
        ArmorType  *m_pType;
        ArmorClass  *m_pClass;
    };

};
#endif



