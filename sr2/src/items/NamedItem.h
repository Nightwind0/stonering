#ifndef SR_NAMED_ITEM_H
#define SR_NAMED_ITEM_H

#include "Item.h"
#include "Element.h"

namespace StoneRing{
    class NamedItemElement :  public Element
    {
    public:
        NamedItemElement();
        virtual ~NamedItemElement();

        CL_Image GetIcon() const;

        virtual std::string GetName() const;
        virtual uint GetMaxInventory() const ;
        virtual Item::eDropRarity GetDropRarity() const;

    protected:
	virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap attributes) ;
        virtual void load_finished();
        void SetIconRef(const std::string &ref);
        void SetName ( const std::string &name );
        void SetMaxInventory ( uint max );
        void SetDropRarity( Item::eDropRarity rarity );
	
    private:
        std::string m_name;
	CL_Image m_icon;
        uint m_nMaxInventory;
        Item::eDropRarity m_eDropRarity;
    };

};
#endif



