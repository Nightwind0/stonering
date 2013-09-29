#ifndef SR_RUNETYPE_H
#define SR_RUNETYPE_H

#include "Element.h"

namespace StoneRing{

    class RuneType : public Element
    {
    public:
        RuneType();
        virtual ~RuneType();
        virtual eElement WhichElement() const{ return ERUNETYPE; }

        enum eRuneType { NONE, RUNE, ULTRA_RUNE };

        eRuneType GetRuneType() const;
        std::string GetRuneTypeAsString() const;
        void SetRuneType ( eRuneType type) { m_eRuneType = type; }
        bool operator==(const RuneType &lhs);
		virtual std::string GetDebugId() const { return IntToString(m_eRuneType); }				
		
    private:
        virtual void load_attributes(CL_DomNamedNodeMap attributes) ;
    protected:
        eRuneType m_eRuneType;
    };
};

#endif



