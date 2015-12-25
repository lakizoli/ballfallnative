#pragma once

#include "IUtil.h"
#include "IContentManager.h"
#include "scene.h"
#include "content/vector2D.h"

//J�t�k �letek:
//1.) ballfall: lees� labd�k v�logat�sa k�l�nb�z� sz�n� cs�vekbe (vagy allatok etetese)
//1b.) labd�s j�t�k: olyan, mint a v�rfal v�delem, csak itt az a j�t�k, hogy meg kell akad�lyozni, hogy rossz sz�n� labda menjen le egy adott sz�n� cs�ben. Ki kell lukasztani a rossz sz�n� bubikat.
//2.) dek�zo j�t�k: egy labd�t kell dek�zni mindenf�le sz�lben, es�ben, ill. k�l�nb�z� skill-ek gesture f�gg�en stb...
//3.) trojan horse (nintendo handheld)
//4.) varfal v�delem (nintendo handheld)
//5.) krakout, traz (C64) (krakout m�r van android-ra, �s sz�p is sajnos...) TRAZ m�g nincs...
//5b.) Ugyanaz a bontogat�s j�t�k, de �gy, hogy a labd�t a fallal kell �t�getni. Teh�t a telefon mozgat�s�val lehet a labd�t mindenfel� �t�getni, ir�ny�tani... :)
//6.) hunch back (C64) (M�r van egy r�gi �tirat, de szerintem �n jobbat fogok csin�lni..., viszont iOS-en van egy nagyon kir�ly :( )
//7.) p�rg� leveleken ugr�l� b�ka
//8.) Aknakeres� k�l�nb�z� alak� p�ly�kkal... (be kellene m�r fejezni)
//9.) Tangoes j�t�k (faj�t�k)
//10.) snapszer

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
