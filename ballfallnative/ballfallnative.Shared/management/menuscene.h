#pragma once

#include "management/scene.h"
#include "content/imagemesh.h"

class MenuScene : public Scene {
//Definitions
private:

//Data
private:
	shared_ptr<ImageMesh> _background;
	shared_ptr<ImageMesh> _start;
	vector<shared_ptr<ImageMesh>> _score;
	int _lastHighScore;

	Rect2D _startRegion;
	bool _startPressed;
	bool _startInPressedState;

	int _clickSoundID;

//Construction
public:
	MenuScene () {}

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
};
