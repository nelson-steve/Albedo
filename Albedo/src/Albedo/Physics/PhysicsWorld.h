#pragma once

#include "Albedo/Scene/Components.h"
#include <Albedo/Core/Timestep.h>
#include <glm/glm.hpp>

#include "Collider.h"

namespace Albedo {
#if 0
	//struct Object
	//{
	//	glm::vec3 Position;
	//	glm::vec3 Velocity;
	//	glm::vec3 Force;
	//	float Mass;
	//};

	class RigidBody;

	struct Contact
	{
		RigidBody* Body[2];
		float Friction;
		float Restitution;
		glm::vec3 Normal;
		glm::vec3 Point;
		float Penetration;
	};

	struct CollisionData
	{
		//Contact* ContactList;
		//uint32_t MaxContactLeft;

		Contact* contactArray;
        Contact* contacts;
        uint32_t contactsLeft;
        uint32_t contactCount;
        float    friction;
        float    restitution;
        float    tolerance;

        bool hasMoreContacts()
        {
            return contactsLeft > 0;
        }

        void reset(unsigned maxContacts)
        {
            contactsLeft = maxContacts;
            contactCount = 0;
            contacts = contactArray;
        }
		
		void addContacts(unsigned count)
        {
            contactsLeft -= count;
            contactCount += count;

            contacts += count;
        }
	};

	struct Primitive
	{
		RigidBody* body;
		glm::mat4 Offset;
	};

	struct Sphere : public Primitive
	{
		float Radius;
	};

	class CollisionDetector
	{
	public:
		void SphereAndSphere(const Sphere& s1, const Sphere& s2, CollisionData* data)
		{
			if (data->contactCount >= 0) return;

			// Cache the sphere positions.
			glm::vec3 positionOne = glm::vec3(s1.Offset[4]);
			glm::vec3 positionTwo = glm::vec3(s2.Offset[4]);
			// Find the vector between the objects.
			glm::vec3 midline = positionOne - positionTwo;
			float size = midline.x * midline.x + midline.y * midline.y + midline.z * midline.z;
			// See if it is large enough.
			if (size <= 0.0f || size >= s1.Radius + s2.Radius)
			{
				return;
			}

			glm::vec3 normal = midline * (((float)1.0) / size);
			Contact* contact = data->ContactList;
			contact->Normal = normal;
			contact->Point = positionOne + midline * (float)0.5;
			contact->Penetration = (s1.Radius + s2.Radius - size);
			// Write the appropriate data.
			contact->body[0] = s1.body;
			contact->body[1] = s2.body;
			contact->restitution = data->Restitution;
			contact->friction = data->friction;
			return 1;
		}
	};
#endif

	class RigidBody
	{
	public:
		void EnableGravity(bool g) { m_UseGravity = g; }
		void SetMass(float mass) { m_Mass = mass; }
		void SetInverseMass(float invMass) { m_InverseMass = invMass; }
		void SetRestitution(float restitution) { m_Restitution = restitution; }

		bool GetGravityStatus() const { return m_UseGravity; }
		const glm::vec3& GetVelocity() const { return m_Velocity; }
		float GetMass() const { return m_Mass; }
		float GetInverseMass() const { return m_InverseMass; }
		float GetRestitution() const { return m_Restitution; }
		const glm::vec3& GetTotalForce() const { return m_TotalForce; }
	private:
		bool m_UseGravity = true;
		glm::vec3 m_Velocity = glm::vec3(0.0f);
		float m_Mass = 1.0f;
		float m_InverseMass;
		float m_Restitution = 0.5f;
		glm::vec3 m_TotalForce = glm::vec3(0.0f);
	};

	class PhysicsWorld
	{
	public:
		void Update();
		void Update(const Timestep& ts, PhysicsComponent& phyCmp, TransformComponent& transformComp, ColliderComponent& colliderComp);

		const std::vector<ColliderComponent&>& GetColliderList() const { return m_Colliders; }
		std::vector<ColliderComponent&>& GetColliderList() { return m_Colliders; }
	private:
		std::vector<ColliderComponent&> m_Colliders;
		std::vector<std::pair<ColliderComponent&, ColliderComponent&>> m_IntersectingColliders;
		void CheckCollision();
		glm::vec3 m_Gravity = glm::vec3(0, -9.8, 0);
	};

}