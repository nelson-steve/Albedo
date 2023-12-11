#pragma once

#include "Albedo/Scene/Entity.h"
#include "PhysicsTaskDispatcher.h"
#include "Albedo/Core/Core.h"

#include <entt.hpp>
#include <glm/glm.hpp>
#include <PxPhysics.h>
#include <PxPhysicsAPI.h>

namespace Albedo {

    class PhysicsSolver : public physx::PxSimulationEventCallback {
        friend class PhysicsCollider;
        PhysicsTaskDispatcher taskDispatcher;
    public:
        PhysicsSolver() = default;
        ~PhysicsSolver();

        void Init();

        void AddActor(const RigidBodyDynamicComponent& rg);
        void UpdatePhysics(Timestep ts);
        void Deallocate();

        void setupFiltering(physx::PxRigidActor* actor, physx::PxU32 filterGroup, physx::PxU32 filterMask);

        // PxSimulationEventCallback overrides
        virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) override {}
        virtual void onWake(physx::PxActor** actors, physx::PxU32 count) override {}
        virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) override {}
        virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) override;
        virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override;
        virtual void onAdvance(const physx::PxRigidBody* const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count) override {}
    public:
        physx::PxRigidStatic* groundPlane;
        physx::PxMaterial* terrainMaterial;
        static physx::PxPhysics* phys;
        static physx::PxPvd* pvd;
        static physx::PxCooking* cooking;
        static physx::PxScene* scene;
    protected:
        //struct PhysicsTransform 
        //{
        //    glm::vec3 pos;
        //    glm::quat rot;
        //    PhysicsTransform(const glm::vec3& p, const glm::quat& q) : pos(p), rot(q) {}
        //};
    protected:
        static physx::PxDefaultErrorCallback gDefaultErrorCallback;
        static physx::PxDefaultAllocator gDefaultAllocatorCallback;
        static physx::PxFoundation* foundation;

        // If deltatime > this value, the system will substep
        constexpr static float max_step_time = 1.f / 30;
        //friend class PhysicsBodyComponent;
    };
}