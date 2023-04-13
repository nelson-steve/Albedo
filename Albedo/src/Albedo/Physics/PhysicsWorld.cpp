#include "AlbedoPreCompiledHeader.h"

#include "PhysicsWorld.h"

namespace Albedo {

	void PhysicsWorld::Update(const Timestep& ts, PhysicsComponent& phyCmp, TransformComponent& transformComp)
	{
		phyCmp.Force += phyCmp.Mass * m_Gravity;
		phyCmp.Velocity += phyCmp.Force / phyCmp.Mass * ts.GetTime();
		phyCmp.Position += phyCmp.Velocity * ts.GetTime();
		phyCmp.Force = glm::vec3(0);
		
		transformComp.Position = phyCmp.Position;
	}

	void PhysicsWorld::Update()
	{
		CheckCollision();
	}

	void PhysicsWorld::CheckCollision()
	{
		for (auto& collider1 : m_Colliders)
		{
			for (auto& collider2 : m_Colliders)
			{
				if (collider2 == collider1) continue;
#if 1
				switch (collider1.GetType())
				{
					case Type::Sphere:
					{
						float distance = glm::dot((collider1.GetCenter() - collider2.GetCenter()), (collider1.GetCenter() - collider2.GetCenter()));
						if (distance < (collider1.GetRadius() + collider2.GetRadius()) * (collider1.GetRadius() + collider2.GetRadius()))
							Albedo_Core_INFO("Collision");
						break;
					}
					case Type::Cube:
						break;
					default:
						Albedo_Core_ERROR("Error: unkown collider type");
						break;
				}
#endif
			}
		}
	}

}