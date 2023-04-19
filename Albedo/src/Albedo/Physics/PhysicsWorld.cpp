#include "AlbedoPreCompiledHeader.h"

#include "PhysicsWorld.h"
#include "SphererCollider.h"

namespace Albedo {

	void PhysicsWorld::Update(const Timestep& ts, PhysicsComponent& phyCmp, TransformComponent& transformComp, ColliderComponent& colliderComp)
	{
		//phyCmp.Force += phyCmp.Mass * m_Gravity;
		//phyCmp.Velocity += phyCmp.Force / phyCmp.Mass * ts.GetTime();
		//transformComp.Position += phyCmp.Velocity * ts.GetTime();
		//phyCmp.Force = glm::vec3(0);
		//std::dynamic_pointer_cast<SphereCollider>(colliderComp.collider)->SetCenter(transformComp.Position);
		
		Update();
	}

	void PhysicsWorld::Update()
	{
		CheckCollision();
	}

	void PhysicsWorld::CheckCollision()
	{
		// Broad Phase
		{

			for (auto& collider1 : m_Colliders)
			{
				for (auto& collider2 : m_Colliders)
				{
					if (collider2 == collider1) continue;
					switch (collider1.collider->GetType())
					{
						case Type::Sphere:
						{
							const auto& c1 = std::dynamic_pointer_cast<SphereCollider>(collider1.collider);
							const auto& c2 = std::dynamic_pointer_cast<SphereCollider>(collider2.collider);
							auto a = c1->GetCenter();
							auto b = c2->GetRadius();
							float distance = glm::dot((c1->GetCenter() - c1->GetCenter()), (c1->GetCenter() - c1->GetCenter()));
							if (distance < (c2->GetRadius() + c2->GetRadius()) * (c2->GetRadius() + c2->GetRadius()))
								Albedo_Core_INFO("Collision");
							break;
						}
						case Type::BoxAABB:
						{
							const auto& c1 = std::dynamic_pointer_cast<BoxCollider>(collider1.collider);
							const auto& c2 = std::dynamic_pointer_cast<BoxCollider>(collider2.collider);
							if (c1->GetMin().x > c2->GetMax().x || c1->GetMax().x < c2->GetMin().x)
								break;
							else if (c1->GetMin().y > c2->GetMax().y || c1->GetMax().y < c2->GetMin().y)
								break;
							else if (c1->GetMin().z > c2->GetMax().z || c1->GetMax().z < c2->GetMin().z)
								break;
							else
							{
								m_IntersectingColliders.push_back(std::pair<ColliderComponent&, ColliderComponent&>(collider1, collider2));
								Albedo_Core_INFO("Collision");
								break;
							}
						}
						default:
							Albedo_Core_ERROR("Error: unkown collider type");
							break;
					}
				}
			}
		}

		// Narrow Phase
		{

		}
	}

}