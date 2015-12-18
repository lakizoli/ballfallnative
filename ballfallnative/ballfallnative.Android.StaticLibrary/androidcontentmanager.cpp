#include "pch.h"
#include "androidcontentmanager.h"
#include "JavaString.h"

#include <android/asset_manager.h>
#include <android/bitmap.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
// JNI helper for AndroidContentManager class
////////////////////////////////////////////////////////////////////////////////////////////////////
class JNI_ContentManager {
	friend class AndroidContentManager;

	JNI_ContentManager () :
		mClassActivity (nullptr),
		mClassAsset (nullptr),
		mClassInputStream (nullptr),
		mClassBitmapFactory (nullptr),
		mGetAssetsMethod (nullptr),
		mOpenMethod (nullptr),
		mCloseMethod (nullptr),
		mDecodeStreamMethod (nullptr),
		mActivity (nullptr) {
	}

	~JNI_ContentManager () {
		Release ();
	}

	static JNI_ContentManager &Get (bool checkInit = true) {
		static JNI_ContentManager inst;
		if (checkInit && !inst.IsInited ())
			inst.Init ();
		return inst;
	}

	static void ReinitJNI () {
		JNI_ContentManager &jni = JNI_ContentManager::Get (); //Get reference for jni interface
		jni.Release (); //Release all of the stored interface data
		jni.Init (); //Reinitialize jni interface
	}

	bool IsInited () const {
		return mClassActivity != nullptr;
	}

	void Init () {
		if (IsInited ())
			return;

		//Find and reference Java class and collect needed methods
		JNI::AutoLocalRef <jclass> clazzActivity (JNI::FindClass ("android/app/Activity"));
		mClassActivity = JNI::GlobalReferenceObject (clazzActivity.get (), "android/app/Activity");

		JNI::AutoLocalRef <jclass> clazzAsset (JNI::FindClass ("android/content/res/AssetManager"));
		mClassAsset = JNI::GlobalReferenceObject (clazzAsset.get (), "android/content/res/AssetManager");

		JNI::AutoLocalRef <jclass> clazzInputStream (JNI::FindClass ("java/io/InputStream"));
		mClassInputStream = JNI::GlobalReferenceObject (clazzInputStream.get (), "java/io/InputStream");

		JNI::AutoLocalRef <jclass> clazzBitmapFactory (JNI::FindClass ("android/graphics/BitmapFactory"));
		mClassBitmapFactory = JNI::GlobalReferenceObject (clazzBitmapFactory.get (), "android/graphics/BitmapFactory");

		//Collect needed methods
		mGetAssetsMethod = JNI::GetMethod (clazzActivity, "getAssets", "()Landroid/content/res/AssetManager;");
		mOpenMethod = JNI::GetMethod (clazzAsset, "open", "(Ljava/lang/String;)Ljava/io/InputStream;");
		mCloseMethod = JNI::GetMethod (clazzInputStream, "close", "()V");
		mDecodeStreamMethod = JNI::GetStaticMethod (clazzBitmapFactory, "decodeStream", "(Ljava/io/InputStream;)Landroid/graphics/Bitmap;");
	}

	void Release () {
		if (!IsInited ())
			return;

		JNI::ReleaseGlobalReferencedObject (mClassBitmapFactory, "JNI_ContentManager::Release () - BitmapFactory");
		mClassBitmapFactory = nullptr;

		JNI::ReleaseGlobalReferencedObject (mClassInputStream, "JNI_ContentManager::Release () - InputStream");
		mClassInputStream = nullptr;

		JNI::ReleaseGlobalReferencedObject (mClassAsset, "JNI_ContentManager::Release () - Asset");
		mClassAsset = nullptr;

		JNI::ReleaseGlobalReferencedObject (mClassActivity, "JNI_ContentManager::Release () - Activity");
		mClassActivity = nullptr;

		mGetAssetsMethod = nullptr;
		mOpenMethod = nullptr;
		mCloseMethod = nullptr;
		mDecodeStreamMethod = nullptr;
	}

	//private:
	jclass mClassActivity; ///< The java class: android/app/Activity
	jclass mClassAsset; ///< The java class: android/content/res/AssetManager
	jclass mClassInputStream; ///< The java class: java/io/InputStream
	jclass mClassBitmapFactory; ///< The java class: android/graphics/BitmapFactory

	jmethodID mGetAssetsMethod; ///< The getAssets () method.
	jmethodID mOpenMethod; ///< The open () method.
	jmethodID mCloseMethod; ///< The close () method.
	jmethodID mDecodeStreamMethod; ///< The static decodeStream () method.

	jobject mActivity; ///< The java instance of the Activity.
};

////////////////////////////////////////////////////////////////////////////////////////////////////
// AndroidContentManager implementation
////////////////////////////////////////////////////////////////////////////////////////////////////
AndroidContentManager::AndroidContentManager (JavaVM * vm, jobject activity) {
	CHECKMSG (vm != nullptr, "Java virtual machine reference cannot be nullptr!");
	CHECKMSG (activity != nullptr, "Activity reference cannot be nullptr!");

	//Init JNI layer (only once at all)
	if (!JNI::IsInited ())
		JNI::Init (vm);

	JNI_ContentManager& jni = JNI_ContentManager::Get ();
	jni.mActivity = activity;
}

inline Image AndroidContentManager::LoadImage (const string & asset) {
	return (Image) loadBitmap (asset);
}

const uint8_t * AndroidContentManager::LockPixels (Image image) {
	void* ptr = nullptr;
	AndroidBitmap_lockPixels (JNI::GetEnv (), (jobject) image, &ptr);
	return (const uint8_t *) ptr;
}

void AndroidContentManager::UnlockPixels (Image image) {
	AndroidBitmap_unlockPixels (JNI::GetEnv (), (jobject) image);
}

int AndroidContentManager::GetWidth (Image image) {
	AndroidBitmapInfo info;
	AndroidBitmap_getInfo (JNI::GetEnv (), (jobject) image, &info);
	return (int) info.width;
}

int AndroidContentManager::GetHeight (Image image) {
	AndroidBitmapInfo info;
	AndroidBitmap_getInfo (JNI::GetEnv (), (jobject) image, &info);
	return (int) info.height;
}

void AndroidContentManager::SetTopLeftStyle (float size, float red, float green, float blue, float alpha) {
	//TODO: ...
}

void AndroidContentManager::SetTopRightStyle (float size, float red, float green, float blue, float alpha) {
	//TODO: ...
}

void AndroidContentManager::SetTopLeftStatus (const string & text) {
	//TODO: ...
}

void AndroidContentManager::SetTopRightStatus (const string & text) {
	//TODO: ...
}

jobject AndroidContentManager::openAsset (const string & asset) const {
	JNI_ContentManager& jni = JNI_ContentManager::Get ();
	JNIEnv* env = JNI::GetEnv ();

	JNI::AutoLocalRef <jobject> assets (env->CallObjectMethod (jni.mActivity, jni.mGetAssetsMethod));
	CHECKMSG (assets.get () != nullptr, "AndroidContentManager::openAsset () - assets cannot be nullptr!");

	jobject istream = env->CallObjectMethod (assets, jni.mOpenMethod, JavaString (asset).get ());
	if (istream == nullptr) {
		stringstream ss;
		ss << "Unable to open asset: " << asset;
		JNI::ThrowException ("java/lang/Exception", ss.str ());
	}

	return istream;
}

void AndroidContentManager::closeStream (jobject istream) const {
	JNI_ContentManager& jni = JNI_ContentManager::Get ();
	JNIEnv* env = JNI::GetEnv ();
	env->CallVoidMethod (istream, jni.mCloseMethod);
}

jobject AndroidContentManager::loadBitmap (const string & asset) const {
	JNI_ContentManager& jni = JNI_ContentManager::Get ();
	JNIEnv* env = JNI::GetEnv ();

	JNI::AutoLocalRef <jobject> istream (openAsset (asset));
	CHECKMSG (istream.get () != nullptr, "AndroidContentManager::LoadBitmap () - istream cannot be nullptr!");

	jobject bitmap = env->CallStaticObjectMethod (jni.mClassBitmapFactory, jni.mDecodeStreamMethod, istream.get ());
	CHECKMSG (bitmap != nullptr, "AndroidContentManager::LoadBitmap () - bitmap cannot be nullptr!");

	closeStream (istream);

	return bitmap;
}
