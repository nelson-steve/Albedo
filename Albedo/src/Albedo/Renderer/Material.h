#pragma once

#include "VertexArray.h"
#include "Texture.h"
#include "Shader.h"

namespace Albedo {

	class Material
	{
	public:
		Material() {}

		void SetAlbedoColor(const glm::vec3& color);
		void SetRoughnessScale(float scale);
		void SetExposure(float exposure);

		const glm::vec3& GetAlbedoColor() { return m_AlbedoColor; }
		float GetRoughnessScale() { return m_RoughnessScale; }
		float GetExposure() { return m_Exposure; }

		const glm::vec3& GetAlbedoColor() const { return m_AlbedoColor; }
		float GetRoughnessScale() const { return m_RoughnessScale; }
		float GetExposure() const { return m_Exposure; }
	private:
		glm::vec3 m_AlbedoColor = glm::vec3(1.0, 0.0, 0.0);
		float m_RoughnessScale = 0.0;
		float m_Exposure = 0.0;
	};

}