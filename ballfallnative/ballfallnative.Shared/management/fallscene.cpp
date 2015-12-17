#include "pch.h"
#include "fallscene.h"

void FallScene::Init (int width, int height) {
	Game& game = Game::Get ();

	_state = State::PreGame;

	_levels.push_back (LevelDefinition ({ 0,  30.0f, 1000, 2000, 0.0f, 1 }));
	_levels.push_back (LevelDefinition ({ 1,  60.0f,  850, 1500, 0.5f, 1 }));
	_levels.push_back (LevelDefinition ({ 2,  90.0f,  750, 1200, 1.0f, 1 }));
	_levels.push_back (LevelDefinition ({ 3, 120.0f,  750, 1200, 1.0f, 2 }));

	_currentLevel = 0;
	_score = 0;
	_lastMins = 0;
	_lastSecs = 0;

	//_random = new Random ();
	_fullTime = 0;
	_lastAddTime = 0;

	_yellowRegion = Rect2D (game.ToLocal (0, 65 * 4), game.ToLocal (20 * 4, 265 * 4));
	_greenRegion = Rect2D (game.ToLocal (0, 265 * 4), game.ToLocal (20 * 4, 480 * 4));
	_redRegion = Rect2D (game.ToLocal (width - 20 * 4, 65 * 4), game.ToLocal (width, 265 * 4));
	_blueRegion = Rect2D (game.ToLocal (width - 20 * 4, 265 * 4), game.ToLocal (width, 480 * 4));

	_ready.reset (new QTEGrowText ("ready.png", 0.5f));
	_ready->Init ();
	_ready->Start ();

	_steady = nullptr;
	_go = nullptr;

	_background.reset (new ImageMesh ("background.png"));
	_background->Init ();

	Vector2D screenSize = game.ToLocal (width, height);
	_background->Pos = screenSize / 2.0f;
	_background->Scale = screenSize / _background->boundingBox.Size ();

	IContentManager& contentManager = Game::ContentManager ();
	contentManager.SetTopLeftStyle (20, 1, 0, 0, 1);
	contentManager.SetTopRightStyle (20, 1, 0, 0, 1);

	RefreshOverlays (true);
}

void FallScene::Shutdown () {
	_touchedBalls.clear ();

	if (_go != nullptr) {
		_go->Stop ();
		_go->Shutdown ();
	}

	if (_steady != nullptr) {
		_steady->Stop ();
		_steady->Shutdown ();
	}

	if (_ready != nullptr) {
		_ready->Stop ();
		_ready->Shutdown ();
	}

	if (_fail != nullptr) {
		_fail->Stop ();
		_fail->Shutdown ();
	}

	_background->Shutdown ();

	for (shared_ptr<QTEExplodeBall> item : _explodedBalls) {
		item->Stop ();
		item->Shutdown ();
	}
	_explodedBalls.clear ();

	for (shared_ptr<QTEGoodBall> item : _endedBalls) {
		item->Stop ();
		item->Shutdown ();
	}
	_endedBalls.clear ();

	for (shared_ptr<QTEGoodBall> item : _wrongBalls) {
		item->Stop ();
		item->Shutdown ();
	}
	_wrongBalls.clear ();

	for (shared_ptr<FallingBall> item : _fallingBalls)
		item->ball->Shutdown ();
	_fallingBalls.clear ();

	_levels.clear ();
}

void FallScene::Resize (int oldWidth, int oldHeight, int newWidth, int newHeight) {
	_background->Scale = _background->boundingBox.Size () / Game::Get ().ToLocal (newWidth, newHeight);
}

void FallScene::Update (float elapsedTime) {
	Game& game = Game::Get ();

	//Measure time
	_fullTime += elapsedTime;
	_lastAddTime += elapsedTime;

	//Handle pre game effects
	if (_state == State::PreGame) {
		if (_ready != nullptr) {
			_ready->Update (elapsedTime);
			if (_ready->IsEnded ()) {
				_fullTime = 0;
				_lastAddTime = 0;

				_ready->Shutdown ();
				_ready.reset ();

				_steady.reset (new QTEGrowText ("steady.png", 0.5f));
				_steady->Init ();
				_steady->Start ();
			}
		}

		if (_steady != nullptr) {
			_steady->Update (elapsedTime);
			if (_steady->IsEnded ()) {
				_fullTime = 0;
				_lastAddTime = 0;

				_steady->Shutdown ();
				_steady.reset ();

				_go.reset (new QTEGrowText ("go.png", 0.5f));
				_go->Init ();
				_go->Start ();
			}
		}

		if (_go != nullptr) {
			_go->Update (elapsedTime);
			if (_go->IsEnded ()) {
				_fullTime = 0;
				_lastAddTime = 0;

				_go->Shutdown ();
				_go.reset ();

				_state = State::Game;
			}
		}

		return;
	}

	//Refresh overlay texts
	RefreshOverlays (false);

	//Pick next level
	const LevelDefinition& curLevel = _levels[_currentLevel];

	if (_fullTime > curLevel.endTime && _currentLevel < (int) _levels.size () - 1) {
		_currentLevel += 1;
		RefreshOverlays (true);
	}

	const LevelDefinition& level = _levels[_currentLevel];

	//Add new ball to the system
	AddNewBalls (level);

	//Calculate physic (Remove balls not on screen)
	for (int i = (int) _fallingBalls.size () - 1; i >= 0; --i) {
		shared_ptr<FallingBall> item = _fallingBalls[i];
		item->body->Force += Vector2D (0, 1.0f * RigidBody2D::Gravity);
		item->body->Update (elapsedTime);

		if (item->body->CollideBody != nullptr) { //If this item collide with other item
			shared_ptr<Mesh2D> collideBall = item->body->CollideBody->Mesh;
			if (collideBall != nullptr && ((Ball*) collideBall.get ())->Type () == Ball::Color::Bomb) { //Handle collision with bomb
				_fallingBalls.erase (_fallingBalls.begin () + i);
				HandleBombBlowEnd (shared_ptr<Ball> ((Ball*) collideBall.get ()));
				continue;
			} else if (item->ball->Type () == Ball::Color::Bomb) { //Handle collision with bomb
				_fallingBalls.erase (_fallingBalls.begin () + i);
				HandleBombBlowEnd (item->ball);
				continue;
			}
		}

		RegionTest test = TestBallInEndRegions (item);
		switch (test) {
		default:
		case RegionTest::NotInRegion:
			break;
		case RegionTest::GoodRegion:
			_fallingBalls.erase (_fallingBalls.begin () + i);
			HandleGoodRegionEnd (item->ball);
			continue;
		case RegionTest::WrongRegion:
			_fallingBalls.erase (_fallingBalls.begin () + i);
			HandleWrongRegionEnd (item->ball);
			continue;
		}

		//Test fall off from screen
		float yTest = item->ball->Pos.y - item->ball->boundingBox.Height () * item->ball->Scale.y / 2.0f;
		if (yTest < 0 || yTest > game.ScreenHeight ()) { //When ball has fallen
			if (item->ball->Type () == Ball::Color::Bomb) {
				_fallingBalls.erase (_fallingBalls.begin () + i);
				item->ball->Shutdown ();
			} else { //Fail
				float xTest = item->ball->Pos.x - item->ball->boundingBox.Width () * item->ball->Scale.x / 2.0f;
				if (xTest >= 0 && xTest <= game.ScreenWidth ())
					HandleBallFallFailEnd ();
				else { //Ball has fallen, but not fail (hack)
					_fallingBalls.erase (_fallingBalls.begin () + i);
					item->ball->Shutdown ();
				}
				break;
			}
		}
	}

	//Calculate ended ball animation
	for (int i = (int) _endedBalls.size () - 1; i >= 0; --i) {
		shared_ptr<QTEGoodBall> item = _endedBalls[i];
		item->Update (elapsedTime);
		if (item->IsEnded ()) {
			_endedBalls.erase (_endedBalls.begin () + i);
			item->Stop ();
			item->Shutdown ();
		}
	}

	//Calculate wrong ball animation
	for (int i = (int) _wrongBalls.size () - 1; i >= 0; --i) {
		shared_ptr<QTEGoodBall> item = _wrongBalls[i];
		item->Update (elapsedTime);
		if (item->IsEnded ()) {
			_wrongBalls.erase (_wrongBalls.begin () + i);
			item->Stop ();
			item->Shutdown ();

			HandleBallFallFailEnd ();
		}
	}

	//Calculate exploded ball animation
	for (int i = (int) _explodedBalls.size () - 1; i >= 0; --i) {
		shared_ptr<QTEExplodeBall> item = _explodedBalls[i];
		item->Update (elapsedTime);
		if (item->IsEnded ()) {
			_explodedBalls.erase (_explodedBalls.begin () + i);
			item->Stop ();
			item->Shutdown ();

			HandleBallFallFailEnd ();
		}
	}

	if (_fail != nullptr)
		_fail->Update (elapsedTime);

	//...
}

void FallScene::Render () {
	glClearColor (1.0f, 0.5f, 0.5f, 1.0f);
	glClear (GL_COLOR_BUFFER_BIT);

	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity ();

	_background->Render ();

	//Draw falling balls
	for (shared_ptr<FallingBall> item : _fallingBalls)
		item->ball->Render ();

	//Draw dragged balls
	for (auto& item : _touchedBalls)
		item.second->ball->Render ();

	//Draw ended balls
	for (shared_ptr<QTEGoodBall> item : _endedBalls)
		item->Render ();

	//Draw wrong balls
	for (shared_ptr<QTEGoodBall> item : _wrongBalls)
		item->Render ();

	//Draw exploded balls
	for (shared_ptr<QTEExplodeBall> item : _explodedBalls)
		item->Render ();

	//QTE texts
	if (_ready != nullptr)
		_ready->Render ();

	if (_steady != nullptr)
		_steady->Render ();

	if (_go != nullptr)
		_go->Render ();

	if (_fail != nullptr)
		_fail->Render ();
}

void FallScene::TouchDown (int fingerID, float x, float y) {
	Scene::TouchDown (fingerID, x, y);

	switch (_state) {
	case State::Game:
	{
		Game& game = Game::Get ();
		auto it = _touchedBalls.find (fingerID);
		if (it == _touchedBalls.end ()) {
			for (size_t i = _fallingBalls.size (); i > 0; --i) {
				shared_ptr<FallingBall> item = _fallingBalls[i - 1];
				Vector2D pos = game.ToLocal (x, y);

				Rect2D testRect = item->ball->TransformedBoundingBox ().Scale (Vector2D (1.1f, 1.1f));
				testRect.leftTop -= Vector2D (0, 0.15f);
				if (testRect.Contains (pos)) {
					if (item->ball->Type () == Ball::Color::Bomb) {
						_fallingBalls.erase (_fallingBalls.begin () + (i - 1));
						HandleBombBlowEnd (item->ball);
					} else {
						item->lastTouch = _fullTime;
						item->touchPosition = pos;
						item->lastVelocities.clear ();
						_touchedBalls.insert ({ fingerID, item });
						_fallingBalls.erase (_fallingBalls.begin () + (i - 1));
					}
					break;
				}
			}
		}
		break;
	}
	default:
		break;
	}
}

void FallScene::TouchUp (int fingerID, float x, float y) {
	Scene::TouchUp (fingerID, x, y);

	switch (_state) {
	case State::Game:
	{
		auto it = _touchedBalls.find (fingerID);
		if (it != _touchedBalls.end ()) {
			shared_ptr<FallingBall> item = it->second;
			RefreshTouchedBall (item, x, y);

			RegionTest test = TestBallInEndRegions (item);
			switch (test) {
			default:
			case RegionTest::NotInRegion:
				_fallingBalls.push_back (item);
				_touchedBalls.erase (it);
				break;
			case RegionTest::GoodRegion:
				_touchedBalls.erase (it);
				HandleGoodRegionEnd (item->ball);
				break;
			case RegionTest::WrongRegion:
				HandleWrongRegionEnd (item->ball);
				break;
			}
		}
		break;
	}
	default:
		break;
	}
}

void FallScene::TouchMove (int fingerID, float x, float y) {
	Scene::TouchMove (fingerID, x, y);

	switch (_state) {
	case State::Game:
	{
		auto it = _touchedBalls.find (fingerID);
		if (it != _touchedBalls.end ()) {
			shared_ptr<FallingBall> item = it->second;
			RefreshTouchedBall (item, x, y);

			RegionTest test = TestBallInEndRegions (item);
			switch (test) {
			default:
			case RegionTest::NotInRegion:
				break;
			case RegionTest::GoodRegion:
				_touchedBalls.erase (it);
				HandleGoodRegionEnd (item->ball);
				break;
			case RegionTest::WrongRegion:
				HandleWrongRegionEnd (item->ball);
				break;
			}
		}
		break;
	}
	default:
		break;
	}
}

void FallScene::RefreshTouchedBall (shared_ptr<FallingBall> item, float x, float y) {
	Game& game = Game::Get ();
	Vector2D curPos = game.ToLocal (x, y);
	if (item->ball->Pos != curPos) {
		item->body->LastPos = item->ball->Pos;
		item->ball->Pos = curPos;

		float elapsedTime = _fullTime - item->lastTouch;
		item->lastTouch = _fullTime;

		Vector2D dist = item->ball->Pos - item->body->LastPos;
		Vector2D velocity = dist / elapsedTime * RigidBody2D::PhysicalScale;
		item->lastVelocities.push_back (velocity);

		while (item->lastVelocities.size () > 3)
			item->lastVelocities.pop_front ();

		float touchDist = (item->ball->Pos - item->touchPosition).Length ();
		if (touchDist > 0.05f) { //Prevent the single touch to speed up the touched ball
			Vector2D avgVel;
			for (auto vel : item->lastVelocities)
				avgVel += vel;
			avgVel /= (float) item->lastVelocities.size ();

			item->body->Velocity = avgVel;

			if (item->body->Velocity.Length () > 3.0f) {
				item->body->Velocity = item->body->Velocity.Normalize () * 3.0f;
			}
		}
	}
}

shared_ptr<RigidBody2D> FallScene::FindCollision (shared_ptr<RigidBody2D> body) {
	//Check dragged balls for collision
	float radius = 0.1f;
	for (auto& rec : _touchedBalls) {
		shared_ptr<FallingBall> item = rec.second;
		if (item->body == body)
			continue;

		Vector2D dist = item->ball->Pos - body->Mesh->Pos;
		if (dist.Length () < radius * 2.0f)
			return item->body;
	}

	//Check other falling balls for collision
	for (size_t i = 0; i < _fallingBalls.size (); ++i) {
		shared_ptr<FallingBall> item = _fallingBalls[i];
		if (item->body == body)
			continue;

		Vector2D dist = item->ball->Pos - body->Mesh->Pos;
		if (dist.Length () < radius * 2.0f)
			return item->body;
	}

	return nullptr;
}

FallScene::RegionTest FallScene::TestBallInEndRegions (shared_ptr<FallingBall> item) const {
	if (_yellowRegion.Contains (item->ball->Pos)) {
		return item->ball->Type () == Ball::Color::Yellow || item->ball->Type () == Ball::Color::Magic ? RegionTest::GoodRegion : RegionTest::WrongRegion;
	} else if (_greenRegion.Contains (item->ball->Pos)) {
		return item->ball->Type () == Ball::Color::Green || item->ball->Type () == Ball::Color::Magic ? RegionTest::GoodRegion : RegionTest::WrongRegion;
	} else if (_redRegion.Contains (item->ball->Pos)) {
		return item->ball->Type () == Ball::Color::Red || item->ball->Type () == Ball::Color::Magic ? RegionTest::GoodRegion : RegionTest::WrongRegion;
	} else if (_blueRegion.Contains (item->ball->Pos)) {
		return item->ball->Type () == Ball::Color::Blue || item->ball->Type () == Ball::Color::Magic ? RegionTest::GoodRegion : RegionTest::WrongRegion;
	}

	return RegionTest::NotInRegion;
}

void FallScene::HandleGoodRegionEnd (shared_ptr<Ball> ball) {
	shared_ptr<QTEGoodBall> qte (new QTEGoodBall (ball));
	qte->Init ();
	qte->Start ();
	_endedBalls.push_back (qte);

	++_score;
	RefreshOverlays (true);
}

void FallScene::HandleWrongRegionEnd (shared_ptr<Ball> ball) {
	shared_ptr<QTEGoodBall> qte (new QTEGoodBall (ball));
	qte->Init ();
	qte->Start ();
	_wrongBalls.push_back (qte);
}

void FallScene::HandleBombBlowEnd (shared_ptr<Ball> ball) {
	shared_ptr<QTEExplodeBall> qte (new QTEExplodeBall (ball));
	qte->Init ();
	qte->Start ();
	_explodedBalls.push_back (qte);
}

void FallScene::HandleBallFallFailEnd () {
	_state = State::FallError;

	for (shared_ptr<FallingBall> item : _fallingBalls)
		item->ball->Shutdown ();
	_fallingBalls.clear ();

	for (auto& item : _touchedBalls)
		item.second->ball->Shutdown ();
	_touchedBalls.clear ();

	_fail.reset (new QTEGrowText ("fail.png", 0.1f));
	_fail->Init ();
	_fail->Start ();
}

void FallScene::RefreshOverlays (bool force) {
	int mins = (int) (_fullTime / 60.0f);
	int secs = (int) (fmodf (_fullTime, 60.0f));
	if (force || mins != _lastMins || secs != _lastSecs) {
		_lastMins = mins;
		_lastSecs = secs;

		IContentManager& contentManager = Game::ContentManager ();

		stringstream ssTopLeft;
		ssTopLeft << "Level: " << (_currentLevel + 1) << " Time: " << setw (2) << setfill ('0') << mins << ":" << setw (2) << setfill ('0') << secs;
		contentManager.SetTopLeftStatus (ssTopLeft.str ());

		stringstream ssTopRight;
		ssTopRight << "Score: " << _score;
		contentManager.SetTopRightStatus (ssTopRight.str ());
	}
}