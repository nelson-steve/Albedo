#pragma once
#include "PhysicsTaskDispatcher.h"
#include <PxPhysicsAPI.h>
#include <Albedo/Core/Timestep.h>

namespace Albedo {

	struct ColliderConfig
	{
		double width = 1, height = 1, depth = 1;
	};

	struct RigidBodyConfig
	{
		double width = 1, height = 1, depth = 1;
	};

	class PhysicsWorld : public physx::PxSimulationEventCallback
	{
	public:
		PhysicsWorld();
		~PhysicsWorld();
		void Init();
		void Update(Timestep ts);

		// PxSimulationEventCallback overrides
		virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override {}
		virtual void onWake(physx::PxActor** actors, physx::PxU32 count) override {}
		virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) override {}
		virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override {};
		virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override {};
		virtual void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override {}
	public:
		PhysicsTaskDispatcher taskDispatcher;

		Ref<ColliderConfig> m_ColliderConfig;
		Ref<RigidBodyConfig> m_RigidBodyConfig;

		physx::PxMaterial* mat = nullptr;
		physx::PxShape* collider = nullptr;
		physx::PxRigidDynamic* rigidActor = nullptr;

		physx::PxMaterial* mat2 = nullptr;
		physx::PxShape* collider2 = nullptr;
		physx::PxRigidDynamic* rigidActor2 = nullptr;

		static physx::PxPhysics* phys;
		static physx::PxPvd* pvd;
		static physx::PxCooking* cooking;
		static physx::PxScene* scene;

		static physx::PxDefaultErrorCallback gDefaultErrorCallback;
		static physx::PxDefaultAllocator gDefaultAllocatorCallback;
		static physx::PxFoundation* foundation;
	};

}