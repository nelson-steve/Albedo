#pragma once

#include "Collider.h"
#include "SphererCollider.h"

class PlaneCollider : public Collider
{
public:
	virtual CollisionPoints TestCollisions(
		const Transform& transform,
		const Collider& collider,
		const Transform& colliderTransform) const override
	{

	}

	virtual CollisionPoints TestCollisions(
		const Transform& transform,
		const SphereCollider& sphereCollider,
		const Transform& sphereTransform) const override
	{

	}

	virtual CollisionPoints TestCollisions(
		const Transform& transform,
		const PlaneCollider& planeCollider,
		const Transform& planeTransform) const
	{

	}
private:
	glm::vec3 m_Plane;
	float m_Distance;
};