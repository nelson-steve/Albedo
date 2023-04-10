#pragma once

#include "SphererCollider.h"

#include "PhysicsImpl.h"

struct CollisionPoints
{
	glm::vec3 PointA;
	glm::vec3 PointB;
	glm::vec3 Normal;
	float Depth;
	bool HasCollision;
};

struct Transform
{
	glm::vec3 Position;
	glm::vec3 Scale;
	float Rotation; // TODO: use quaternion
};

class Collider
{
	virtual CollisionPoints TestCollisions(
		const Transform& transform,
		const Collider& collider,
		const Transform& colliderTransform) const = 0;

	virtual CollisionPoints TestCollisions(
		const Transform& transform,
		const SphereCollider& sphereCollider,
		const Transform& sphereTransform) const = 0;

	virtual CollisionPoints TestCollisions(
		const Transform& transform,
		const PlaneCollider& planeCollider,
		const Transform& planeTransform) const = 0;
};