#include "PlayScene.h"
#include "IApplication.h"


using namespace StoneRing;


PlayScene::PlayScene()
{

}


CL_DomElement  PlayScene::createDomElement(CL_DomDocument &doc) const
{
	CL_DomElement element(doc,"playScene");

	CL_DomText text(doc,mAnimation);

	text.set_node_value( mAnimation );

	element.append_child ( text );

	return element;
}

void PlayScene::handleText(const std::string &text)
{
	mAnimation = text;
}


PlayScene::~PlayScene()
{
}

void PlayScene::invoke()
{
	IApplication::getInstance()->playScene ( mAnimation );
}

