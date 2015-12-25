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

	virtual string ReadFile (const string& fileName) const override;
	virtual void WriteFile (const string& fileName, const string& content) override;

private:
	jobject openAsset (const string& asset) const;
	void closeStream (jobject istream) const;
	jobject loadBitmap (const string& asset) const;
};
