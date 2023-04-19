#pragma once
#include "Albedo/Core/Core.h"

#include <glm/glm.hpp>

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

enum class Type
{
	Sphere = 0,
	BoxAABB
};

class Collider
{
public:
	virtual bool operator==(Collider& c) = 0;
	virtual void SetType(const Type& type) = 0;
	virtual const Type GetType() const = 0;
protected:
	Type m_Type = Type::Sphere;
};