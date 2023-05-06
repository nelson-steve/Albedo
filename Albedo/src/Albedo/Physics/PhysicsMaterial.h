#pragma once

#include <PxPhysicsAPI.h>

namespace Albedo {
	enum class PhysicsMaterialFlag
	{
		DisableFriction = 1 << 0,
		DisableStrongFriction = 1 << 1
	};

	enum class PhysicsCombineMode
	{
		Average = 0,
		Min = 1,
		Multiply = 2,
		Max = 3,
		NValues = 4,
		Pad32 = 0x7fffffff
	};

	class PhysicsMaterial
	{
	public:
		PhysicsMaterial(float staticFriction = 1, float dynamicFriction = 1, float restitution = 1);
		virtual ~PhysicsMaterial();

		void SetStaticFriction(float);
		void SetDynamicFriction(float);
		void SetRestitution(float);
		void SetFrictionCombineMode(PhysicsCombineMode);
		void SetRestitutionCombineMode(PhysicsCombineMode);

		float GetStaticFriction() const;
		float GetDynamicFriction() const;
		float GetRestitution() const;
		PhysicsCombineMode GetFrictionCombineMode() const;
		PhysicsCombineMode GetRestitutionCombineMode() const;

		inline const physx::PxMaterial* GetPhysXmat() const { return mat;}

	private:
		physx::PxMaterial* mat = nullptr;
	};
}