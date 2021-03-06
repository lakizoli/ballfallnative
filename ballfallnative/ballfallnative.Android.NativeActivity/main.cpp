/*
* Copyright (C) 2010 The Android Open Source Project
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*      http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
*/

#include "../ballfallnative.Shared/pch.h"
#include "../ballfallnative.Shared/management/ballfallgame.h"
#include "../ballfallnative.Android.StaticLibrary/androidutil.h"
#include "../ballfallnative.Android.StaticLibrary/androidcontentmanager.h"

#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "ballfallnative.NativeActivity", __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "ballfallnative.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "ballfallnative.NativeActivity", __VA_ARGS__))

#define CHECK_GL_ERROR { GLenum err = glGetError (); if (err != GL_NO_ERROR) { LOGW ("!!!!!! opengl error occured !!!!!! error code: %d, file: %s, line: %d", (int)err, __FILE__, __LINE__); } }

/**
* Our saved state data.
*/
struct saved_state {
	float angle;
	int32_t x;
	int32_t y;
};

/**
* Shared state for our app.
*/
struct engine {
	struct android_app* app;

	ASensorManager* sensorManager;
	const ASensor* accelerometerSensor;
	ASensorEventQueue* sensorEventQueue;

	int animating;
	EGLDisplay display;
	EGLSurface surface;
	EGLContext context;
	int32_t width;
	int32_t height;
	struct saved_state state;

	std::unique_ptr<AndroidUtil> util;
	std::unique_ptr<AndroidContentManager> contentManager;
	std::unique_ptr<BallFallGame> game;
	std::unique_ptr<std::set<int32_t>> pointerIDs;
	double lastUpdateTime;
};

static double engine_get_current_time () {
	timespec now;
	clock_gettime (CLOCK_MONOTONIC, &now);
	return (double) now.tv_sec + (double) now.tv_nsec / 1e9;
}

/**
* Initialize an EGL context for the current display.
*/
static int engine_init_display(struct engine* engine) {
	// initialize OpenGL ES and EGL

	/*
	* Here specify the attributes of the desired configuration.
	* Below, we select an EGLConfig with at least 8 bits per color
	* component compatible with on-screen windows
	*/
	const EGLint attribs[] = {
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_STENCIL_SIZE, 8,
        EGL_NONE
	};
	EGLint w, h, format;
	EGLint numConfigs;
	EGLConfig config;
	EGLSurface surface;
	EGLContext context;

	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

	eglInitialize(display, 0, 0);

	/* Here, the application chooses the configuration it desires. In this
	* sample, we have a very simplified selection process, where we pick
	* the first EGLConfig that matches our criteria */
	eglChooseConfig(display, attribs, &config, 1, &numConfigs);

	/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
	* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
	* As soon as we picked a EGLConfig, we can safely reconfigure the
	* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
	eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

	ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

	surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);

	EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 1, EGL_NONE };
	context = eglCreateContext(display, config, NULL, contextAttribs);

	if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
		LOGW("Unable to eglMakeCurrent");
		return -1;
	}

	eglQuerySurface(display, surface, EGL_WIDTH, &w);
	eglQuerySurface(display, surface, EGL_HEIGHT, &h);

	engine->display = display;
	engine->context = context;
	engine->surface = surface;
	engine->width = w;
	engine->height = h;
	engine->state.angle = 0;

	if (!engine->pointerIDs) {
		engine->pointerIDs.reset (new std::set<int32_t> ());
	}

	if (!engine->util) {
		engine->util.reset (new AndroidUtil ());
	}

	if (!engine->contentManager) {
		engine->contentManager.reset (new AndroidContentManager (engine->app->activity->vm, engine->app->activity->clazz, engine->app->activity->assetManager));
	}

	if (!engine->game) {
		engine->game.reset(new BallFallGame (*(engine->util), *(engine->contentManager)));
		engine->game->Init (w, h, 1920, 1080);

		engine->animating = 1;
	}

	return 0;
}

/**
* Just the current frame in the display.
*/
static void engine_draw_frame(struct engine* engine) {
	//Handle no display
	if (engine->display == NULL) {
		// No display.
		return;
	}

	//Handle focus loss, or shutdown
	if (engine->animating == 0) {
		if (engine->game) {
			engine->game->Shutdown ();
			engine->game.reset ();
		}

		if (engine->contentManager) {
			engine->contentManager.reset ();
		}

		if (engine->util) {
			engine->util.reset ();
		}

		if (engine->pointerIDs) {
			engine->pointerIDs.reset ();
		}

		engine->lastUpdateTime = -1;
		return;
	}

	//Render the game
	double elapsedTime = 0;
	double currentTime = engine_get_current_time ();
	if (engine->lastUpdateTime >= 0)
		elapsedTime = currentTime - engine->lastUpdateTime;
	engine->lastUpdateTime = currentTime;

	engine->game->Update ((float)elapsedTime);
	engine->game->Render ();

	eglSwapBuffers(engine->display, engine->surface);
}

/**
* Tear down the EGL context currently associated with the display.
*/
static void engine_term_display(struct engine* engine) {
	if (engine->display != EGL_NO_DISPLAY) {
		eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		if (engine->context != EGL_NO_CONTEXT) {
			eglDestroyContext(engine->display, engine->context);
		}
		if (engine->surface != EGL_NO_SURFACE) {
			eglDestroySurface(engine->display, engine->surface);
		}
		eglTerminate(engine->display);
	}
	engine->animating = 0;
	engine->display = EGL_NO_DISPLAY;
	engine->context = EGL_NO_CONTEXT;
	engine->surface = EGL_NO_SURFACE;
}

/**
* Process the next input event.
*/
static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
	struct engine* engine = (struct engine*)app->userData;
	if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
		//engine->state.x = AMotionEvent_getX(event, 0);
		//engine->state.y = AMotionEvent_getY(event, 0);
		//return 1;

		int32_t action = AMotionEvent_getAction (event);
		int32_t pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
		//LOGD ("OnTouchEvent (Raw) -> action: %d, idx: %d, id: %d, x: %.2f, y: %.2f", action, pointerIndex, AMotionEvent_getPointerId (event, pointerIndex), AMotionEvent_getX (event, pointerIndex), AMotionEvent_getY (event, pointerIndex));
		if (action == AMOTION_EVENT_ACTION_DOWN || (action & AMOTION_EVENT_ACTION_POINTER_DOWN) == AMOTION_EVENT_ACTION_POINTER_DOWN) {
			int32_t id = AMotionEvent_getPointerId (event, pointerIndex);
			if (engine->pointerIDs && engine->pointerIDs->find (id) == engine->pointerIDs->end ()) {
				//LOGD ("OnTouchEvent (Down) -> idx: %d, id: %d, x: %.2f, y: %.2f", pointerIndex, id, AMotionEvent_getX (event, pointerIndex), AMotionEvent_getY (event, pointerIndex));
				engine->pointerIDs->insert (id);
				engine->game->TouchDown (id, AMotionEvent_getX (event, pointerIndex), AMotionEvent_getY (event, pointerIndex));
			}
		} else if (action == AMOTION_EVENT_ACTION_UP || (action & AMOTION_EVENT_ACTION_POINTER_UP) == AMOTION_EVENT_ACTION_POINTER_UP) {
			int32_t id = AMotionEvent_getPointerId (event, pointerIndex);
			if (engine->pointerIDs && engine->pointerIDs->find (id) != engine->pointerIDs->end ()) {
				//LOGD ("OnTouchEvent (Up) -> idx: %d, id: %d, x: %.2f, y: %.2f", pointerIndex, id, AMotionEvent_getX (event, pointerIndex), AMotionEvent_getY (event, pointerIndex));
				engine->pointerIDs->erase (id);
				engine->game->TouchUp (id, AMotionEvent_getX (event, pointerIndex), AMotionEvent_getY (event, pointerIndex));
			}
		} else if (action == AMOTION_EVENT_ACTION_MOVE) {
			size_t pointerCount = AMotionEvent_getPointerCount (event);
			for (size_t i = 0; i < pointerCount; ++i) {
				int32_t id = AMotionEvent_getPointerId (event, i);
				if (engine->pointerIDs && engine->pointerIDs->find (id) != engine->pointerIDs->end ()) {
					//LOGD ("OnTouchEvent (Move) -> idx: %d, id: %d, x: %.2f, y: %.2f", pointerIndex, id, AMotionEvent_getX (event, pointerIndex), AMotionEvent_getY (event, pointerIndex));
					engine->game->TouchMove (id, AMotionEvent_getX (event, i), AMotionEvent_getY (event, i));
				}
			}
		}

		return 1;
	}
	return 0;
}

/**
* Process the next main command.
*/
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
	struct engine* engine = (struct engine*)app->userData;
	switch (cmd) {
	case APP_CMD_SAVE_STATE:
		// The system has asked us to save our current state.  Do so.
		engine->app->savedState = malloc(sizeof(struct saved_state));
		*((struct saved_state*)engine->app->savedState) = engine->state;
		engine->app->savedStateSize = sizeof(struct saved_state);
		break;
	case APP_CMD_INIT_WINDOW:
		// The window is being shown, get it ready.
		if (engine->app->window != NULL) {
			engine_init_display(engine);
			engine_draw_frame(engine);
		}
		break;
	case APP_CMD_TERM_WINDOW:
		// The window is being hidden or closed, clean it up.
		engine_term_display(engine);
		break;
	case APP_CMD_GAINED_FOCUS:
		// When our app gains focus, we start monitoring the accelerometer.
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_enableSensor(engine->sensorEventQueue,
				engine->accelerometerSensor);
			// We'd like to get 60 events per second (in us).
			ASensorEventQueue_setEventRate(engine->sensorEventQueue,
				engine->accelerometerSensor, (1000L / 60) * 1000);
		}
		break;
	case APP_CMD_LOST_FOCUS:
		// When our app loses focus, we stop monitoring the accelerometer.
		// This is to avoid consuming battery while not being used.
		if (engine->accelerometerSensor != NULL) {
			ASensorEventQueue_disableSensor(engine->sensorEventQueue,
				engine->accelerometerSensor);
		}
		// Also stop animating.
		engine->animating = 0;
		engine_draw_frame(engine);
		break;
	//case APP_CMD_CONFIG_CHANGED:
	//	if (engine->app->window != NULL && engine->game) {
	//		int32_t width = ANativeWindow_getWidth (engine->app->window);
	//		int32_t height = ANativeWindow_getHeight (engine->app->window);
	//		glViewport (0, 0, width, height);
	//		//engine->game->Resize (width, height);
	//	}
		break;
	}
}

/**
* This is the main entry point of a native application that is using
* android_native_app_glue.  It runs in its own thread, with its own
* event loop for receiving input events and doing other things.
*/
void android_main(struct android_app* state) {
	struct engine engine;

	memset(&engine, 0, sizeof(engine));
	state->userData = &engine;
	state->onAppCmd = engine_handle_cmd;
	state->onInputEvent = engine_handle_input;
	engine.app = state;

	// Prepare to monitor accelerometer
	engine.sensorManager = ASensorManager_getInstance();
	//engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
	//	ASENSOR_TYPE_ACCELEROMETER);
	engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
		state->looper, LOOPER_ID_USER, NULL, NULL);

	if (state->savedState != NULL) {
		// We are starting with a previous saved state; restore from it.
		engine.state = *(struct saved_state*)state->savedState;
	}

	engine.animating = 1;
	engine.lastUpdateTime = -1;

	// loop waiting for stuff to do.

	while (1) {
		// Read all pending events.
		int ident;
		int events;
		struct android_poll_source* source;

		// If not animating, we will block forever waiting for events.
		// If animating, we loop until all events are read, then continue
		// to draw the next frame of animation.
		while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events,
			(void**)&source)) >= 0) {

			// Process this event.
			if (source != NULL) {
				source->process(state, source);
			}

			// If a sensor has data, process it now.
			if (ident == LOOPER_ID_USER) {
				if (engine.accelerometerSensor != NULL) {
					ASensorEvent event;
					while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
						&event, 1) > 0) {
						LOGI("accelerometer: x=%f y=%f z=%f",
							event.acceleration.x, event.acceleration.y,
							event.acceleration.z);
					}
				}
			}

			// Check if we are exiting.
			if (state->destroyRequested != 0) {
				engine_term_display(&engine);
				return;
			}
		}

		if (engine.animating) {
			// Done with events; draw next animation frame.
			engine.state.angle += .01f;
			if (engine.state.angle > 1) {
				engine.state.angle = 0;
			}

			// Drawing is throttled to the screen update rate, so there
			// is no need to do timing here.
			engine_draw_frame(&engine);
		}
	}
}
