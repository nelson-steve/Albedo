#pragma once

#include "Albedo/Scene/Components.h"

#include <glm/glm.hpp>
#include <Albedo/Core/Timestep.h>

namespace Albedo {

	//struct Object
	//{
	//	glm::vec3 Position;
	//	glm::vec3 Velocity;
	//	glm::vec3 Force;
	//	float Mass;
	//};

	class PhysicsWorld
	{
	public:
		//void AddObject(const std::shared_ptr<Object> obj) { m_Objects.push_back(obj); }
		//
		//void RemoveObject(const std::shared_ptr<Object> obj)
		//{
		//	if (!obj) return;
		//	auto itr = std::find(m_Objects.begin(), m_Objects.end(), obj);
		//	m_Objects.erase(itr);
		//}

		//void Update(Timestep ts, PhysicsComponent& phyCmp, TransformComponent transformComp)
		//{
		//	phyCmp.Force += phyCmp.Mass * m_Gravity;
		//	phyCmp.Velocity += phyCmp.Force / phyCmp.Mass * ts.GetTime();
		//	phyCmp.Position += phyCmp.Velocity * ts.GetTime();
		//	phyCmp.Force = glm::vec3(0);
		//
		//	transformComp.Position = phyCmp.Position;
		//}
	private:
		glm::vec3 m_Gravity = glm::vec3(0, -9.8, 0);
		//std::vector<std::shared_ptr<Object>> m_Objects;
		//Collider m_collider;
	};

}