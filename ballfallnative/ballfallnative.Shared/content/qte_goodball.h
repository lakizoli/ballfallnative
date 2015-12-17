#pragma once

#include "content/qte.h"
#include "ball.h"
#include "content/animation.h"
#include "content/vector2D.h"

class QTEGoodBall : public QTE {
private:
	shared_ptr<Ball> _ball;
	Vector2D _ballOriginalScale;
	LinearAnimation _scale;

public:
	bool IsEnded () const {
		return _scale.Value () < 0.105f;
	}

	QTEGoodBall (shared_ptr<Ball> ball) : _ball (ball), _scale (1.0f, 0.1f, 0.1f) {}

	virtual void Init () override {
		QTE::Init ();
		_ballOriginalScale = _ball->Scale;
	}

	virtual void Shutdown () override {
		QTE::Shutdown ();
		_ball->Shutdown ();
	}

protected:
	virtual void OnStart () override {
		QTE::OnStart ();
		_scale.Start ();
	}

	virtual void OnStop () override {
		QTE::OnStop ();
		_scale.Stop ();
		_ball->Scale = _ballOriginalScale;
	}

	virtual void OnUpdate (float elapsedTime) override {
		QTE::OnUpdate (elapsedTime);
		_scale.Update (elapsedTime);
		_ball->Scale = _ballOriginalScale * _scale.Value ();
	}

	virtual void OnRender () override {
		QTE::OnRender ();
		_ball->Render ();
	}
};
