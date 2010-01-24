#ifndef SR_NAMED_ITEM_H
#define SR_NAMED_ITEM_H

#include "Item.h"
#include "Element.h"

namespace StoneRing{
    class NamedItem : public virtual Item, public Element
    {
    public:
        NamedItem();
        virtual ~NamedItem();

        std::string GetIconRef() const;

        virtual std::string GetName() const;
        virtual uint GetMaxInventory() const ;
        virtual eDropRarity GetDropRarity() const;


        void SetIconRef(const std::string &ref);
        void SetName ( const std::string &name );
        void SetMaxInventory ( uint max );
        void SetDropRarity( Item::eDropRarity rarity );

        virtual bool operator== ( const ItemRef &ref );

    private:
        std::string m_name;
        std::string m_icon_ref;
        uint m_nMaxInventory;
        Item::eDropRarity m_eDropRarity;
    };

    class NamedItemElement : public Element
    {
    public:
        NamedItemElement();
        ~NamedItemElement();

        virtual eElement WhichElement() const{ return ENAMEDITEMELEMENT; }

        NamedItem * GetNamedItem() const;

        std::string GetIconRef() const;

        uint GetMaxInventory() const;
        Item::eDropRarity GetDropRarity() const;
        std::string GetName() const;


    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap attributes) ;
        virtual void load_finished();
        NamedItem * m_pNamedItem;
        std::string m_name;
        std::string m_icon_ref;
        Item::eDropRarity m_eDropRarity;
        uint m_nMaxInventory;
    };

};
#endif



