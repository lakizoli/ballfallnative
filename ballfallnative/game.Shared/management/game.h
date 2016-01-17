#pragma once

#include "IUtil.h"
#include "IContentManager.h"
#include "scene.h"
#include "content/vector2D.h"

//J�t�k �letek:
//1.) ballfall: lees� labd�k v�logat�sa k�l�nb�z� sz�n� cs�vekbe (vagy allatok etetese)
//2.) labd�s j�t�k: olyan, mint a v�rfal v�delem, csak itt az a j�t�k, hogy meg kell akad�lyozni, hogy rossz sz�n� labda menjen le egy adott sz�n� cs�ben. Ki kell lukasztani a rossz sz�n� bubikat.
//3.) ugyanaz, mint el�bb, csak lufik, �s felfel� sz�llnak. Ki kell lukasztani a rossz sz�n� cs�be men� lufikat...
//4.) A r�gi macsk�s fizikai (robbant�s) j�t�k mint�j�ra, legyen egy olyan j�t�k, ahol bomb�k lerak�s�val lehet p�ly�kat lebontani. Az a c�l, hogy minden "macska", csillag ak�rmi leessen, vagy felrobbanjon meghat�rozott sz�m� bomb�b�l...
//5.) Var�zsk�pek matek j�t�k (zolcsi matekh�zija, sz�rz�t�bla)
//6.) dek�zo j�t�k: egy labd�t kell dek�zni mindenf�le sz�lben, es�ben, ill. k�l�nb�z� skill-ek gesture f�gg�en stb...
//7.) trojan horse (nintendo handheld)
//8.) varfal v�delem (nintendo handheld)
//9.) krakout, traz (C64) (krakout m�r van android-ra, �s sz�p is sajnos...) TRAZ m�g nincs...
//9b.) Ugyanaz a bontogat�s j�t�k, de �gy, hogy a labd�t a fallal kell �t�getni. Teh�t a telefon mozgat�s�val lehet a labd�t mindenfel� �t�getni, ir�ny�tani... :)
//10.) hunch back (C64) (M�r van egy r�gi �tirat, de szerintem �n jobbat fogok csin�lni..., viszont iOS-en van egy nagyon kir�ly :( )
//11.) p�rg� leveleken ugr�l� b�ka
//12.) Aknakeres� k�l�nb�z� alak� p�ly�kkal... (be kellene m�r fejezni)
//13.) Tangoes j�t�k (faj�t�k)
//14.) snapszer

/// Base class of the game.
class Game {
//Data 
private:
	static Game* mGame;
	IUtil& mUtil;
	IContentManager& mContentManager;

	int mWidth;
	int mHeight;

	int mRefWidth;
	int mRefHeight;

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
	virtual void Init (int width, int height, int refWidth, int refHeight);
	virtual void Shutdown ();

	void Resize (int newWidth, int newHeight);

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

	float WidthRatio () const {
		return (float) mWidth / (float) mRefWidth;
	}

	float HeightRatio () const {
		return (float) mHeight / (float) mRefHeight;
	}

	Vector2D ToLocal (float x, float y) const;

//Inner methods
private:
	void InitProjection (int width, int height);
};
