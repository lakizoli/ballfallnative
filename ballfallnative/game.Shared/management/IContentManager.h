#pragma once

/// The interface of the OS specific content managers.
class IContentManager {
public:
	//IDisposable LoadImage (string asset);
	//IntPtr LockPixels (object image);
	//void UnlockPixels (object image);
	//int GetWidth (object image);
	//int GetHeight (object image);

	virtual void SetTopLeftStyle (float size, float red, float green, float blue, float alpha) = 0;
	virtual void SetTopRightStyle (float size, float red, float green, float blue, float alpha) = 0;

	virtual void SetTopLeftStatus (const string& text) = 0;
	virtual void SetTopRightStatus (const string& text) = 0;
};
