#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Albedo {


	struct RidigBodyDynamicConfig
	{
		enum AxisLock {
			Linear_X = (1 << 0),
			Linear_Y = (1 << 1),
			Linear_Z = (1 << 2),
			Angular_X = (1 << 3),
			Angular_Y = (1 << 4),
			Angular_Z = (1 << 5)
		};
		bool enableAxisLock = false;
		bool isAsleep = false;
		bool enableGravity = true;
		float mass = 1.0f;
		glm::vec3 position = glm::vec3(0.0f);
		glm::quat rotation = glm::quat();
		glm::vec3 linearVelocity = glm::vec3(0.0f);
		glm::vec3 angularVelocity = glm::vec3(0.0f);
		glm::vec3 kinematicTargetPosition = glm::vec3(0.0f);
		glm::vec3 force = glm::vec3(0.0f);
		glm::vec3 torque = glm::vec3(0.0f);
		glm::quat kinematicTargetRotation = glm::quat();
		AxisLock axisLock = AxisLock::Linear_X;
	};


}