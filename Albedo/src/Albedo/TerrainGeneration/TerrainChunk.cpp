#include "TerrainChunk.h"

#include "Albedo/Cameras/EditorCamera.h"

namespace Albedo {

	void TerrainChunk::SetShaderData(const EditorCamera& camera)
	{
		m_Shader->Bind();
		m_Shader->SetUniformMat4("viewProject", camera.GetViewProjection());
		glm::mat4 transform = glm::mat4(1.0);
		m_Shader->SetUniformMat4("model", transform);
		m_Shader->SetUniformFloat3("viewPos", camera.GetPosition());

		m_Shader->SetUniformInt1("grassTex", m_TextureGrass->GetTextureID());
		m_Shader->SetUniformInt1("stoneTex", m_TextureStone->GetTextureID());
		m_Shader->SetUniformInt1("normalMap", m_TextureNormalmap->GetTextureID());
		m_Shader->SetUniformInt1("heightMap", m_TextureHeightmap->GetTextureID());

		m_Shader->SetUniformInt1("maxTessLevel", m_MaxTessLevel);
		m_Shader->SetUniformFloat("farPlane", camera.GetFarClip());
		m_Shader->SetUniformFloat3("clipPlane", glm::vec3(0.0));
		m_Shader->SetUniformFloat("depth", m_Depth);
		m_Shader->SetUniformInt1("lightsAmount", 0);
		m_Shader->SetUniformInt1("allowShadows", 0);
	}

	void TerrainChunk::LoadHeightmapTexture(int width, int height)
	{
		m_HeightmapGenerator = std::make_shared<HeightmapGenerator>(width, height);
		m_HeightmapGenerator->InitPerlinNoiseTexture();
		m_HeightmapGenerator->GeneratePerlinNoiseTexture(1);
	}

}

