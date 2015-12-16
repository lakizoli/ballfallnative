#include "pch.h"
#include "game.h"

shared_ptr<Game> Game::mGame;
shared_ptr<IUtil> Game::mUtil;
shared_ptr<IContentManager> Game::mContentManager;

//        private Scene _currentScene;
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
//        public float ScreenWidth { get { return ToLocal (_width, 0).X; } }
//
//        public float ScreenHeight { get { return ToLocal (0, _height).Y; } }
//        #endregion
//
//        #region Construction
//        public Game (IUtil util, IContentManager contentManager) {
//            _game = this;
//            _util = util;
//            _contentManager = contentManager;
//        }
//        #endregion
//
//        #region Interface
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
//
Vector2D Game::ToLocal (float x, float y) const {
	float min = (float) std::min (mWidth, mHeight);
	float max = (float) std::max (mWidth, mHeight);
    float aspect = max / min;
    if (mWidth <= mHeight) {
        return Vector2D (x / (float)mWidth, aspect * y / (float)mHeight);
    }

    return Vector2D (aspect * x / (float)mWidth, y / (float)mHeight);
}

void Game::InitProjection (int width, int height) {
    mWidth = width;
    mHeight = height;

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();

    float min = (float) std::min (width, height);
    float max = (float) std::max (width, height);
    float aspect = max / min;
    if (width <= height) {
		glOrthof (0, 1.0f, aspect, 0, -1.0f, 1.0f);
    } else {
		glOrthof (0, aspect, 1.0f, 0, -1.0f, 1.0f);
    }
}