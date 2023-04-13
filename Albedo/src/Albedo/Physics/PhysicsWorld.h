#pragma once

#include "Albedo/Scene/Components.h"
#include <Albedo/Core/Timestep.h>
#include <glm/glm.hpp>

#include "Collider.h"

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
		void Update();
		void Update(const Timestep& ts, PhysicsComponent& phyCmp, TransformComponent& transformComp);
	private:
		void CheckCollision();
		glm::vec3 m_Gravity = glm::vec3(0, -9.8, 0);
		//std::vector<Collider> m_Colliders;
	};

}