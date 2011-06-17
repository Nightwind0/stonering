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
        virtual std::string GetDescription() const;

        ArmorType * GetArmorType() const ;
        ArmorClass * GetArmorClass() const { return m_pClass; }
        ArmorClass * GetImbuement() const { return m_pImbuement; }

        virtual void Invoke(const ParameterList& params);
        virtual bool EquipCondition(const ParameterList& params);

        virtual bool operator== ( const ItemRef &ref );

        void generate( ArmorType * pType, ArmorClass * pClass,
                       ArmorClass *pImbuement = NULL, RuneType *pRune = NULL);

    private:
        virtual void OnEquipScript(const ParameterList& params);
        virtual void OnUnequipScript(const ParameterList& params);
        std::string m_name;
        ArmorType  *m_pType;
        ArmorClass  *m_pClass;
        ArmorClass *m_pImbuement;
    };

};
#endif



