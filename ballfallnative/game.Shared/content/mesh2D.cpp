#include "pch.h"
#include "mesh2D.h"
#include "management/game.h"

void Mesh2D::Init ()
{
    Scale = Vector2D (1, 1);
}

void Mesh2D::Shutdown ()
{
    Pos = Vector2D ();
    Rotation = 0;
    Scale = Vector2D (1, 1);
}

void Mesh2D::Render ()
{
	Game& game = Game::Get ();

	glm::mat4& matrix = game.PushMatrix ();
	matrix = glm::translate (matrix, glm::vec3 (Pos.x, Pos.y, 0.0f));
	matrix = glm::rotate (matrix, Rotation, glm::vec3 (0.0f, 0.0f, 1.0f));
	matrix = glm::scale (matrix, glm::vec3 (Scale.x, Scale.y, 1.0f));

    RenderMesh ();

    game.PopMatrix ();
}

//        #region Helper methods
//        protected int LoadTextureFromAsset (string asset) {
//            int texID = 0;
//            GL.GenTextures (1, ref texID);
//            GL.BindTexture (All.Texture2D, texID);
//
//            using (var image = Game.ContentManager.LoadImage (asset)) {
//                IntPtr pixels = Game.ContentManager.LockPixels (image);
//                GL.TexImage2D (All.Texture2D, 0, (int)All.Rgba, Game.ContentManager.GetWidth (image), Game.ContentManager.GetHeight (image), 0, All.Rgba, All.UnsignedByte, pixels);
//                Game.ContentManager.UnlockPixels (image);
//            }
//
//            GL.TexParameter (All.Texture2D, All.TextureMinFilter, (int)All.Linear);
//            GL.TexParameter (All.Texture2D, All.TextureMagFilter, (int)All.Linear);
//
//            return texID;
//        }

Rect2D Mesh2D::CalculateBoundingBox (const vector<float>& vertices) const {
    Vector2D min (FLT_MAX, FLT_MAX);
    Vector2D max (-FLT_MAX, -FLT_MAX);

    for (int i = 0; i < vertices.size (); i += 2) {
        Vector2D pt (vertices[i], vertices[i+1]);
        if (pt < min)
            min = pt;

        if (pt > max)
            max = pt;
    }

    return Rect2D (min, max);
}

//        protected int NewVBO<T> (T[] data) where T : struct  {
//            int vboID = 0;
//            GL.GenBuffers (1, ref vboID);
//
//            GL.BindBuffer (All.ArrayBuffer, vboID);
//            GL.BufferData<T> (All.ArrayBuffer, (IntPtr)(data.Length * Marshal.SizeOf (typeof (T))), data, All.StaticDraw);
//
//            return vboID;
//        }
//
//        protected int[] NewTexturedVBO (int texID, float[] vertices = null, float[] texCoords = null) {
//            if (vertices == null) {
//                vertices = new float[] {
//                    -1.0f, -1.0f,
//                    1.0f, -1.0f,
//                    -1.0f, 1.0f,
//                    1.0f, 1.0f
//                };
//            }
//
//            BoundingBox = CalculateBoundingBox (vertices);
//
//            return new int[] {
//                NewVBO<float> (vertices),
//                NewVBO<float> (texCoords == null ? new float[] {
//                    0.0f, 0.0f,
//                    1.0f, 0.0f,
//                    0.0f, 1.0f,
//                    1.0f, 1.0f
//                } : texCoords)
//            };
//        }

void Mesh2D::RenderTexturedVBO (int texID, int vertCoordID, int texCoordID, GLenum mode, int vertexCount) const {
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable (GL_TEXTURE_2D);
    glBindTexture (GL_TEXTURE_2D, texID);

    //GL.BindBuffer (All.ArrayBuffer, vertCoordID);
    //GL.VertexPointer (2, All.Float, 0, IntPtr.Zero);
    //GL.EnableClientState (All.VertexArray);

    //GL.BindBuffer (All.ArrayBuffer, texCoordID);
    //GL.TexCoordPointer (2, All.Float, 0, IntPtr.Zero);
    //GL.EnableClientState (All.TextureCoordArray);

    glDrawArrays (mode, 0, vertexCount);
}