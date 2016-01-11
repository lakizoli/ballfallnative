#include "pch.h"
#include "audiomanager.h"
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include "jniload.h"

AudioManager::Player::Player (int soundID) :
	soundID (soundID),
	looped (false),
	ended (false),
	player (nullptr),
	play (nullptr),
	seek (nullptr),
	volume (nullptr) {
}

AudioManager::Player::~Player () {
	if (play != nullptr) {
		(*play)->SetPlayState (play, SL_PLAYSTATE_STOPPED);
	}

	if (player != nullptr) {
		(*player)->Destroy (player);
	}

	soundID = 0;
	looped = false;
	ended = false;
	player = nullptr;
	play = nullptr;
	seek = nullptr;
	volume = nullptr;
}

int AudioManager::mNextID = 1;

AudioManager::AudioManager () :
	mInited (false),
	mEngineObject (nullptr),
	mEngine (nullptr),
	mOutputMixObject (nullptr),
	mAssetManager (nullptr) {
}

AudioManager::~AudioManager () {
	Shutdown ();
}

bool AudioManager::Init (AAssetManager * assetManager) {
	if (mInited)
		return true;

	CHECKMSG (assetManager != nullptr, "AudioManager::Init () - assetManager cannot be nullptr!");
	mAssetManager = assetManager;

	SLresult result;
	result = slCreateEngine (&mEngineObject, 0, NULL, 0, NULL, NULL);
	CHECKMSG (result == SL_RESULT_SUCCESS && mEngineObject != nullptr, "AudioManager::Init () - slCreateEngine () failed");

	result = (*mEngineObject)->Realize (mEngineObject, SL_BOOLEAN_FALSE);
	CHECKMSG (result == SL_RESULT_SUCCESS, "AudioManager::Init () - EngineObject::Realize () failed");

	result = (*mEngineObject)->GetInterface (mEngineObject, SL_IID_ENGINE, &mEngine);
	CHECKMSG (result == SL_RESULT_SUCCESS && mEngine != nullptr, "AudioManager::Init () - GetInterface () -> Engine failed");

	result = (*mEngine)->CreateOutputMix (mEngine, &mOutputMixObject, 0, NULL, NULL);
	CHECKMSG (result == SL_RESULT_SUCCESS && mOutputMixObject != nullptr, "AudioManager::Init () - CreateOutputMix () failed");

	result = (*mOutputMixObject)->Realize (mOutputMixObject, SL_BOOLEAN_FALSE);
	CHECKMSG (result == SL_RESULT_SUCCESS, "AudioManager::Init () - OutputMixObject->Realize () failed");

	mInited = true;
	return mInited;
}

void AudioManager::Shutdown () {
	if (!mInited)
		return;

	for (auto& it : mPlayers)
		delete it.second;
	mPlayers.clear ();

	mAssetManager = nullptr;

	CHECKMSG (mOutputMixObject != nullptr, "AudioManager::Shutdown () - mOutputMixObject cannot be nullptr!");
	(*mOutputMixObject)->Destroy (mOutputMixObject);
	mOutputMixObject = nullptr;

	CHECKMSG (mEngine != nullptr, "AudioManager::Shutdown () - mEngine cannot be nullptr!");
	mEngine = nullptr;

	CHECKMSG (mEngineObject != nullptr, "AudioManager::Shutdown () - mEngineObject cannot be nullptr!");
	(*mEngineObject)->Destroy (mEngineObject);
	mEngineObject = nullptr;

	mInited = false;
}

int AudioManager::Load (const string & assetName) {
	CHECKMSG (mInited, "AudioManager::Load () - Can be called only after Init ()!");

	AAsset* asset = AAssetManager_open (mAssetManager, assetName.c_str (), AASSET_MODE_UNKNOWN);
	CHECKMSG (asset != nullptr, "AudioManager::Load () - AAssetManager_open () returns nullptr!");

	off_t start = 0;
	off_t length = 0;
	int fd = AAsset_openFileDescriptor (asset, &start, &length);
	CHECKMSG (fd > 0, "AudioManager::Load () - AAsset_openFileDescriptor () returns invalid file descriptor!");

	AAsset_close (asset);
	asset = nullptr;

	// configure audio source
	SLDataLocator_AndroidFD loc_fd = {
		SL_DATALOCATOR_ANDROIDFD,
		fd,
		start,
		length
	};

	SLDataFormat_MIME format_mime = {
		SL_DATAFORMAT_MIME,
		NULL,
		SL_CONTAINERTYPE_UNSPECIFIED
	};

	SLDataSource audioSrc = {
		&loc_fd,
		&format_mime
	};

	// configure audio sink
	SLDataLocator_OutputMix loc_outmix = {
		SL_DATALOCATOR_OUTPUTMIX,
		mOutputMixObject
	};

	SLDataSink audioSnk = {
		&loc_outmix,
		NULL
	};

	// allocate played sound instance
	struct Player* player = new struct Player (mNextID++);

	// create audio player
	const unsigned int NUM_INTERFACES = 3;
	const SLInterfaceID ids[NUM_INTERFACES] = { SL_IID_PLAY, SL_IID_SEEK, SL_IID_VOLUME };
	const SLboolean req[NUM_INTERFACES] = { SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE };

	SLresult result = (*mEngine)->CreateAudioPlayer (mEngine, &player->player, &audioSrc, &audioSnk, NUM_INTERFACES, ids, req);
	CHECKMSG (result == SL_RESULT_SUCCESS && player->player != nullptr, "AudioManager::Load () - CreateAudioPlayer () failed");

	// realize the player
	result = (*player->player)->Realize (player->player, SL_BOOLEAN_FALSE);
	CHECKMSG (result == SL_RESULT_SUCCESS, "AudioManager::Load () - Player::Realize () failed");

	// get the play interface
	result = (*player->player)->GetInterface (player->player, SL_IID_PLAY, &player->play);
	CHECKMSG (result == SL_RESULT_SUCCESS && player->play != nullptr, "AudioManager::Load () - Player::GetInterface (SL_IID_PLAY) failed");

	// get the seek interface
	result = (*player->player)->GetInterface (player->player, SL_IID_SEEK, &player->seek);
	CHECKMSG (result == SL_RESULT_SUCCESS && player->seek != nullptr, "AudioManager::Load () - Player::GetInterface (SL_IID_SEEK) failed");

	// get the volume interface
	result = (*player->player)->GetInterface (player->player, SL_IID_VOLUME, &player->volume);
	CHECKMSG (result == SL_RESULT_SUCCESS && player->volume != nullptr, "AudioManager::Load () - Player::GetInterface (SL_IID_VOLUME) failed");

	(*player->play)->RegisterCallback (player->play, AudioManager::PlayCallback, player);
	(*player->play)->SetCallbackEventsMask (player->play, SL_PLAYEVENT_HEADATEND);

	mPlayers[player->soundID] = player;
	return player->soundID;
}

void AudioManager::Unload (int soundID) {
	CHECKMSG (mInited, "AudioManager::Unload () - Can be called only after Init ()!");

	auto it = mPlayers.find (soundID);
	if (it == mPlayers.end ())
		return;

	delete it->second;
	mPlayers.erase (it);
}

void AudioManager::Play (int soundID, float volume, bool looped) {
	CHECKMSG (mInited, "AudioManager::Play () - Can be called only after Init ()!");

	auto it = mPlayers.find (soundID);
	if (it == mPlayers.end ())
		return;

	struct Player* player = (struct Player*)it->second;
	if (player == nullptr)
		return;

	player->looped = looped;
	player->ended = false;

	if (player->play != nullptr) {
		(*player->play)->SetPlayState (player->play, SL_PLAYSTATE_STOPPED);
	}

	if (looped && player->seek != nullptr) {
		(*player->seek)->SetLoop (player->seek, SL_BOOLEAN_TRUE, 0, SL_TIME_UNKNOWN);
	}

	if (player->volume != nullptr) {
		SLmillibel maxLevel = 0;
		(*player->volume)->GetMaxVolumeLevel (player->volume, &maxLevel);

		// Convert UI volume to linear factor (cube)
		float vol = volume * volume * volume;

		// millibels from linear amplification
		int level = lroundf (2000.f * log10f (vol));
		if (level < SL_MILLIBEL_MIN)
			level = SL_MILLIBEL_MIN;
		else if (level > (int)maxLevel)
			level = (int) maxLevel;

		(*player->volume)->SetVolumeLevel (player->volume, (SLmillibel) level);
	}

	if (player->play != nullptr) {
		(*player->play)->SetPlayState (player->play, SL_PLAYSTATE_PLAYING);
	}
}

void AudioManager::Stop (int soundID) {
	CHECKMSG (mInited, "AudioManager::Stop () - Can be called only after Init ()!");

	auto it = mPlayers.find (soundID);
	if (it == mPlayers.end ())
		return;

	struct Player* player = (struct Player*)it->second;
	if (player == nullptr)
		return;

	SLPlayItf play = player->play;
	if (play != nullptr) {
		(*play)->SetPlayState (play, SL_PLAYSTATE_STOPPED);
	}

	player->looped = false;
	player->ended = true;
}

bool AudioManager::IsEnded (int soundID) const {
	CHECKMSG (mInited, "AudioManager::IsEnded () - Can be called only after Init ()!");

	auto it = mPlayers.find (soundID);
	if (it == mPlayers.end ())
		return true;

	struct Player* player = (struct Player*)it->second;
	if (player == nullptr)
		return true;

	return player->ended;
}

void SLAPIENTRY AudioManager::PlayCallback (SLPlayItf play, void *context, SLuint32 event) {
	CHECKMSG (play != nullptr, "play_callback () - play cannot be nullptr!");
	CHECKMSG (context != nullptr, "play_callback () - context cannot be nullptr!");

	if (event & SL_PLAYEVENT_HEADATEND) {
		(*play)->SetPlayState (play, SL_PLAYSTATE_STOPPED);

		struct Player* player = (struct Player*) context;
		if (player != nullptr && !player->looped) { //Register sound end when not looped
			player->ended = true;
		}
	}
}
