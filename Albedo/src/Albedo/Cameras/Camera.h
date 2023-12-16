#pragma once

#include <glm/glm.hpp>

namespace Albedo {

	class Camera
	{
	public:
		Camera() = default;
		virtual ~Camera() = default;

		virtual const glm::mat4& GetProjection() const = 0;
		virtual const glm::mat4& GetView() const = 0;
		virtual const glm::mat4& GetProjectionView() const = 0;
		virtual const glm::vec3& GetPosition() const = 0;
	protected:
		glm::mat4 m_Projection{ 1.0f };
		glm::mat4 m_View{ 1.0f };
		glm::vec3 m_Position{ 0.0f };
	};

}