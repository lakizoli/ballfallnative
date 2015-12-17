#pragma once

#include "content/qte.h"
#include "ball.h"
#include "content/animation.h"
#include "content/vector2D.h"

class QTEPulseBall : public QTE {
private:
	shared_ptr<Ball> _ball;
	Vector2D _ballOriginalScale;
	PulseAnimation _pulse;

public:
	QTEPulseBall (shared_ptr<Ball> ball) :_ball (ball), _pulse (2, 1) {}

	virtual void Init () override {
		QTE::Init ();
		_ballOriginalScale = _ball->Scale;
	}

protected:
	virtual void OnStart () override {
		QTE::OnStart ();
		_pulse.Start ();
	}

	virtual void OnStop () override {
		QTE::OnStop ();
		_pulse.Stop ();
		_ball->Scale = _ballOriginalScale;
	}

	virtual void OnUpdate (float elapsedTime) override {
		QTE::OnUpdate (elapsedTime);
		_pulse.Update (elapsedTime);
		_ball->Scale = _ballOriginalScale * _pulse.Scale ();
	}

	virtual void OnRender () override {
		QTE::OnRender ();
		_ball->Render ();
	}
};
