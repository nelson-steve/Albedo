#pragma once

#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>

namespace Albedo {
    struct StaticBody {
    public:
        void SetPosition(const glm::vec3& pos) {
            reactphysics3d::Transform transform;
            transform.setPosition(reactphysics3d::Vector3(pos.x, pos.y, pos.z));
            m_Body->setTransform(transform);
        }
        void SetOrientation(const glm::vec4& orient) {
            reactphysics3d::Transform transform;
            transform.setOrientation(reactphysics3d::Quaternion(orient.x, orient.y, orient.z, orient.w));
            m_Body->setTransform(transform);
        }
        const glm::vec3& GetPosition() const { 
            return glm::vec3{ 
                m_Body->getTransform().getPosition().x, 
                m_Body->getTransform().getPosition().y, 
                m_Body->getTransform().getPosition().z 
            };
        }
        const glm::vec4& GetOrientation() const {
            return glm::vec4{
                m_Body->getTransform().getOrientation().x,
                m_Body->getTransform().getOrientation().y,
                m_Body->getTransform().getOrientation().z,
                m_Body->getTransform().getOrientation().w
            };
        }
    private:
    reactphysics3d::CollisionBody* m_Body;

    friend class PhysicsWorld;
    };

    struct DynamicBody {
    private:
        enum class BodyType {
            STATIC,
            KINEMATIC,
            DYNAMIC
        } m_Type = BodyType::STATIC;
        reactphysics3d::RigidBody* m_Body;
    public:
        void EnableGravity(bool gravity) {
            m_Body->enableGravity(gravity);
        }
        const glm::vec3& GetPosition() const {
            return glm::vec3{
                m_Body->getTransform().getPosition().x,
                m_Body->getTransform().getPosition().y,
                m_Body->getTransform().getPosition().z
            };
        }
        const glm::vec4& GetOrientation() const {
            return glm::vec4{
                m_Body->getTransform().getOrientation().x,
                m_Body->getTransform().getOrientation().y,
                m_Body->getTransform().getOrientation().z,
                m_Body->getTransform().getOrientation().w
            };
        }
        void SetPosition(const glm::vec3& pos) {
            reactphysics3d::Transform transform;
            transform.setPosition(reactphysics3d::Vector3(pos.x, pos.y, pos.z));
            m_Body->setTransform(transform);
        }
        void SetOrientation(const glm::vec4& orient) {
            reactphysics3d::Transform transform;
            transform.setOrientation(reactphysics3d::Quaternion(orient.x, orient.y, orient.z, orient.w));
            m_Body->setTransform(transform);
        }
        void SetBodyType(const BodyType type) {
            m_Type = type;
            switch (type)
            {
                case BodyType::STATIC:
                    m_Body->setType(reactphysics3d::BodyType::STATIC);
                    break;
                case BodyType::KINEMATIC:
                    m_Body->setType(reactphysics3d::BodyType::KINEMATIC);
                    break;
                case BodyType::DYNAMIC:
                    m_Body->setType(reactphysics3d::BodyType::DYNAMIC);
                    break;
                default:
                    assert(false);
            }
        }
    private:
        reactphysics3d::RigidBody* m_Body;

    friend class PhysicsWorld;
    };

    class PhysicsWorld {
    public:
        void Init();
        void Update(float ts);
        void Destroy();

        Ref<StaticBody> CreateStaticBody(const glm::vec3& pos = glm::vec3{0.0f}, const glm::vec4& rot = glm::vec4{ 0.0f });
        Ref<DynamicBody> CreateDynamicBody(const glm::vec3& pos = glm::vec3{0.0f}, const glm::vec4& rot = glm::vec4{ 0.0f });

        void DestroyStaticBody(const Ref<StaticBody> body);
        void DestroyDynamicBody(const Ref<DynamicBody> body);
    private:
        reactphysics3d::PhysicsCommon m_PhysicsCommon;
        // Create a physics world
        reactphysics3d::PhysicsWorld* m_World;
    };
}