#pragma once

#include "content/mesh2D.h"

class Ball : public Mesh2D {
public:
	enum class Color {
		Red,
		Green,
		Blue,
		Yellow,
		Magic,
		Bomb
	};

private:
	Color mColor;
	GLuint mTex;
	vector<GLuint> mVbo;

public:
	Color Type () const {
		return mColor;
	}

	const char* TypeAsString () const;

public:
	Ball (Color color) : mColor (color) {}

	virtual void Init () override;
	virtual void Shutdown () override;

protected:
	virtual void RenderMesh () override;
};
