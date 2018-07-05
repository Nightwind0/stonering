#ifndef SR_ARMOR_H
#define SR_ARMOR_H

#include "Equipment.h"
#include "DamageCategory.h"

namespace StoneRing{

    class Armor : public Equipment
    {
    public:
        Armor();
        virtual ~Armor();

        virtual ArmorType *GetArmorType() const = 0;
        virtual eSlot GetSlot() const;

        enum eAttribute
        {
            _FIRST_ATTR,
            AC,
            RST,
            _LAST_ATTR,
        };

        virtual bool IsArmor() const { return true; }

        static std::string StringForAttribute(eAttribute attr);
        /*
        * These are based on the armor type, and any enhancers added by the class
        * or the unique enhancements.
        * example, if the ArmorType's baseResist is 100, and this particular armor
        * has a 1.5 multiplier and +7 add, this will return 157
        */
        double GetArmorAttribute ( eAttribute attr );
        
        virtual void Invoke(const Steel::ParameterList& params)=0;
  
		static bool       AttributeIsInteger( eAttribute attr );
        static eAttribute AttributeForString ( const std::string str );
        static std::string CreateArmorName(ArmorType *pType, ArmorClass *pClass,
            ArmorClass* pImbuement, RuneType *pRune);
    protected:
        void Clear_Armor_Enhancers();
        void Add_Armor_Enhancer (ArmorEnhancer * pEnhancer);
        void Set_Rune_Type(RuneType* pType);
    private:
        std::list<ArmorEnhancer*> m_armor_enhancers;
    };

}

#endif



