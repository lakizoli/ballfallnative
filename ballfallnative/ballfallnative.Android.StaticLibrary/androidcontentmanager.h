#pragma once

#include "management/IContentManager.h"

class AndroidContentManager : public IContentManager {
public:
	AndroidContentManager (JavaVM* vm, jobject activity);

	virtual Image LoadImage (const string & asset) override;
	virtual void UnloadImage (Image& image) override;

	virtual const uint8_t * LockPixels (Image image) override;
	virtual void UnlockPixels (Image image) override;
	virtual int GetWidth (const Image image) const override;
	virtual int GetHeight (const Image image) const override;

	virtual void InitAdMob () const override;

	virtual void SetTopLeftStyle (float size, float red, float green, float blue, float alpha) override;
	virtual void SetTopRightStyle (float size, float red, float green, float blue, float alpha) override;

	virtual void SetTopLeftStatus (const string & text) override;
	virtual void SetTopRightStatus (const string & text) override;

	virtual int PlaySound (const string& asset, bool looped) override;
	virtual bool IsSoundEnded (int soundID) const override;
	virtual void RemoveEndedSoundID (int soundID) override;

	virtual string ReadFile (const string& fileName) const override;
	virtual void WriteFile (const string& fileName, const string& content) override;

private:
	jobject openAsset (const string& asset) const;
	void closeStream (jobject istream) const;
	jobject loadBitmap (const string& asset) const;

public:
	static void AddCompletedSoundID (int soundID) {
		completedSoundIDs.insert (soundID);
	}

	static bool IsSoundCompleted (int soundID) {
		return completedSoundIDs.find (soundID) != completedSoundIDs.end ();
	}

	static void RemoveCompletedSoundID (int soundID) {
		auto it = completedSoundIDs.find (soundID);
		if (it != completedSoundIDs.end ())
			completedSoundIDs.erase (it);
	}

private:
	static set<int> completedSoundIDs;
};
