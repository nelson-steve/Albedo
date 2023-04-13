#pragma once

#include "Collider.h"

class PlaneCollider : public Collider
{
public:
	//virtual bool operator==(Collider& c) override
	//{
	//	auto* sphere = dynamic_cast<PlaneCollider*>(&c);
	//	//if (this->m_Center == sphere->m_Center && this->m_Radius == sphere->m_Radius) return true;
	//	//else return false;
	//
	//	Albedo_Core_ERROR("Error: SphereColliders unknown");
	//	return false;
	//}
	//
	//virtual const Type GetType() const override { return m_Type; }
private:
	glm::vec3 m_Plane;
	float m_Distance;
};