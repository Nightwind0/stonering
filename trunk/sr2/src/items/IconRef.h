#include "Element.h"

namespace StoneRing{
    class IconRef : public Element
    {
    public:
        IconRef();
        virtual ~IconRef();
        virtual eElement WhichElement() const{ return EICONREF; }

        std::string GetIcon() const;
		virtual std::string GetDebugId() const { return m_icon; }				
		
    private:
        virtual bool handle_element(eElement element, Element * pElement );
        virtual void load_attributes(CL_DomNamedNodeMap  attributes) ;
        virtual void handle_text(const std::string &text);
        std::string m_icon;
    };


};



