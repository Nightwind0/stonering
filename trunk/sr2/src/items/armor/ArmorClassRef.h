#ifndef SR_ARMORCLASS_REF
#define SR_ARMORCLASS_REF

#include "Element.h"

namespace StoneRing{


    class ArmorClassRef : public Element
    {
    public:
        ArmorClassRef();
        virtual ~ArmorClassRef();
        virtual eElement whichElement() const{ return EARMORCLASSREF; } 
        std::string getName() const;
        void setName(const std::string &name){ mName = name; }
        bool operator==(const ArmorClassRef &lhs );
    private:
        virtual void handleText(const std::string &text);
    protected:
        std::string mName;
    };

};


#endif




