#ifndef SR_RUNE_H
#define SR_RUNE_H
#include "NamedItem.h"

namespace StoneRing{    
    class Rune : public NamedItem
    {
    public:
        Rune();
        virtual ~Rune();

        virtual eElement whichElement() const{ return ERUNE; }  
        virtual uint getValue() const ;
        virtual uint getSellValue() const ;

        // We're overriding whatever was specified in the XML. Never drop a rune unless specified by the monster
        virtual eDropRarity getDropRarity() const { return NEVER; } 
        virtual eItemType getItemType() const { return RUNE ; }

        SpellRef * getSpellRef() const;
    private:
        virtual bool handleElement(eElement element, Element * pElement );
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
        virtual void loadFinished();
        SpellRef *mpSpellRef;
    };
};

#endif



