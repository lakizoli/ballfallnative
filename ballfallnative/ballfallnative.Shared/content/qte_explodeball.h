#pragma once

#include "content/qte.h"
#include "ball.h"
#include "imagemesh.h"
#include "content/animation.h"
#include "content/vector2D.h"

class QTEExplodeBall : public QTE {
private:
	shared_ptr<Ball> _ball;
	ImageMesh _explosion;
	FrameAnimation _frame;

public:
	bool IsEnded () const {
		return _frame.Frame () > 10;
	}

	QTEExplodeBall (shared_ptr<Ball> ball) : _ball (ball), _explosion ("explosion.png"), _frame (0.2f) {}

	virtual void Init () override {
		QTE::Init ();
		_explosion.Init ();
		_explosion.Scale = Vector2D (0.2f, 0.2f);
		_explosion.Pos = _ball->Pos;
	}

	virtual void Shutdown () override {
		QTE::Shutdown ();
		_explosion.Shutdown ();
		_ball->Shutdown ();
	}

protected:
	virtual void OnStart () override {
		QTE::OnStart ();
		_frame.Start ();
	}

	virtual void OnStop () override {
		QTE::OnStop ();
		_frame.Stop ();
	}

	virtual void OnUpdate (float elapsedTime) override {
		QTE::OnUpdate (elapsedTime);
		_frame.Update (elapsedTime);
		_explosion.Rotation = (_frame.Frame () % 4) * 90.0f;
		_ball->Pos = _explosion.Pos;
	}

	virtual void OnRender () override {
		QTE::OnRender ();
		_ball->Render ();
		_explosion.Render ();
	}
};
