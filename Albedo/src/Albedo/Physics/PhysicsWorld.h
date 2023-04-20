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

	//class RigidBody;

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

		void CalculateDerivedData();
		void Integrate();

		void CanSleep(bool sleep) { m_CanSleep = sleep; }
		void Awake(bool awake) { m_IsAwake = awake; }
		void EnableGravity(bool g) { m_UseGravity = g; }
		void SetLinearDamping(float damping) { m_LinearDamping = damping; }
		void SetAngularDamping(float angularDamping) { m_AngularDamping = angularDamping; }
		void SetMotion(float motion) { m_Motion = motion; }
		void SetMass(float mass) { m_Mass = mass; }
		void SetInverseMass(float invMass) { m_InverseMass = invMass; }
		void SetRestitution(float restitution) { m_Restitution = restitution; }
		void SetPosition(const glm::vec3& pos) { m_Position = pos; }
		void SetTotalTorque(const glm::vec3& torque) { m_TotalTorque = torque; }
		void SetAcceleration(const glm::vec3& acceleration) { m_Acceleration = acceleration; }
		void SetLastFrameAcceleration(const glm::vec3& lastAcceleration) { m_LastFrameAcceleration = lastAcceleration; }
		void SetVelocity(const glm::vec3& velocity) { m_Velocity = velocity; }
		void SetTotalForce(const glm::vec3& totalForce) { m_TotalForce = totalForce; }
		void SetInverseInertiaTensorWorld(const glm::mat3& inertiaTensorWorld) { m_InverseIntertiaTensor = inertiaTensorWorld; }
		void SetInverseInertiaTensor(const glm::mat3& inertiaTensor) { m_InverseIntertiaTensor = inertiaTensor; }
		void SetTransform(const glm::mat4 transform) { m_Transform = transform; }
		void SetOrientation(const glm::quat& orientation) { m_Orientation = orientation; }

		bool CanSleep() const { return m_CanSleep; }
		bool IsAwake() const { return m_IsAwake; }
		bool GetGravityStatus() const { return m_UseGravity; }
		float GetLinearDamping() const { return m_LinearDamping; }
		float GetAngularDamping() const { return m_AngularDamping; }
		float GetMotion() const { return m_Motion; }
		float GetMass() const { return m_Mass; }
		float GetInverseMass() const { return m_InverseMass; }
		float GetRestitution() const { return m_Restitution; }
		const glm::vec3& GetPosition() const { return m_Position; }
		const glm::vec3& GetTotalTorque() const { return m_TotalTorque; }
		const glm::vec3& GetAcceleration() const { return m_Acceleration; }
		const glm::vec3& GetLastFrameAcceleration() const { return m_LastFrameAcceleration; }
		const glm::vec3& GetVelocity() const { return m_Velocity; }
		const glm::vec3& GetTotalForce() const { return m_TotalForce; }
		const glm::mat3& GetInverseInertiaTensorWorld() const { return m_InverseInertiaTensorWorld; }
		const glm::mat3& GetInverseInertiaTensor() const { return m_InverseIntertiaTensor; }
		const glm::mat4& GetTransform() const { return m_Transform; }
		const glm::quat& GetOrientation() const { return m_Orientation; }
		const glm::vec3& GetAxis(uint32_t index) const { return m_Transform[index]; }
	private:
		bool  m_CanSleep = false;
		bool  m_IsAwake = true;
		bool  m_UseGravity = true;
		float m_LinearDamping = 0.0f;
		float m_AngularDamping = 0.0f;
		float m_Motion = 0.0f;
		float m_Mass = 1.0f;
		float m_InverseMass = 0.0f;
		float m_Restitution = 0.5f;
		glm::vec3 m_Position = glm::vec3(0.0f);
		glm::vec3 m_TotalTorque = glm::vec3(0.0f);
		glm::vec3 m_Acceleration = glm::vec3(0.0f);
		glm::vec3 m_LastFrameAcceleration = glm::vec3(0.0f);
		glm::vec3 m_Velocity = glm::vec3(0.0f);
		glm::vec3 m_TotalForce = glm::vec3(0.0f);
		glm::mat3 m_InverseInertiaTensorWorld = glm::mat3(1.0f);
		glm::mat3 m_InverseIntertiaTensor = glm::mat3(1.0f);
		glm::mat4 m_Transform = glm::mat4(1.0f);
		glm::quat m_Orientation;
	};

	class PhysicsWorld
	{
	public:
		void Update();
		void Update(const Timestep& ts, PhysicsComponent& phyCmp, TransformComponent& transformComp, ColliderComponent& colliderComp);

		const std::vector<ColliderComponent>& GetColliderList() const { return m_Colliders; }
		std::vector<ColliderComponent>& GetColliderList() { return m_Colliders; }
	private:
		std::vector<ColliderComponent> m_Colliders;
		std::vector<std::pair<ColliderComponent, ColliderComponent>> m_IntersectingColliders;
		void CheckIntersections();
		void CheckCollisions();
	};

}