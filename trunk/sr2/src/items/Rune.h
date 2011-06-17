#ifndef SR_RUNE_H
#define SR_RUNE_H
#include "NamedItem.h"

namespace StoneRing{
    class Rune : public NamedItemElement, public Item
    {
    public:
        Rune();
        virtual ~Rune();

        virtual eElement WhichElement() const{ return ERUNE; }
        virtual uint GetValue() const ;
        virtual uint GetSellValue() const ;
        virtual std::string GetName() const { return NamedItemElement::GetName(); }
        virtual uint GetMaxInventory() const { return NamedItemElement::GetMaxInventory(); }
        virtual CL_Image GetIcon() const { return NamedItemElement::GetIcon(); }
        virtual std::string GetDescription() const { return GetName(); }

        // We're overriding whatever was specified in the XML. Never drop a rune unless specified by the monster
        virtual eDropRarity GetDropRarity() const { return NEVER; }
        virtual eItemType GetItemType() const { return RUNE ; }
	
	virtual bool operator==(const StoneRing::ItemRef&);
    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_finished();
    };
};

#endif



