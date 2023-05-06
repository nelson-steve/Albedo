#pragma once

#include <PxPhysics.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


namespace Albedo {

	namespace glmToPhysx {
		
		using namespace physx;

		static PxVec3 ToVec3(const glm::vec3 v)
		{
			return PxVec3(v.x, v.y, v.z);
		}

		static PxQuat ToQuaternion(const glm::quat& q)
		{
			return PxQuat(q.x, q.y, q.z, q.w);
		}

		static PxTransform ToTransform(const glm::vec3& pos)
		{
			return PxTransform(physx::PxTransform(glmToPhysx::ToVec3(pos)));
		}

		static PxTransform ToTransform(const glm::vec3& pos, const glm::quat& rot)
		{
			return PxTransform(physx::PxTransform(glmToPhysx::ToVec3(pos), glmToPhysx::ToQuaternion(rot)));
		}

	}

}