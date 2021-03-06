#ifndef STONE_RING_ITEM_REF_H
#define STONE_RING_ITEM_REF_H

#include "Element.h"
#include <ClanLib/core.h>

//using StoneRing::Element;

namespace StoneRing
{

    class NamedItemRef;
    class WeaponRef;
    class ArmorRef;
    class Item;

    class ItemRef : public Element
    {
    public:
        ItemRef();
        ItemRef(clan::DomElement *pElement );
        virtual ~ItemRef();
        virtual eElement WhichElement() const{ return EITEMREF; }
        enum eRefType { INVALID, NAMED_ITEM, WEAPON_REF, ARMOR_REF };

        std::string GetItemName() const;
        eRefType GetType() const;
        NamedItemRef * GetNamedItemRef() const;
        WeaponRef * GetWeaponRef() const;
        ArmorRef * GetArmorRef() const;

        inline Item * GetItem() const { return m_pItem; }
		virtual std::string GetDebugId() const { return GetItemName(); }				
        
    protected:
        virtual bool handle_element(Element::eElement,Element *);
        virtual void load_attributes(clan::DomNamedNodeMap attributes);
        virtual void load_finished();

        union Ref{
            NamedItemRef * mpNamedItemRef;
            WeaponRef * mpWeaponRef;
            ArmorRef * mpArmorRef;
        };
        eRefType m_eType;
        Ref m_ref;
        Item * m_pItem;
    };



    class NamedItemRef : public Element
    {
    public:
        NamedItemRef();
        NamedItemRef(clan::DomElement *pElement );
        virtual ~NamedItemRef();
        virtual eElement WhichElement() const{ return ENAMEDITEMREF; }
        std::string GetItemName();
		virtual std::string GetDebugId() const { return m_name; }				
		
    protected:
        virtual void load_attributes(clan::DomNamedNodeMap attributes) ;
        std::string m_name;
    };

    bool operator<(const ItemRef &lhs,const ItemRef &rhs);

}

#endif




