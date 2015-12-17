#include "pch.h"
#include "ball.h"

void Ball::Init () {
	Mesh2D::Init ();

	string asset;
	switch (mColor) {
	case Color::Red: asset = "ball_red.png"; break;
	case Color::Green: asset = "ball_green.png"; break;
	case Color::Blue: asset = "ball_blue.png"; break;
	case Color::Yellow: asset = "ball_yellow.png"; break;
	case Color::Magic: asset = "ball_magic.png"; break; //TODO: magic-rõl az árnyék leszedése
	case Color::Bomb: asset = "bomb.png"; break; //TODO: bomb-ról az árnyék leszedése
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
