#include "pch.h"
#include "game.h"

shared_ptr<Game> Game::mGame;

//using System;
//using OpenTK.Graphics.ES11;
//using game.content;
//
////Játék öletek:
////1.) ballfall: leesõ labdák válogatása különbözõ színû csövekbe
////2.) dekázo játék: egy labdát kell dekázni mindenféle szélben, esõben, ill. különbözõ skill-ek gesture függõen stb...
////3.) trojan horse
////4.) varfal védelem
////5.) krakout, traz (C64)
////6.) hunch back (C64)
////7.) pörgõ leveleken ugráló béka
//
//namespace game.management {
//    #region OS specific interfaces
//    public interface IContentManager {
//        IDisposable LoadImage (string asset);
//
//        IntPtr LockPixels (object image);
//
//        void UnlockPixels (object image);
//
//        int GetWidth (object image);
//
//        int GetHeight (object image);
//
//        void SetTopLeftStyle (float size, float red, float green, float blue, float alpha);
//
//        void SetTopRightStyle (float size, float red, float green, float blue, float alpha);
//
//        void SetTopLeftStatus (string text);
//
//        void SetTopRightStatus (string text);
//    }
//
//    public interface IUtil {
//        void Log (string log);
//    }
//    #endregion
//
//    public abstract class Game {
//        #region Management data
//        static Game _game;
//        public static Game Instance { get { return _game; } }
//
//        static IUtil _util;
//        public static IUtil Util { get { return _util; } }
//
//        static IContentManager _contentManager;
//        public static IContentManager ContentManager { get { return _contentManager; } }
//        #endregion
//
//        #region Data
//        int _width = 0;
//        int _height = 0;
//
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

glm::mat4& Game::SelectMatrix (Matrix matrix) {
	mCurrentMatrix = matrix;

	switch (matrix) {
	default:
	case Game::Matrix::Model:
		mMatrix = mModelMatrixStack.top ();
		break;
	case Game::Matrix::View:
		mMatrix = mViewMatrixStack.top ();
		break;
	case Game::Matrix::Projection:
		mMatrix = mProjectionMatrixStack.top ();
		break;
	}

	return mMatrix;
}

glm::mat4& Game::PushMatrix () {
	switch (mCurrentMatrix) {
	default:
	case Game::Matrix::Model:
		mModelMatrixStack.push (mMatrix);
		break;
	case Game::Matrix::View:
		mViewMatrixStack.push (mMatrix);
		break;
	case Game::Matrix::Projection:
		mProjectionMatrixStack.push (mMatrix);
		break;
	}

	return mMatrix;
}

glm::mat4& Game::PopMatrix () {
	switch (mCurrentMatrix) {
	default:
	case Game::Matrix::Model:
		mMatrix = mModelMatrixStack.top ();
		mModelMatrixStack.pop ();
		break;
	case Game::Matrix::View:
		mMatrix = mViewMatrixStack.top ();
		mViewMatrixStack.pop ();
		break;
	case Game::Matrix::Projection:
		mMatrix = mProjectionMatrixStack.top ();
		mProjectionMatrixStack.pop ();
		break;
	}

	return mMatrix;
}

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
//        #region Helper methods
//        public Vector2D ToLocal (float x, float y) {
//            float min = Math.Min (_width, _height);
//            float max = Math.Max (_width, _height);
//            float aspect = max / min;
//            if (_width <= _height) {
//                return new Vector2D (x / (float)_width, aspect * y / (float)_height);
//            }
//
//            return new Vector2D (aspect * x / (float)_width, y / (float)_height);
//        }
//        #endregion
//
//        #region Inner methods
//        private void InitProjection (int width, int height) {
//            _width = width;
//            _height = height;
//
//            GL.MatrixMode (All.Projection);
//            GL.LoadIdentity ();
//
//            float min = Math.Min (width, height);
//            float max = Math.Max (width, height);
//            float aspect = max / min;
//            if (width <= height) {
//                GL.Ortho (0, 1.0f, aspect, 0, -1.0f, 1.0f);
//            } else {
//                GL.Ortho (0, aspect, 1.0f, 0, -1.0f, 1.0f);
//            }
//        }
//        #endregion
//    }
//}
