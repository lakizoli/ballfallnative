#include "pch.h"
#include "menuscene.h"
#include "fallscene.h"
#include "ballfallgame.h"

void MenuScene::Init (int width, int height) {
	BallFallGame& game = (BallFallGame&) Game::Get ();

	_background.reset (new ImageMesh ("menu_background.png"));
	_background->Init ();

	Vector2D refScale = Vector2D (width, height) / Vector2D (1440, 2392);

	Vector2D screenSize = game.ToLocal (width, height);
	_background->Pos = screenSize / 2.0f;
	_background->Scale = screenSize / _background->boundingBox.Size ();

	_start.reset (new ImageMesh ("start_new_game.png"));
	_start->Init ();

	_start->Pos = game.ToLocal (0, 235 * 4) * refScale + Vector2D (screenSize.x / 2.0f, 0);
	_start->Scale = game.ToLocal (350 * 4, 50 * 4) * refScale / _start->boundingBox.Size ();

	game.ReadGameState ();
	_lastHighScore = game.State ().highScore;

	stringstream ss;
	ss << _lastHighScore;
	string highScore = ss.str ();

	Vector2D charSize (32 * 4, 37 * 4);
	for (size_t i = 0; i < highScore.size ();++i) {
		char ch = highScore[i];

		stringstream ssNum;
		ssNum << "score" << ch << ".png";

		shared_ptr<ImageMesh> mesh (new ImageMesh (ssNum.str ()));
		mesh->Init ();

		float charX = (float)i * charSize.x - charSize.x * (float) highScore.size () / 2.0f + 15 * 4;
		mesh->Pos = game.ToLocal (charX, 390 * 4) * refScale + Vector2D (screenSize.x / 2.0f, 0);
		mesh->Scale = game.ToLocal (charSize.x, charSize.y) * refScale / mesh->boundingBox.Size ();

		_score.push_back (mesh);
	}

	_startRegion = Rect2D (game.ToLocal (15 * 4, 195 * 4) * refScale, game.ToLocal (335 * 4, 285 * 4) * refScale);
	_startPressed = false;
	_startInPressedState = false;

	_clickSoundID = 0;

	game.ContentManager ().InitAdMob ();
}

void MenuScene::Shutdown () {
	_startPressed = false;

	for (shared_ptr<ImageMesh> num : _score)
		num->Shutdown ();
	_score.clear ();

	_start->Shutdown ();
	_start.reset ();

	_background->Shutdown ();
	_background.reset ();
}

void MenuScene::Resize (int newWidth, int newHeight) {
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

	for (shared_ptr<ImageMesh> num : _score)
		num->Render ();

	//Wait click sound end and advance screen
	Game& game = Game::Get ();
	IContentManager& contentManager = Game::ContentManager ();
	if (contentManager.IsSoundEnded (_clickSoundID)) {
		contentManager.RemoveEndedSoundID (_clickSoundID);
		_clickSoundID = 0;

		game.SetCurrentScene (shared_ptr<Scene> (new FallScene ()));
		return;
	}
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
		_clickSoundID = Game::ContentManager ().PlaySound ("click", false);
	} else {
		_startPressed = false;
	}
}

void MenuScene::TouchMove (int fingerID, float x, float y) {
	Scene::TouchMove (fingerID, x, y);

	//...
}
