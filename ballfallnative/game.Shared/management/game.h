#pragma once

#include "content/vector2D.h"
#include "scene.h"

//Játék öletek:
//1.) ballfall: leesõ labdák válogatása különbözõ színû csövekbe (vagy allatok etetese)
//2.) dekázo játék: egy labdát kell dekázni mindenféle szélben, esõben, ill. különbözõ skill-ek gesture függõen stb...
//3.) trojan horse
//4.) varfal védelem
//5.) krakout, traz (C64)
//6.) hunch back (C64)
//7.) pörgõ leveleken ugráló béka

class IContentManager {
	//IDisposable LoadImage (string asset);
	//IntPtr LockPixels (object image);
	//void UnlockPixels (object image);
	//int GetWidth (object image);
	//int GetHeight (object image);

	//void SetTopLeftStyle (float size, float red, float green, float blue, float alpha);
	//void SetTopRightStyle (float size, float red, float green, float blue, float alpha);

	//void SetTopLeftStatus (string text);
	//void SetTopRightStatus (string text);
};

class IUtil {
	virtual void Log (string log) = 0;
};

/// Base class of the game.
class Game {
//Management 
private:
	static shared_ptr<Game> mGame;
	static shared_ptr<IUtil> mUtil;
	static shared_ptr<IContentManager> mContentManager;

    int mWidth;
    int mHeight;

	unique_ptr<Scene> mCurrentScene;

//        public Scene CurrentScene
//        {
//            get { return _currentScene; }
//            set
//            {
//                if (_currentScene != null) {
//                    _currentScene.Shutdown ();
//                }
//                if (value != null) {
//                    value.Init (_width, _height);
//                }
//                _currentScene = value;
//            }
//        }
//
//        #endregion
//

//Construction
protected:
	Game (IUtil& util, IContentManager& contentManager) : mWidth (0), mHeight (0) {
        mGame.reset (this);
        mUtil.reset (&util);
        mContentManager.reset (&contentManager);
    }

public:
	virtual ~Game () {
		mGame.reset ();
		mUtil.reset ();
		mContentManager.reset ();
	}

	static Game& Get () {
		return *mGame;
	}

	static IUtil& GetUtil () {
		return *mUtil;
	}

	static IContentManager& GetContentManager () {
		return *mContentManager;
	}

//Management interface
public:
//        public virtual void Init (int width, int height) {
//            InitProjection (width, height);
//        }
//
//        public virtual void Shutdown () {
//            CurrentScene = null;
//        }
//
//        public void Resize (int oldWidth, int oldHeight, int newWidth, int newHeight) {
//            InitProjection (newWidth, newHeight);
//
//            if (_currentScene != null)
//                _currentScene.Resize (oldWidth, oldHeight, newWidth, newHeight);
//        }
//
//        /// <summary>
//        /// The update step of the game.
//        /// </summary>
//        /// <param name="elapsedTime">The elapsed time from the last update in seconds.</param>
//        public virtual void Update (float elapsedTime) {
//            if (_currentScene != null)
//                _currentScene.Update (elapsedTime);
//        }
//
//        public virtual void Render () {
//            if (_currentScene != null)
//                _currentScene.Render ();
//        }
//        #endregion
//
//        #region Input handlers
//        public virtual void TouchDown (int fingerID, float x, float y) {
//            if (_currentScene != null)
//                _currentScene.TouchDown (fingerID, x, y);
//        }
//
//        public virtual void TouchUp (int fingerID, float x, float y) {
//            if (_currentScene != null)
//                _currentScene.TouchUp (fingerID, x, y);
//        }
//
//        public virtual void TouchMove (int fingerID, float x, float y) {
//            if (_currentScene != null)
//                _currentScene.TouchMove (fingerID, x, y);
//        }
//        #endregion

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
