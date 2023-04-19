#pragma once

#include "Collider.h"

#include <glm/glm.hpp>

namespace Albedo {

	class SphereCollider : public Collider
	{
	public:
		virtual bool operator==(Collider& c) override
		{
			auto* sphere = dynamic_cast<SphereCollider*>(&c);
			if (this->m_Center == sphere->m_Center && this->m_Radius == sphere->m_Radius) return true;
			else return false;
			
			Albedo_Core_ERROR("Error: SphereColliders unknown");
			return false;
		}
		
		void SetCenter(const glm::vec3& center) { m_Center = center; }
		void SetRadius(float radius) { m_Radius = radius; }
		virtual void SetType(const Type& type) override { m_Type = type; }

		const glm::vec3& GetCenter() const { return m_Center; }
		const float GetRadius() const { return m_Radius; }
		virtual const Type GetType() const override { return m_Type; }
	private:
		float m_Radius;
		glm::vec3 m_Center;
	};

}