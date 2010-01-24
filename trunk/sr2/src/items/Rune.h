#ifndef SR_RUNE_H
#define SR_RUNE_H
#include "NamedItem.h"

namespace StoneRing{
    class Rune : public NamedItem
    {
    public:
        Rune();
        virtual ~Rune();

        virtual eElement WhichElement() const{ return ERUNE; }
        virtual uint GetValue() const ;
        virtual uint GetSellValue() const ;

        // We're overriding whatever was specified in the XML. Never drop a rune unless specified by the monster
        virtual eDropRarity GetDropRarity() const { return NEVER; }
        virtual eItemType GetItemType() const { return RUNE ; }

        SpellRef * GetSpellRef() const;
    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap * attributes) ;
        virtual void load_finished();
        SpellRef *m_pSpellRef;
    };
};

#endif



