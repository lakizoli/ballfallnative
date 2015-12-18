#pragma once

#include "management/game.h"
#include "fallscene.h"

class BallFallGame : public Game {
	shared_ptr<FallScene> _fallScene;

public:
	BallFallGame (IUtil& util, IContentManager& contentManager) : Game (util, contentManager), _fallScene (new FallScene ()) {}

	virtual void Init (int width, int height) override {
		Game::Init (width, height);
		SetCurrentScene (static_pointer_cast<Scene> (_fallScene));
	}
};
