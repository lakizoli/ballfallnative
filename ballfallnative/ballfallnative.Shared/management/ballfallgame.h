#pragma once

#include "management/game.h"
#include "fallscene.h"

class BallFallGame : public Game {
	shared_ptr<FallScene> _fallScene;

public:
	BallFallGame (IUtil& util, IContentManager& contentManager) : Game (util, contentManager), _fallScene (new FallScene ()) {}

	public override void Init (int width, int height) {
		Game::Init (width, height);
		SetCurrentScene (_fallScene);
	}
};
