#pragma once

#include "content/qte.h"
#include "imagemesh.h"
#include "content/animation.h"
#include "content/vector2D.h"
#include "management/game.h"

class QTEGrowText : public QTE {
private:
	ImageMesh _test;
	LinearAnimation _scale;

public:
	bool IsEnded () const {
		return _scale.Value () >= 0.29f;
	}

	QTEGrowText (const string& asset, float timeFrame) : _test (asset), _scale (0.01f, 0.3f, timeFrame) {}

	virtual void Init () override {
		QTE::Init ();

		Game& game = Game::Get ();

		_test.Init ();
		_test.Scale = Vector2D (0.01f, 0.01f);
		_test.Pos = Vector2D (game.ScreenWidth () / 2.0f, game.ScreenHeight () / 2.0f);
	}

	virtual void Shutdown () override {
		QTE::Shutdown ();
		_test.Shutdown ();
	}

protected:
	virtual void OnStart () override {
		QTE::OnStart ();
		_scale.Start ();
	}

	virtual void OnStop () override {
		QTE::OnStop ();
		_scale.Stop ();
	}

	virtual void OnUpdate (float elapsedTime) override {
		QTE::OnUpdate (elapsedTime);
		_scale.Update (elapsedTime);
		_test.Scale = Vector2D (_scale.Value (), _scale.Value ());
	}

	virtual void OnRender () override {
		QTE::OnRender ();
		_test.Render ();
	}
};
