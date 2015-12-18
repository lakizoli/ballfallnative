#pragma once

typedef void* Image;

/// The interface of the OS specific content managers.
class IContentManager {
public:
	virtual Image LoadImage (const string& asset) = 0;
	virtual const uint8_t* LockPixels (Image image) = 0;
	virtual void UnlockPixels (Image image) = 0;
	virtual int GetWidth (Image image) = 0;
	virtual int GetHeight (Image image) = 0;

	virtual void SetTopLeftStyle (float size, float red, float green, float blue, float alpha) = 0;
	virtual void SetTopRightStyle (float size, float red, float green, float blue, float alpha) = 0;

	virtual void SetTopLeftStatus (const string& text) = 0;
	virtual void SetTopRightStatus (const string& text) = 0;
};
