#include "AlbedoPreCompiledHeader.h"

#include "PhysicsBodyComponent.h"

#include "Albedo/Physics/PhysicsSolver.h"
#include "Conversions.h"
#include "PhysicsConfig.h"

#define STATIC(a) decltype(a) a

STATIC(Albedo::RigidBodyDynamicComponent::defaultConfig);

#undef STATIC(a) decltype(a) a

namespace Albedo { 

	// DYNAMIC

	RigidBodyDynamicComponent::RigidBodyDynamicComponent()
	{
		defaultConfig = new RidigBodyDynamicConfig();

		rigidActor = PhysicsSolver::phys->createRigidDynamic(glmToPhysx::ToTransform(defaultConfig->position));	//will be set pre-tick to the entity's location
		rigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, defaultConfig->enableGravity);
		rigidActor->setGlobalPose(glmToPhysx::ToTransform(defaultConfig->position, defaultConfig->rotation));
		static_cast<physx::PxRigidBody*>(rigidActor)->setLinearVelocity(glmToPhysx::ToVec3(defaultConfig->linearVelocity));
		static_cast<physx::PxRigidBody*>(rigidActor)->setAngularVelocity(glmToPhysx::ToVec3(defaultConfig->angularVelocity));
		static_cast<physx::PxRigidBody*>(rigidActor)->setMass(defaultConfig->mass);
		static_cast<physx::PxRigidBody*>(rigidActor)->setForceAndTorque(glmToPhysx::ToVec3(defaultConfig->force), glmToPhysx::ToVec3(defaultConfig->torque));
		delete defaultConfig;

		AddToScene();
	}

	RigidBodyDynamicComponent::RigidBodyDynamicComponent(const glm::vec3& pos, const glm::quat& rot, float mass)
	{
		rigidActor = PhysicsSolver::phys->createRigidDynamic(glmToPhysx::ToTransform(pos));	//will be set pre-tick to the entity's location
		rigidActor->setGlobalPose(glmToPhysx::ToTransform(pos, rot));
		static_cast<physx::PxRigidBody*>(rigidActor)->setMass(mass);

		AddToScene();
	}

	void RigidBodyDynamicComponent::DisableGravity(bool gravity)
	{
		rigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
	}

	void RigidBodyDynamicComponent::AddToScene()
	{
		PhysicsSolver::scene->lockWrite();
		PhysicsSolver::scene->addActor(*rigidActor);
		PhysicsSolver::scene->unlockWrite();
	}

	void RigidBodyDynamicComponent::SetLinearVelocity(const glm::vec3& vel, bool autowake)
	{
		rigidActor->getScene()->lockWrite();
		static_cast<physx::PxRigidDynamic*>(rigidActor)->setLinearVelocity(physx::PxVec3(vel.x, vel.y, vel.z), autowake);
		rigidActor->getScene()->unlockWrite();
	}

	void RigidBodyDynamicComponent::SetAngularVelocity(const glm::vec3& newvel, bool autowake)
	{
		rigidActor->getScene()->lockWrite();
		static_cast<physx::PxRigidDynamic*>(rigidActor)->setAngularVelocity(physx::PxVec3(newvel.x, newvel.y, newvel.z), autowake);
		rigidActor->getScene()->unlockWrite();
	}

	void RigidBodyDynamicComponent::SetKinematicTarget(const glm::vec3& targetPos, const glm::quat& targetRot)
	{
		physx::PxTransform transform(physx::PxVec3(targetPos.x, targetPos.y, targetPos.z), physx::PxQuat(targetRot.x, targetRot.y, targetRot.z, targetRot.w));
		rigidActor->getScene()->lockWrite();
		static_cast<physx::PxRigidDynamic*>(rigidActor)->setKinematicTarget(transform);
		rigidActor->getScene()->unlockWrite();
	}

	void RigidBodyDynamicComponent::SetAxisLock(uint16_t LockFlags)
	{
		static_cast<physx::PxRigidDynamic*>(rigidActor)->setRigidDynamicLockFlags(static_cast<physx::PxRigidDynamicLockFlag::Enum>(LockFlags));
	}

	glm::vec3 RigidBodyDynamicComponent::GetLinearVelocity() const
	{
		rigidActor->getScene()->lockRead();
		auto& vel = static_cast<physx::PxRigidBody*>(rigidActor)->getLinearVelocity();
		glm::vec3& ret = glm::vec3(vel.x, vel.y, vel.z);
		rigidActor->getScene()->unlockRead();
		return ret;
	}

	glm::vec3 RigidBodyDynamicComponent::GetAngularVelocity() const
	{
		rigidActor->getScene()->lockRead();
		auto& vel = static_cast<physx::PxRigidBody*>(rigidActor)->getAngularVelocity();
		glm::vec3& ret = glm::vec3(vel.x, vel.y, vel.z);
		rigidActor->getScene()->unlockRead();
		return ret;
	}

	float RigidBodyDynamicComponent::GetMass() const
	{
		rigidActor->getScene()->lockRead();
		float mass = static_cast<physx::PxRigidDynamic*>(rigidActor)->getMass();
		rigidActor->getScene()->unlockRead();
		return mass;
	}

	float RigidBodyDynamicComponent::GetInverseMass() const
	{
		rigidActor->getScene()->lockRead();
		float invMass;
		invMass = static_cast<physx::PxRigidDynamic*>(rigidActor)->getInvMass();
		rigidActor->getScene()->unlockRead();
		return invMass;
	}

	std::pair<glm::vec3, glm::quat> RigidBodyDynamicComponent::GetKinematicTarget() const
	{
		physx::PxTransform trns;
		rigidActor->getScene()->lockRead();
		static_cast<physx::PxRigidDynamic*>(rigidActor)->getKinematicTarget(trns);
		rigidActor->getScene()->unlockRead();
		return std::make_pair(glm::vec3(trns.p.x, trns.p.y, trns.p.z), glm::quat(trns.q.w, trns.q.x, trns.q.y, trns.q.z));
	}

	uint16_t RigidBodyDynamicComponent::GetAxisLock() const
	{
		return static_cast<physx::PxRigidDynamic*>(rigidActor)->getRigidDynamicLockFlags();
	}

	void RigidBodyDynamicComponent::AddForce(const glm::vec3& force)
	{
		rigidActor->getScene()->lockWrite();
		static_cast<physx::PxRigidDynamic*>(rigidActor)->addForce(physx::PxVec3(force.x, force.y, force.z));
		rigidActor->getScene()->unlockWrite();
	}

	void RigidBodyDynamicComponent::AddTorque(const glm::vec3& torque)
	{
		rigidActor->getScene()->lockWrite();
		static_cast<physx::PxRigidDynamic*>(rigidActor)->addTorque(physx::PxVec3(torque.x, torque.y, torque.z));
		rigidActor->getScene()->unlockWrite();
	}

	void RigidBodyDynamicComponent::ClearAllForces()
	{
		rigidActor->getScene()->lockWrite();
		static_cast<physx::PxRigidDynamic*>(rigidActor)->clearForce();
		rigidActor->getScene()->unlockWrite();
	}

	void RigidBodyDynamicComponent::ClearAllTorques()
	{
		rigidActor->getScene()->lockWrite();
		static_cast<physx::PxRigidDynamic*>(rigidActor)->clearTorque();
		rigidActor->getScene()->unlockWrite();
	}

	// STATIC

	RigidBodyStaticComponent::RigidBodyStaticComponent(const glm::vec3& pos, const glm::quat& rot)
	{
		rigidActor = PhysicsSolver::phys->createRigidStatic(glmToPhysx::ToTransform(pos, rot));	//will be set pre-tick to the entity's location

		AddToScene();
	}

	RigidBodyStaticComponent::~RigidBodyStaticComponent()
	{
	}

	void RigidBodyStaticComponent::AddToScene()
	{
		PhysicsSolver::scene->addActor(*rigidActor);
	}

}