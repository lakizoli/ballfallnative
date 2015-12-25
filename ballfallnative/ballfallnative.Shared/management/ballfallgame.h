#pragma once

#include "management/game.h"
#include "menuscene.h"

class BallFallGame : public Game {
public:
	BallFallGame (IUtil& util, IContentManager& contentManager) : Game (util, contentManager) {}

	virtual void Init (int width, int height) override {
		Game::Init (width, height);
		SetCurrentScene (shared_ptr<Scene> (new MenuScene ()));
	}
};
