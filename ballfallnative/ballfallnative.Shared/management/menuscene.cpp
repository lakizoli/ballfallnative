#include "pch.h"
#include "menuscene.h"
#include "fallscene.h"
#include "management/game.h"

void MenuScene::Init (int width, int height) {
	Game& game = Game::Get ();

	_background.reset (new ImageMesh ("menu_background.png"));
	_background->Init ();

	Vector2D screenSize = game.ToLocal (width, height);
	_background->Pos = screenSize / 2.0f;
	_background->Scale = screenSize / _background->boundingBox.Size ();

	_start.reset (new ImageMesh ("start_new_game.png"));
	_start->Init ();

	_start->Pos = game.ToLocal (0, 235 * 4) + Vector2D (screenSize.x / 2.0f, 0);
	_start->Scale = game.ToLocal (350 * 4, 50 * 4) / _start->boundingBox.Size ();

	//TODO: read score from file...
	_lastHighScore = 0;

	//TODO: high score kiirasa...

	_startRegion = Rect2D (game.ToLocal (24 * 4, 220 * 4), game.ToLocal (260 * 4, 255 * 4));
	_startPressed = false;
	_startInPressedState = false;

	game.ContentManager ().InitAdMob ();
}

void MenuScene::Shutdown () {
	_startPressed = false;

	_start->Shutdown ();
	_start.reset ();

	_background->Shutdown ();
	_background.reset ();
}

void MenuScene::Resize (int oldWidth, int oldHeight, int newWidth, int newHeight) {
	_background->Scale = _background->boundingBox.Size () / Game::Get ().ToLocal (newWidth, newHeight);
}

void MenuScene::Update (float elapsedTime) {
	Game& game = Game::Get ();

	if (_startPressed && !_startInPressedState) {
		_start->Pos += game.ToLocal (10, 10);
		_startInPressedState = true;
	} else if (!_startPressed && _startInPressedState) {
		_start->Pos -= game.ToLocal (10, 10);
		_startInPressedState = false;
	}
}

void MenuScene::Render () {
	//glClearColor (1.0f, 0.5f, 0.5f, 1.0f);
	glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	_background->Render ();
	_start->Render ();
}

void MenuScene::TouchDown (int fingerID, float x, float y) {
	Scene::TouchDown (fingerID, x, y);

	Game& game = Game::Get ();
	Vector2D pos = game.ToLocal (x, y);
	if (!_startPressed && _startRegion.Contains (pos)) {
		_startPressed = true;
	}
}

void MenuScene::TouchUp (int fingerID, float x, float y) {
	Scene::TouchUp (fingerID, x, y);

	Game& game = Game::Get ();
	Vector2D pos = game.ToLocal (x, y);
	if (_startPressed && _startRegion.Contains (pos)) {
		_startPressed = false;

		Game& game = Game::Get ();
		game.SetCurrentScene (shared_ptr<Scene> (new FallScene ()));
		return;
	} else {
		_startPressed = false;
	}
}

void MenuScene::TouchMove (int fingerID, float x, float y) {
	Scene::TouchMove (fingerID, x, y);

	//...
}
