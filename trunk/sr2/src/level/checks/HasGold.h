#ifndef SR_HAS_GOLD_H
#define SR_HAS_GOLD_H   

#include "Element.h"
#include "Check.h"

namespace StoneRing{    

    class HasGold : public Check
    {
    public:
        virtual eElement whichElement() const{ return EHASGOLD; }   
        enum eOperator{LT, GT, LTE, GTE, EQ};

        HasGold();
        virtual ~HasGold();

        virtual bool evaluate();
        virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

    protected:
        virtual bool handleElement(eElement element, Element * pElement );
        virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes);
        virtual void handleText(const std::string &);
        uint mAmount;
        bool mbNot;
        eOperator meOperator;

    };

};

#endif

