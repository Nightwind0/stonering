#include "Element.h"

namespace StoneRing{
	class IconRef : public Element
	{
	public:
		IconRef();
		virtual ~IconRef();
		virtual eElement whichElement() const{ return EICONREF; }	 

		std::string getIcon() const;
		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;	

	private:
	    virtual bool handleElement(eElement element, Element * pElement );
	    virtual void loadAttributes(CL_DomNamedNodeMap * pAttributes) ;
		virtual void handleText(const std::string &text);
		std::string mIcon;
	};


};