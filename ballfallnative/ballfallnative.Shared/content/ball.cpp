#include "pch.h"
#include "ball.h"

const char* Ball::TypeAsString () const {
	switch (mColor) {
	case Color::Red: return "red";
	case Color::Green: return "green";
	case Color::Blue: return "blue";
	case Color::Yellow: return "yellow";
	case Color::Magic: return "magic";
	case Color::Bomb: return "bomb";
	default:
		assert (nullptr); //Not implemented
	}

	return "unknown";
}

void Ball::Init () {
	Mesh2D::Init ();

	string asset;
	switch (mColor) {
	case Color::Red: asset = "ball_red.png"; break;
	case Color::Green: asset = "ball_green.png"; break;
	case Color::Blue: asset = "ball_blue.png"; break;
	case Color::Yellow: asset = "ball_yellow.png"; break;
	case Color::Magic: asset = "ball_magic.png"; break;
	case Color::Bomb: asset = "bomb.png"; break;
	default:
		assert (nullptr); //Not implemented
	}

	mTex = LoadTextureFromAsset (asset);
	mVbo = NewTexturedVBO (mTex);
}

void Ball::Shutdown () {
	if (mVbo.size () > 0) {
		glDeleteBuffers (mVbo.size (), &mVbo[0]);
		mVbo.clear ();
	}

	if (mTex > 0) {
		glDeleteTextures (1, &mTex);
		mTex = 0;
	}

	Mesh2D::Shutdown ();
}

void Ball::RenderMesh () {
	RenderTexturedVBO (mTex, mVbo[0], mVbo[1]);
}
