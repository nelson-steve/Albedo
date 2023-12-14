#pragma once

#include <glm/glm.hpp>
#include <reactphysics3d/reactphysics3d.h>

namespace Albedo {
    struct BoxCollider {
        void SetHalfSize(const glm::vec3& halfSize) {
            m_HalfSize = halfSize;
        }
        const glm::vec3& GetHalfSize() const { return m_HalfSize; }
    private:
        glm::vec3 m_HalfSize;
        reactphysics3d::BoxShape* m_Shape;

        friend class PhysicsWorld;
        friend class StaticBody;
        friend class DynamicBody;
    };

    struct SphereCollider {
        void SetRadius(float radius) {
            m_Radius = radius;
        }
        float GetRadius() const { return m_Radius; }
    private:
        float m_Radius;
        reactphysics3d::SphereShape* m_Shape;

        friend class PhysicsWorld;
        friend class StaticBody;
        friend class DynamicBody;
    };

    struct CapsuleCollider {
        void SetSize(float radius, float height) {
            m_Radius = radius;
            m_Height = height;
        }
    private:
        float m_Radius;
        float m_Height;
        reactphysics3d::CapsuleShape* m_Shape;

        friend class PhysicsWorld;
        friend class StaticBody;
        friend class DynamicBody;
    };

    struct StaticBody {
    public:
        void AddBoxCollider(Ref<BoxCollider> collider, const glm::vec3& relativePos) {
            assert(m_Body != nullptr);
            reactphysics3d::Transform transform;
            transform.setPosition(reactphysics3d::Vector3(relativePos.x, relativePos.y, relativePos.z));
            m_Body->addCollider(collider->m_Shape, transform);
        }
        void AddSphereCollider(Ref<SphereCollider> collider, const glm::vec3& relativePos) {
            assert(m_Body != nullptr);
            reactphysics3d::Transform transform;
            transform.setPosition(reactphysics3d::Vector3(relativePos.x, relativePos.y, relativePos.z));
            m_Body->addCollider(collider->m_Shape, transform);
        }
        void AddCapsuleCollider(Ref<CapsuleCollider> collider, const glm::vec3& relativePos) {
            assert(m_Body != nullptr);
            reactphysics3d::Transform transform;
            transform.setPosition(reactphysics3d::Vector3(relativePos.x, relativePos.y, relativePos.z));
            m_Body->addCollider(collider->m_Shape, transform);
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
    public:
        enum class BodyType {
            STATIC,
            KINEMATIC,
            DYNAMIC
        } m_Type = BodyType::STATIC;
    public:
        void AddBoxCollider(Ref<BoxCollider> collider, const glm::vec3& relativePos) {
            assert(m_Body != nullptr);
            reactphysics3d::Transform transform;
            transform.setPosition(reactphysics3d::Vector3(relativePos.x, relativePos.y, relativePos.z));
            m_Body->addCollider(collider->m_Shape, transform);
        }
        void AddSphereCollider(Ref<SphereCollider> collider, const glm::vec3& relativePos) {
            assert(m_Body != nullptr);
            reactphysics3d::Transform transform;
            transform.setPosition(reactphysics3d::Vector3(relativePos.x, relativePos.y, relativePos.z));
            m_Body->addCollider(collider->m_Shape, transform);
        }
        void AddCapsuleCollider(Ref<CapsuleCollider> collider, const glm::vec3& relativePos) {
            assert(m_Body != nullptr);
            reactphysics3d::Transform transform;
            transform.setPosition(reactphysics3d::Vector3(relativePos.x, relativePos.y, relativePos.z));
            m_Body->addCollider(collider->m_Shape, transform);
        }
        void EnableGravity(bool gravity) {
            m_Body->enableGravity(gravity);
        }
        float GetAngularDamping() {
            m_Body->getAngularDamping();
        }
        float GetLinearDamping() {
            m_Body->getLinearDamping();
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
        void SetAngularDamping(float dampingValue) {
            m_Body->setAngularDamping(dampingValue);
        }
        void SetLinearDamping(float dampingValue) {
            m_Body->setLinearDamping(dampingValue);
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

        Ref<BoxCollider> CreateBoxShape(const glm::vec3& size);
        Ref<SphereCollider> CreateSphereShape(float radius);
        Ref<CapsuleCollider> CreateCapsuleShape(float radius, float height);

        void DestroyStaticBody(const Ref<StaticBody> body);
        void DestroyDynamicBody(const Ref<DynamicBody> body);
    private:
        reactphysics3d::PhysicsCommon m_PhysicsCommon;
        // Create a physics world
        reactphysics3d::PhysicsWorld* m_World;

        std::vector<Ref<StaticBody>> m_StaticBodies;
        std::vector<Ref<DynamicBody>> m_DynamicBodies;

        std::vector<Ref<BoxCollider>> m_BoxColliders;
        std::vector<Ref<SphereCollider>> m_SphereColliders;
        std::vector<Ref<CapsuleCollider>> m_CapsuleColliders;
    };
}