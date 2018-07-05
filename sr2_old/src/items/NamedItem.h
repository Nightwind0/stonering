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

        clan::Image GetIcon() const;

        virtual std::string GetName() const;
        virtual uint GetMaxInventory() const ;
        virtual Item::eDropRarity GetDropRarity() const;
		virtual std::string GetDebugId() const { return m_name; }				

    protected:
	virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(clan::DomNamedNodeMap attributes) ;
        virtual void load_finished();
        void SetIconRef(const std::string &ref);
        void SetName ( const std::string &name );
        void SetMaxInventory ( uint max );
        void SetDropRarity( Item::eDropRarity rarity );
	
    private:
        std::string m_name;
		clan::Image m_icon;
        uint m_nMaxInventory;
        Item::eDropRarity m_eDropRarity;
    };

};
#endif



