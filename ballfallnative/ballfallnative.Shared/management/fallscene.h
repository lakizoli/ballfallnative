#pragma once

#include "management/scene.h"
#include "management/game.h"
#include "content/ball.h"
#include "content/rigidbody2D.h"
#include "content/vector2D.h"
#include "content/rect2D.h"
#include "content/qte_growtext.h"
#include "content/qte_explodeball.h"
#include "content/qte_goodball.h"

class FallScene : public Scene {
//Definitions
private:
	struct FallingBall {
		shared_ptr<Ball> ball;
		shared_ptr<RigidBody2D> body;

		float lastTouch;
		Vector2D touchPosition;
		deque<Vector2D> lastVelocities;

		FallingBall (shared_ptr<Ball> ball, shared_ptr<RigidBody2D> body) : ball (ball), body (body), lastTouch (0.0f) {}
	};

	struct LevelDefinition {
		int index; ///< The index of the level
		float endTime; ///< The absolute end time of the level [sec]
		int minAddTime; ///< minimal time of next add time range [millisec]
		int maxAddTime; ///< maximal time of next add time range [millisec]
		float startVelocityY;
		int maxAddCount;
	};

	enum class RegionTest {
		NotInRegion,
		GoodRegion,
		WrongRegion
	};

	enum class State {
		PreGame,
		Game,
		FallError,
		BlowError
	};

//Data
private:
	vector<LevelDefinition> _levels;
	int _currentLevel;
	int _score;
	int _lastMins;
	int _lastSecs;

	float _fullTime;
	float _lastAddTime;

	Rect2D _yellowRegion;
	Rect2D _greenRegion;
	Rect2D _redRegion;
	Rect2D _blueRegion;

	shared_ptr<QTEGrowText> _ready;
	shared_ptr<QTEGrowText> _steady;
	shared_ptr<QTEGrowText> _go;

	bool _readyStart;
	int _readySoundID;
	int _steadySoundID;
	int _goSoundID;
	int _startSoundID;
	int _explosionSoundID;
	int _whooshSoundID;

	vector<shared_ptr<FallingBall>> _fallingBalls;
	vector<shared_ptr<QTEGoodBall>> _endedBalls;
	vector<shared_ptr<QTEGoodBall>> _wrongBalls;
	vector<shared_ptr<QTEExplodeBall>> _explodedBalls;
	shared_ptr<ImageMesh> _background_1;
	shared_ptr<ImageMesh> _background_2;
	shared_ptr<ImageMesh> _foreground;
	shared_ptr<QTEGrowText> _fail;
	State _state;
	int _background_idx;

	map<int, shared_ptr<FallingBall>> _touchedBalls;

	bool _failTouched;

//Construction
public:
	FallScene () {}

//Interface
public:
	virtual void Init (int width, int height) override;
	virtual void Shutdown () override;

	virtual void Resize (int newWidth, int newHeight) override;

	virtual void Update (float elapsedTime) override;
	virtual void Render () override;

//Input handlers
public:
	virtual void TouchDown (int fingerID, float x, float y) override;
	virtual void TouchUp (int fingerID, float x, float y) override;
	virtual void TouchMove (int fingerID, float x, float y) override;

//Helper methods
private:
	int NextRandom (int exclusiveMaxValue) const {
		return rand () % exclusiveMaxValue;
	}

	int NextRandom (int inclusiveMinValue, int exclusiveMaxValue) const {
		return (rand () % (exclusiveMaxValue - inclusiveMinValue)) + inclusiveMinValue;
	}

	void AddNewBalls (const LevelDefinition& level);
	void RefreshTouchedBall (shared_ptr<FallingBall> item, float x, float y);

	shared_ptr<RigidBody2D> FindCollision (const RigidBody2D* body);

	RegionTest TestBallInEndRegions (shared_ptr<FallingBall> item) const;
	void HandleGoodRegionEnd (shared_ptr<Ball> ball);
	void HandleWrongRegionEnd (shared_ptr<Ball> ball);
	void HandleBombBlowEnd (shared_ptr<Ball> ball);
	void HandleBallFallFailEnd ();

	void RefreshOverlays (bool force);
};
