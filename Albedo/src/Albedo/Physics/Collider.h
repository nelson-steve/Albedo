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
	Cube
};

class Collider
{
public:
	virtual bool operator==(Collider& c) = 0;
	
	virtual const glm::vec3& GetCenter() const = 0;
	virtual const float GetRadius() const = 0;
	virtual const Type GetType() const = 0;
protected:
	Type m_Type;
};