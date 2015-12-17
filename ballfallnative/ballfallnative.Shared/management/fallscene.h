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
		FallError
	};

//Data
private:
    vector<LevelDefinition> _levels;
    int _currentLevel;
    int _score;
    int _lastMins;
    int _lastSecs;

    //Random _random;
    float _fullTime;
    float _lastAddTime;

    Rect2D _yellowRegion;
    Rect2D _greenRegion;
    Rect2D _redRegion;
    Rect2D _blueRegion;

    shared_ptr<QTEGrowText> _ready;
    shared_ptr<QTEGrowText> _steady;
    shared_ptr<QTEGrowText> _go;

    vector<shared_ptr<FallingBall>> _fallingBalls;
    vector<shared_ptr<QTEGoodBall>> _endedBalls;
    vector<shared_ptr<QTEGoodBall>> _wrongBalls;
    vector<shared_ptr<QTEExplodeBall>> _explodedBalls;
    shared_ptr<ImageMesh> _background;
    shared_ptr<QTEGrowText> _fail;
    State _state;

    map<int, shared_ptr<FallingBall>> _touchedBalls;

//Construction
public:
    FallScene () {}

//Interface
public:
	virtual void Init (int width, int height) override;
	virtual void Shutdown () override;

	virtual void Resize (int oldWidth, int oldHeight, int newWidth, int newHeight) override;

	virtual void Update (float elapsedTime) override;
	virtual void Render () override;

//Input handlers
public:
	virtual void TouchDown (int fingerID, float x, float y) override;
	virtual void TouchUp (int fingerID, float x, float y) override;
	virtual void TouchMove (int fingerID, float x, float y) override;

//Helper methods
private:
    void AddNewBalls (const LevelDefinition& level) {
    //    if (_state == State::Game && _lastAddTime > _random.Next (level.minAddTime, level.maxAddTime) / 1000.0f) {
    //        int addCount = 1; //_random.Next (1, level.maxAddCount + 1);
    //        for (int i = 0; i < addCount; ++i) {
    //            Ball ball = null;
    //            switch (_random.Next (6)) {
    //                default:
    //                case 0: ball = new Ball (Ball.Color.Red); break;
    //                case 1: ball = new Ball (Ball.Color.Green); break;
    //                case 2: ball = new Ball (Ball.Color.Blue); break;
    //                case 3: ball = new Ball (Ball.Color.Yellow); break;
    //                case 4: ball = new Ball (Ball.Color.Magic); break;
    //                case 5: ball = new Ball (Ball.Color.Bomb); break;
    //            }

    //            ball.Init ();
    //            ball.Scale = new Vector2D (0.1f, 0.1f);

    //            float screenWidth = Game.Instance.ScreenWidth / (float)addCount;
    //            float screenOffset = i * Game.Instance.ScreenWidth / (float)addCount;
    //            float border = ball.TransformedBoundingBox.Width / 2.0f + Game.Instance.ToLocal (20 * 4, 0).X;
    //            float newX = _random.Next ((int)(screenOffset * 1000.0f), (int)((screenOffset + screenWidth - 2.0f * border) * 1000.0f)) / 1000.0f + border;

    //            ball.Pos = new Vector2D (newX, ball.TransformedBoundingBox.Height / 2.0f);

    //            RigidBody2D body = new RigidBody2D () {
    //                Mesh = ball,
    //                Velocity = new Vector2D (0, level.startVelocityY),
    //                Mass = 1.0f, //1 kg
    //                FindCollision = new RigidBody2D.FindCollisionDelegate (FindCollision)
    //            };

    //            _fallingBalls.Add (new FallingBall () {
    //                ball = ball,
    //                body = body
    //            });
    //            _lastAddTime = 0;
    //        }
    //    }
    }

	void RefreshTouchedBall (shared_ptr<FallingBall> item, float x, float y);

	shared_ptr<RigidBody2D> FindCollision (shared_ptr<RigidBody2D> body);

	RegionTest TestBallInEndRegions (shared_ptr<FallingBall> item) const;

	void HandleGoodRegionEnd (shared_ptr<Ball> ball);
	void HandleWrongRegionEnd (shared_ptr<Ball> ball);
	void HandleBombBlowEnd (shared_ptr<Ball> ball);
	void HandleBallFallFailEnd ();

	void RefreshOverlays (bool force);
};
