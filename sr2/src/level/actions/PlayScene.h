#ifndef SR_PLAY_SCENE_H
#define SR_PLAY_SCENE_H	

#include "Action.h"
#include "Element.h"

namespace StoneRing{
	class PlayScene : public Action, public Element
	{
	public:
		PlayScene();
		virtual ~PlayScene();
		virtual eElement whichElement() const{ return EPLAYSCENE; }	
		virtual void invoke();

		virtual CL_DomElement  createDomElement(CL_DomDocument&) const;

	protected:
		virtual void handleText(const std::string &);
		std::string mAnimation;
	};
};

#endif