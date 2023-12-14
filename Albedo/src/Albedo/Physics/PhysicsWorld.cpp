#include <AlbedoPreCompiledHeader.h>

#include "PhysicsWorld.h"

namespace Albedo {
    void PhysicsWorld::Init() {
        reactphysics3d::PhysicsWorld::WorldSettings settings;
        settings.defaultVelocitySolverNbIterations = 20;
        settings.isSleepingEnabled = false;
        settings.gravity = reactphysics3d::Vector3(0, -9.81, 0);
        //m_World = m_PhysicsCommon.createPhysicsWorld(settings);
        
        m_World = m_PhysicsCommon.createPhysicsWorld();
    }

    void PhysicsWorld::Update(float ts) {
        m_World->update(ts);
    }

    Ref<StaticBody> PhysicsWorld::CreateStaticBody(const glm::vec3& pos = glm::vec3{ 0.0f }, const glm::vec4& rot = glm::vec4{ 0.0f }) {
        Ref<StaticBody> staticBody = std::make_shared<StaticBody>();

        reactphysics3d::Vector3 position(pos.x, pos.y, pos.z);
        reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion(rot.x, rot.y, rot.z, rot.w);
        reactphysics3d::Transform transform(position, orientation);
        staticBody->m_Body = m_World->createCollisionBody(transform);
        return staticBody;
    }

    Ref<DynamicBody> PhysicsWorld::CreateDynamicBody(const glm::vec3& pos = glm::vec3{ 0.0f }, const glm::vec4& rot = glm::vec4{ 0.0f }) {
        Ref<DynamicBody> dynamicBody = std::make_shared<DynamicBody>();

        reactphysics3d::Vector3 position(pos.x, pos.y, pos.z);
        reactphysics3d::Quaternion orientation = reactphysics3d::Quaternion(rot.x, rot.y, rot.z, rot.w);
        reactphysics3d::Transform transform(position, orientation);
        dynamicBody->m_Body = m_World->createRigidBody(transform);
        return dynamicBody;
    }

    void PhysicsWorld::DestroyStaticBody(const Ref<StaticBody> m_Body) {
        m_World->destroyCollisionBody(m_Body->m_Body);
    }
    void PhysicsWorld::DestroyDynamicBody(const Ref<DynamicBody> m_Body) {
        m_World->destroyRigidBody(m_Body->m_Body);
    }

    void PhysicsWorld::Destroy() {
        m_PhysicsCommon.destroyPhysicsWorld(m_World);
    }
}