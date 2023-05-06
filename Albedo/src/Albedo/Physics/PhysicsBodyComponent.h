#pragma once

#include "Albedo/Core/Core.h"
#include "PhysicsConfig.h"

#include <entt.hpp>
#include <PxPhysicsAPI.h>
#include <vector>
#include <glm/gtc/quaternion.hpp>
#include <glm/glm.hpp>

namespace Albedo {

	class RigidBodyDynamicComponent
	{
	public:
		RigidBodyDynamicComponent();
		RigidBodyDynamicComponent(const glm::vec3& pos = glm::vec3(0.0f), const glm::quat& rot = glm::quat(), float mass = 1.0f);
		~RigidBodyDynamicComponent();
		void AddToScene();
		const auto& GetScene() const { return rigidActor->getScene(); }

		void SetLinearVelocity(const glm::vec3& vel, bool autoWake);
		void SetAngularVelocity(const glm::vec3& vel, bool autoWake);
		void SetKinematicTarget(const glm::vec3& targetPos, const glm::quat& targetRot);
		void SetAxisLock(uint16_t LockFlags);
		void SetFilterGroup(physx::PxU32 group) { filterGroup = group; }
		void SetFilterMask(physx::PxU32 mask) { filterMask = mask; }

		glm::vec3 GetLinearVelocity() const;
		glm::vec3 GetAngularVelocity() const;
		std::pair<glm::vec3, glm::quat> GetKinematicTarget() const;
		uint16_t GetAxisLock() const;
		physx::PxU32 GetFilterGroup() const { return filterGroup; }
		physx::PxU32 GetFilterMask() const { return filterMask; }
		physx::PxRigidActor* GetRigidActor() const { return rigidActor; }

		void WakeUp();
		void Sleep();
		bool IsSleeping();
		void SetMass(float mass);
		float GetMass() const;
		float GetInverseMass() const;
		void AddForce(const glm::vec3& force);
		void AddTorque(const glm::vec3& torque);
		void ClearAllForces();
		void ClearAllTorques();
		void Destroy();
	private:
		const static RidigBodyDynamicConfig* defaultConfig;
		physx::PxRigidActor* rigidActor = nullptr;
		physx::PxU32 filterGroup = -1;
		physx::PxU32 filterMask = -1;
	};

	class RigidBodyStaticComponent
	{
	public:
		RigidBodyStaticComponent(const glm::vec3& pos = glm::vec3(0.0f), const glm::quat& rot = glm::quat());
		virtual ~RigidBodyStaticComponent();

		void SetFilterGroup(physx::PxU32 group) { filterGroup = group; }
		void SetFilterMask(physx::PxU32 mask) { filterMask = mask; }

		physx::PxU32 GetFilterGroup() const { return filterGroup; }
		physx::PxU32 GetFilterMask() const { return filterMask; }
		physx::PxRigidActor* GetRigidActor() const { return rigidActor; }

		void AddToScene();
	private:
		physx::PxRigidActor* rigidActor = nullptr;
		physx::PxU32 filterGroup = -1;
		physx::PxU32 filterMask = -1;
	};

}