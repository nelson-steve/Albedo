#pragma once

#include "Collider.h"

#include <glm/glm.hpp>

namespace Albedo {

	class SphereCollider : public Collider
	{
	public:
		//virtual bool operator==(Collider& c) override
		//{
		//	//auto* sphere = dynamic_cast<SphereCollider*>(&c);
		//	//if (this->m_Center == sphere->m_Center && this->m_Radius == sphere->m_Radius) return true;
		//	//else return false;
		//	//
		//	Albedo_Core_ERROR("Error: SphereColliders unknown");
		//	return false;
		//}
		//
		//virtual const glm::vec3& GetCenter() const override { return m_Center; }
		//virtual const float GetRadius() const override { return m_Radius; }
		//virtual const Type GetType() const override { return m_Type; }
	private:
		glm::vec3 m_Center;
		float m_Radius;
	};

}