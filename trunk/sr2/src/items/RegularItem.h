#ifndef SR_REGULARITEM_H
#define SR_REGULARITEM_H  

#include "NamedItem.h"
#include "ScriptElement.h"

// Concrete Named Item classes
namespace StoneRing{
    class RegularItem: public NamedItem
    {
    public:
        RegularItem();
        virtual ~RegularItem();

        virtual eElement WhichElement() const{ return EREGULARITEM; }    
        void Invoke(); // Execute all actions.

        enum eUseType {BATTLE, WORLD, BOTH };
        enum eTargetable { ALL, SINGLE, EITHER, SELF_ONLY };
        enum eDefaultTarget { PARTY, MONSTERS };
        eUseType GetUseType() const;
        eTargetable GetTargetable() const;
        eDefaultTarget GetDefaultTarget() const;
        bool IsReusable() const;
        
        virtual eItemType GetItemType() const { return REGULAR_ITEM; }

        virtual uint GetValue() const ; // Price to buy, and worth when calculating drops.
        virtual uint GetSellValue() const ;
        virtual void LoadItem ( CL_DomElement * pElement );
        static eUseType UseTypeFromString ( const std::string &str );
        static eTargetable TargetableFromString ( const std::string &str );

    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes) ;
        ScriptElement *m_pScript;
        eUseType m_eUseType;
        eTargetable m_eTargetable;
        uint m_nValue;
        uint m_nSellValue;
        bool m_bReusable;
        eDefaultTarget m_eDefaultTarget;
    };
};
#endif



