#ifndef SR_REGULARITEM_H
#define SR_REGULARITEM_H

#include "NamedItem.h"
#include "ScriptElement.h"
#include "ICharacter.h"


// Concrete Named Item classes
namespace StoneRing{
    class RegularItem: public NamedItemElement, public Item
    {
    public:
        RegularItem();
        virtual ~RegularItem();

        virtual eElement WhichElement() const{ return EREGULARITEM; }
        
        virtual std::string GetName() const ;
        virtual eItemType GetItemType() const { return REGULAR_ITEM; }
        virtual uint GetMaxInventory() const;
        virtual eDropRarity GetDropRarity() const;
        virtual CL_Image GetIcon() const;
        
        
        void Invoke(const SteelType& targetArray); // Execute all actions.

        enum eUseType {BATTLE, WORLD, BOTH };
        enum eTargetable { ALL, SINGLE, GROUP, SELF_ONLY, NO_TARGET };
        enum eDefaultTarget { PARTY, MONSTERS };
        eUseType GetUseType() const;
        eTargetable GetTargetable() const;
        eDefaultTarget GetDefaultTarget() const;
        bool IsReusable() const;
	virtual uint GetValue() const ; // Price to buy, and worth when calculating drops.
        virtual uint GetSellValue() const ;
        virtual void LoadItem ( CL_DomElement * pElement );
        static eUseType UseTypeFromString ( const std::string &str );
        static eTargetable TargetableFromString ( const std::string &str );
	virtual std::string GetDescription() const { return m_description; }
        virtual bool operator == ( const ItemRef &ref );	
    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap attributes) ;
        ScriptElement *m_pScript;
        eUseType m_eUseType;
        eTargetable m_eTargetable;
        uint m_nValue;
        uint m_nSellValue;
        bool m_bReusable;
        std::string m_description;
        eDefaultTarget m_eDefaultTarget;
    };

};
#endif



