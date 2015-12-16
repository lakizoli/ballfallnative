#pragma once

#include "vector2D.h"
#include "rect2D.h"

/// Base class for 2D meshes.
class Mesh2D {
//Data
public:
	Vector2D Pos;
	float Rotation;
	Vector2D Scale;

protected:
	Rect2D mBoundingBox;

//Construction
protected:
	Mesh2D () : Rotation (0.0f) {}

//Interface
public:
	virtual void Init ();
	virtual void Shutdown ();
	void Render ();

protected:
    virtual void RenderMesh () = 0;

//Data
public:
	const Rect2D& BoundingBox () const {
		return mBoundingBox;
	}

	Rect2D TransformedBoundingBox () const {
		return mBoundingBox.Scale (Scale).Offset (Pos);
	}

//Helper methods
protected:
    //protected int LoadTextureFromAsset (string asset) {
    //    int texID = 0;
    //    GL.GenTextures (1, ref texID);
    //    GL.BindTexture (All.Texture2D, texID);

    //    using (var image = Game.ContentManager.LoadImage (asset)) {
    //        IntPtr pixels = Game.ContentManager.LockPixels (image);
    //        GL.TexImage2D (All.Texture2D, 0, (int)All.Rgba, Game.ContentManager.GetWidth (image), Game.ContentManager.GetHeight (image), 0, All.Rgba, All.UnsignedByte, pixels);
    //        Game.ContentManager.UnlockPixels (image);
    //    }

    //    GL.TexParameter (All.Texture2D, All.TextureMinFilter, (int)All.Linear);
    //    GL.TexParameter (All.Texture2D, All.TextureMagFilter, (int)All.Linear);

    //    return texID;
    //}

	Rect2D CalculateBoundingBox (const vector<float>& vertices) const;

	int NewVBO (const vector<float>& data) const;
	vector<int> NewTexturedVBO (int texID, const vector<float>& vertices = vector<float> (), const vector<float>& texCoords = vector<float> ());

	void RenderTexturedVBO (int texID, int vertCoordID, int texCoordID, GLenum mode = GL_TRIANGLE_STRIP, int vertexCount = 4) const;
};