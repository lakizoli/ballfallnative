#pragma once

#include "content/vector2D.h"

class RigidBody;
class Mesh2D;

typedef shared_ptr<RigidBody> (*FUNC_FIND_COLLISION) (shared_ptr<RigidBody>);

/// Class for implement basic rigid body physics.
class RigidBody2D {
public:
    //public delegate RigidBody2D FindCollisionDelegate (RigidBody2D body);

    constexpr static const float PhysicalScale = 30.0f; ///< Physical scale [pixel / meter]
	constexpr static const float Gravity = 9.81f; ///< [m/sec^2]

	shared_ptr<Mesh2D> Mesh;
	Vector2D LastPos;
	Vector2D Velocity;
	float Mass;
	Vector2D Force;

	//FindCollisionDelegate FindCollision { get; set; }

	shared_ptr<RigidBody2D> CollideBody;

public:
	RigidBody2D () : Mass (0) {}

    void Update (float elapsedTime) {
    //    //Store last pos
    //    LastPos = Mesh.Pos;
    //    CollideBody = null;

    //    //Calculate new position
    //    Vector2D accel = Force / Mass / PhysicalScale;
    //    Vector2D add = accel * elapsedTime;
    //    Mesh.Pos += Velocity * elapsedTime + add / 2.0f;
    //    Velocity += add;

    //    //Check collision (and calculate new position)
    //    if (FindCollision != null) {
    //        CollideBody = FindCollision (this);
    //        if (CollideBody != null) {
    //            //Find collision position (circle intersection with velocity direction line)
    //            float radius = Mesh.TransformedBoundingBox.Width / 2.0f + CollideBody.Mesh.TransformedBoundingBox.Width / 2.0f; //Most egyszerusitunk, mert minden mesh kor...
    //            Vector2D[] intersection = Geom.LineCircleIntersection (Velocity.Normalize (), Mesh.Pos, CollideBody.Mesh.Pos, radius);
    //            if (intersection != null && intersection.Length > 0) {
    //                float len0 = (intersection[0] - LastPos).SquareLength;
    //                float len1 = intersection.Length > 1 ? (intersection[1] - LastPos).SquareLength : 0;
    //                Vector2D collisionPos = intersection[len0 < len1 ? 0 : 1];

    //                //Calculate elapsed time before collision
    //                float fullLen = (Mesh.Pos - LastPos).Length;
    //                float collLen = (collisionPos - LastPos).Length;
    //                float percent = collLen / fullLen;
    //                float partialTime = percent * elapsedTime;
    //                float remainingTime = elapsedTime - partialTime;
    //                Mesh.Pos = collisionPos;

    //                //Calculate new velocity and position after collision (until remaining time)
    //                Vector2D dist = CollideBody.Mesh.Pos - Mesh.Pos;
    //                Vector2D norm = dist.Normalize ();
    //                Vector2D proj = Velocity.Dot (norm) * norm;
    //                Velocity = (2.0f * (Velocity - proj)) - Velocity;
    //                Mesh.Pos += Velocity * remainingTime;
    //            }
    //        }
    //    }

    //    //Clear all forces
    //    Force = Vector2D.Zero;
    }
};
