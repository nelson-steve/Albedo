#include "AlbedoPreCompiledHeader.h"

#include "PhysicsMaterial.h"
#include "PhysicsSolver.h"

namespace Albedo {
	PhysicsMaterial::PhysicsMaterial(float staticFriction, float dynamicFriction, float restitution)
	{
		mat = PhysicsSolver::phys->createMaterial(staticFriction, dynamicFriction, restitution);
	}

	PhysicsMaterial::~PhysicsMaterial()
	{
		mat->release();
	}

	void PhysicsMaterial::SetStaticFriction(float sf)
	{
		mat->setStaticFriction(sf);
	}

	void PhysicsMaterial::SetDynamicFriction(float df)
	{
		mat->setDynamicFriction(df);
	}

	void PhysicsMaterial::SetRestitution(float r)
	{
		mat->setRestitution(r);
	}

	void PhysicsMaterial::SetFrictionCombineMode(PhysicsCombineMode mode)
	{

		mat->setFrictionCombineMode(static_cast<physx::PxCombineMode::Enum>(static_cast<std::underlying_type<PhysicsCombineMode>::type>(mode)));
	}

	void PhysicsMaterial::SetRestitutionCombineMode(PhysicsCombineMode mode)
	{
		mat->setRestitutionCombineMode(static_cast<physx::PxCombineMode::Enum>(static_cast<std::underlying_type<PhysicsCombineMode>::type>(mode)));
	}


	float PhysicsMaterial::GetStaticFriction() const
	{
		return mat->getStaticFriction();
	}

	float PhysicsMaterial::GetDynamicFriction() const
	{
		return mat->getDynamicFriction();
	}

	float PhysicsMaterial::GetRestitution() const
	{
		return mat->getRestitution();
	}

	PhysicsCombineMode PhysicsMaterial::GetFrictionCombineMode() const
	{
		return PhysicsCombineMode(mat->getFrictionCombineMode());
	}

	PhysicsCombineMode PhysicsMaterial::GetRestitutionCombineMode() const
	{
		return PhysicsCombineMode(mat->getRestitutionCombineMode());
	}
}