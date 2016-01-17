#pragma once

#include "IUtil.h"
#include "IContentManager.h"
#include "scene.h"
#include "content/vector2D.h"

//Játék öletek:
//1.) ballfall: leesõ labdák válogatása különbözõ színû csövekbe (vagy allatok etetese)
//2.) labdás játék: olyan, mint a várfal védelem, csak itt az a játék, hogy meg kell akadályozni, hogy rossz színû labda menjen le egy adott színû csõben. Ki kell lukasztani a rossz színû bubikat.
//3.) ugyanaz, mint elõbb, csak lufik, és felfelé szállnak. Ki kell lukasztani a rossz színû csõbe menõ lufikat...
//4.) A régi macskás fizikai (robbantós) játék mintájára, legyen egy olyan játék, ahol bombák lerakásával lehet pályákat lebontani. Az a cél, hogy minden "macska", csillag akármi leessen, vagy felrobbanjon meghatározott számú bombából...
//5.) Varázsképek matek játék (zolcsi matekházija, szórzótábla)
//6.) dekázo játék: egy labdát kell dekázni mindenféle szélben, esõben, ill. különbözõ skill-ek gesture függõen stb...
//7.) trojan horse (nintendo handheld)
//8.) varfal védelem (nintendo handheld)
//9.) krakout, traz (C64) (krakout már van android-ra, és szép is sajnos...) TRAZ még nincs...
//9b.) Ugyanaz a bontogatós játék, de úgy, hogy a labdát a fallal kell ütögetni. Tehát a telefon mozgatásával lehet a labdát mindenfelé ütögetni, irányítani... :)
//10.) hunch back (C64) (Már van egy régi átirat, de szerintem én jobbat fogok csinálni..., viszont iOS-en van egy nagyon király :( )
//11.) pörgõ leveleken ugráló béka
//12.) Aknakeresõ különbözõ alakú pályákkal... (be kellene már fejezni)
//13.) Tangoes játék (fajáték)
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
