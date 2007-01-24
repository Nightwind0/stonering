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
        
        std::string getIconRef() const;

        virtual std::string getName() const;
        virtual uint getMaxInventory() const ;
        virtual eDropRarity getDropRarity() const;

             
        void setIconRef(const std::string &ref);
        void setName ( const std::string &name );
        void setMaxInventory ( uint max );
        void setDropRarity( Item::eDropRarity rarity );

        virtual bool operator== ( const ItemRef &ref );

    private:
        std::string mName;
        std::string mIconRef;
        uint mnMaxInventory;
        Item::eDropRarity meDropRarity;
    };

    class NamedItemElement : public Element
    {
    public:
        NamedItemElement();
        ~NamedItemElement();

        virtual eElement whichElement() const{ return ENAMEDITEMELEMENT; }          
        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

        NamedItem * getNamedItem() const;

        std::string getIconRef() const;

        uint getMaxInventory() const;
        Item::eDropRarity getDropRarity() const;
        std::string getName() const;

        

    private:
        virtual bool handleElement(eElement element, Element * pElement );
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
        virtual void loadFinished();
        NamedItem * mpNamedItem;
        std::string mName;
        std::string mIconRef;
        Item::eDropRarity meDropRarity;
        uint mnMaxInventory;
    };

};
#endif

