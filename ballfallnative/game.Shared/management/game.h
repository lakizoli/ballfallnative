#pragma once

#include "IUtil.h"
#include "IContentManager.h"
#include "scene.h"
#include "content/vector2D.h"

//J�t�k �letek:
//1.) ballfall: lees� labd�k v�logat�sa k�l�nb�z� sz�n� cs�vekbe (vagy allatok etetese)
//2.) dek�zo j�t�k: egy labd�t kell dek�zni mindenf�le sz�lben, es�ben, ill. k�l�nb�z� skill-ek gesture f�gg�en stb...
//3.) trojan horse
//4.) varfal v�delem
//5.) krakout, traz (C64)
//6.) hunch back (C64)
//7.) p�rg� leveleken ugr�l� b�ka

/// Base class of the game.
class Game {
//Data 
private:
	static Game* mGame;
	IUtil& mUtil;
	IContentManager& mContentManager;

	int mWidth;
	int mHeight;

	shared_ptr<Scene> mCurrentScene;

//Construction
protected:
	Game (IUtil& util, IContentManager& contentManager);

public:
	virtual ~Game () {
		mGame = nullptr;
	}

	static Game& Get () {
		assert (mGame != nullptr);
		return *mGame;
	}

	static IUtil& Util () {
		assert (mGame != nullptr);
		return mGame->mUtil;
	}

	static IContentManager& ContentManager () {
		assert (mGame != nullptr);
		return mGame->mContentManager;
	}

//Management interface
public:
	virtual void Init (int width, int height);
	virtual void Shutdown ();

	void Resize (int oldWidth, int oldHeight, int newWidth, int newHeight);

	/// <summary>
	/// The update step of the game.
	/// </summary>
	/// <param name="elapsedTime">The elapsed time from the last update in seconds.</param>
	virtual void Update (float elapsedTime);
	virtual void Render ();

//Scene interface
public:
	shared_ptr<Scene> CurrentScene () const {
		return mCurrentScene;
	}

	void SetCurrentScene (shared_ptr<Scene> scene);

//Input handlers
public:
	virtual void TouchDown (int fingerID, float x, float y);
	virtual void TouchUp (int fingerID, float x, float y);
	virtual void TouchMove (int fingerID, float x, float y);

//Helpers
public:
	float ScreenWidth () const {
		return ToLocal (mWidth, 0).x;
	}

	float ScreenHeight () const {
		return ToLocal (0, mHeight).y;
	}

	Vector2D ToLocal (float x, float y) const;

//Inner methods
private:
	void InitProjection (int width, int height);
};
