#pragma once
#include "Albedo/Core/Core.h"
#include "PhysicsMaterial.h"
#include "Albedo/Renderer/Mesh.h"
#include "PhysicsBodyComponent.h"

#include <glm/gtc/quaternion.hpp>
#include <PxPhysics.h>

namespace Albedo {

	class PhysicsCollider
	{
	public:
		//matrix4 CalculateWorldMatrix(const RavEngine::Transform&) const;
		static enum class CollisionType { Trigger, Collider };
		void UpdateFilterData(RigidBodyDynamicComponent* body);
		void UpdateFilterData(RigidBodyStaticComponent* body);
		void SetType(CollisionType);
		CollisionType GetType() const;
		void SetQueryable(bool);
		bool GetQueryable() const;
		void SetRelativeTransform(const glm::vec3& position, const glm::quat& rotation);
		glm::mat4 GetRelativeTransform() const;
		//virtual void DebugDraw(RavEngine::DebugDrawer& dbg, color_t debug_color, const RavEngine::Transform&) const {}
	protected:
		physx::PxShape* collider = nullptr;
		//Ref<PhysicsMaterial> material;
		friend class PhysicsBodyComponent;
	};


	class BoxCollider : public PhysicsCollider 
	{
	public:
		BoxCollider(RigidBodyDynamicComponent* rg, const glm::vec3& size, Ref<PhysicsMaterial> mat, const glm::vec3& position = glm::vec3(0, 0, 0), const glm::quat& rotation = glm::quat(1.0, 0.0, 0.0, 0.0));
		BoxCollider(RigidBodyStaticComponent* rg, const glm::vec3& size, Ref<PhysicsMaterial> mat, const glm::vec3& position = glm::vec3(0, 0, 0), const glm::quat& rotation = glm::quat(1.0, 0.0, 0.0, 0.0));
		virtual ~BoxCollider() {}
	};

	class SphereCollider : public PhysicsCollider 
	{
	public:
		SphereCollider(RigidBodyDynamicComponent* owner, float radius, Ref<PhysicsMaterial> mat, const glm::vec3& position = glm::vec3(0, 0, 0), const glm::quat& rotation = glm::quat(1.0, 0.0, 0.0, 0.0));
		SphereCollider(RigidBodyStaticComponent* owner, float radius, Ref<PhysicsMaterial> mat, const glm::vec3& position = glm::vec3(0, 0, 0), const glm::quat& rotation = glm::quat(1.0, 0.0, 0.0, 0.0));

		void SetRadius(float radius);
		float GetRadius() const;
	private:
		float m_Radius;
	};

	class CapsuleCollider : public PhysicsCollider 
	{
	public:
		CapsuleCollider(RigidBodyDynamicComponent* body, float radius, float halfHeight, Ref<PhysicsMaterial> mat, const glm::vec3& pos = glm::vec3(0, 0, 0), const glm::quat& rot = glm::quat(1.0, 0.0, 0.0, 0.0));
		CapsuleCollider(RigidBodyStaticComponent* body, float radius, float halfHeight, Ref<PhysicsMaterial> mat, const glm::vec3& pos = glm::vec3(0, 0, 0), const glm::quat& rot = glm::quat(1.0, 0.0, 0.0, 0.0));
	private:
		float m_Radius;
		float m_HalfHeight;
	};

	class MeshCollider : public PhysicsCollider 
	{
	public:
		MeshCollider(RigidBodyDynamicComponent* body, Ref<Mesh> mesh, Ref<PhysicsMaterial> mat);
		MeshCollider(RigidBodyStaticComponent* body, Ref<Mesh> mesh, Ref<PhysicsMaterial> mat);
	private:
		Ref<Mesh> mesh;
	};

	class ConvexMeshCollider : public PhysicsCollider 
	{
	public:
		ConvexMeshCollider(RigidBodyDynamicComponent* body, Ref<Mesh> mesh, Ref<PhysicsMaterial> mat);
		ConvexMeshCollider(RigidBodyStaticComponent* body, Ref<Mesh> mesh, Ref<PhysicsMaterial> mat);
	};
}