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
        virtual eElement WhichElement() const{ return ESPELLREF; }

        Magic::eMagicType GetSpellType() const;

        std::string GetName() const;
        bool operator==(const SpellRef &lhs);

        void SetType(Magic::eMagicType type){ m_eSpellType = type; }
        void SetName(const std::string &name){ m_name = name; }
    private:
        virtual void load_attributes(CL_DomNamedNodeMap  attributes) ;
        virtual void handle_text(const std::string &text);
    protected:
        Magic::eMagicType m_eSpellType;
        std::string m_name;

    };
};

#endif



