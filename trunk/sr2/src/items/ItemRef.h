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
        ItemRef(CL_DomElement *pElement );
        virtual ~ItemRef();
        virtual eElement WhichElement() const{ return EITEMREF; }
        enum eRefType { INVALID, NAMED_ITEM, WEAPON_REF, ARMOR_REF };

        std::string GetItemName() const;
        eRefType GetType() const;
        NamedItemRef * GetNamedItemRef() const;
        WeaponRef * GetWeaponRef() const;
        ArmorRef * GetArmorRef() const;

        inline Item * GetItem() const { return m_pItem; }
    protected:
        virtual bool handle_element(Element::eElement,Element *);
        virtual void load_attributes(CL_DomNamedNodeMap *pAttributes);
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
        NamedItemRef(CL_DomElement *pElement );
        virtual ~NamedItemRef();
        virtual eElement WhichElement() const{ return ENAMEDITEMREF; }
        std::string GetItemName();
    protected:
        virtual void load_attributes(CL_DomNamedNodeMap * pAttributes) ;
        std::string m_name;
    };

    bool operator<(const ItemRef &lhs,const ItemRef &rhs);

}

#endif




