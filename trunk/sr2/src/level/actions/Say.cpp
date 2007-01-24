#include "Say.h"
#include "IApplication.h"

using namespace StoneRing;

Say::Say()
{
}

CL_DomElement  Say::createDomElement(CL_DomDocument &doc) const
{
    CL_DomElement element(doc,"say");

    element.set_attribute("speaker", mSpeaker );

    CL_DomText text(doc, mText );

    //    text.set_node_value( mText );

    element.append_child( text );

    return element;

}

void Say::loadAttributes(CL_DomNamedNodeMap * pAttributes)
{
    mSpeaker = getRequiredString("speaker",pAttributes);
}

void Say::handleText(const std::string &text)
{
    mText = text;
}


Say::~Say()
{
}

void Say::invoke()
{
    IApplication::getInstance()->say ( mSpeaker, mText );
}

