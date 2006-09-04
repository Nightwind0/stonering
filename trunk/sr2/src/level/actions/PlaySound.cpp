#include "PlaySound.h"
#include "IApplication.h"


using namespace StoneRing;

PlaySound::PlaySound()
{
}

CL_DomElement  PlaySound::createDomElement(CL_DomDocument &doc) const
{
	CL_DomElement element(doc,"playSound");

	CL_DomText text(doc, mSound );

	text.set_node_value ( mSound );

	element.append_child ( text );

	return element;
}


void PlaySound::handleText(const std::string &text)
{
	mSound = text;
}

PlaySound::~PlaySound()
{
}

void PlaySound::invoke()
{
	IApplication::getInstance()->playSound ( mSound );

}