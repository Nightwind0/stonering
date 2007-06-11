#ifndef SR_SPELLREF_H
#define SR_SPELLREF_H


#include "Element.h"  
#include "Magic.h" 


namespace StoneRing{
    class SpellRef : public Element
    {
    public:
        SpellRef();
        virtual ~SpellRef();
        virtual eElement whichElement() const{ return ESPELLREF; }  

        Magic::eMagicType getSpellType() const;

        std::string getName() const;
        bool operator==(const SpellRef &lhs);

        void setType(Magic::eMagicType type){ meSpellType = type; }
        void setName(const std::string &name){ mName = name; }
    private:
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
        virtual void handleText(const std::string &text);
    protected:
        Magic::eMagicType meSpellType;
        std::string mName;

    };
};

#endif



