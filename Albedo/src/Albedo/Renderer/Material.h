#pragma once

#include "VertexArray.h"
#include "Texture.h"
#include "Shader.h"

namespace Albedo {

	class Material
	{
	public:
		inline void AddPBRTexture(const Ref<Texture2D> texture) { m_PBRTextures.push_back(texture); }
		inline void AddDefaultTexture(const Ref<Texture2D> texture) { m_DefaultTexture = texture; }
		inline void SetAlbedoColor(const glm::vec3& color) { m_AlbedoColor = color; }
		inline void SetRoughnessScale(float scale = 0.0) { m_RoughnessScale = scale; }
		inline void SetExposure(float exposure = 0.0) { m_Exposure = exposure; }
		inline void SetShininess(float shininess = 0.0) { m_Shininess = shininess; }
		inline void SetPBRStatus(bool pbr) { m_IsPBR = pbr; m_PBRStatusChanged = true; }

		inline const auto& GetPBRTextures() const { return m_PBRTextures; }
		inline const Ref<Shader> GetAttachedShader() const { return m_AttachedShader; }
		inline const glm::vec3& GetAlbedoColor() const { return m_AlbedoColor; }
		inline float GetRoughnessScale() const { return m_RoughnessScale; }
		inline float GetExposure() const { return m_Exposure; }
		inline float GetShininess() const { return m_Shininess; }
		inline bool IsPBR() const { return m_IsPBR; }

		//const glm::vec3& GetAlbedoColor() const { return m_AlbedoColor; }
		//float GetRoughnessScale() const { return m_RoughnessScale; }
		//float GetExposure() const { return m_Exposure; }
	private:
		bool m_PBRStatusChanged = true;
		Ref<Shader> m_AttachedShader;
		std::vector<Ref<Texture2D>> m_PBRTextures;
		Ref<Texture2D> m_DefaultTexture;
		glm::vec3 m_AlbedoColor = glm::vec3(1.0, 0.0, 0.0);
		float m_RoughnessScale = 0.0;
		float m_Shininess = 0.0;
		float m_Exposure = 0.0;
		bool m_IsPBR = false;
	};

}