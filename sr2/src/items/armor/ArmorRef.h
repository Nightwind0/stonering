#ifndef SR_ARMOR_REF_H
#define SR_ARMOR_REF_H

#include "Element.h"
#include "ArmorTypeRef.h"
#include "ArmorClassRef.h"
#include "ArmorType.h"
#include "ArmorClass.h"
#include "SpellRef.h"
#include "RuneType.h"

namespace StoneRing{
    class ArmorRef : public Element
    {
    public:
        ArmorRef();
        ArmorRef ( ArmorType *pType, ArmorClass *pClass, 
                   SpellRef * pSpell, RuneType *pRune );

        ~ArmorRef();
        virtual eElement WhichElement() const{ return EARMORREF; }  

        ArmorType * GetArmorType() const;
        ArmorClass * GetArmorClass() const;
        SpellRef * GetSpellRef() const;
        RuneType * GetRuneType() const;
        std::string GetName() const;
        bool operator==(const ArmorRef&);

    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_finished();
    protected:
        ArmorType * m_pArmorType;
        ArmorClass * m_pArmorClass;
        ArmorTypeRef *m_pType;
        ArmorClassRef *m_pClass;
        SpellRef * m_pSpellRef;
        RuneType * m_pRuneType;
        std::string m_name;

    };
};

#endif



