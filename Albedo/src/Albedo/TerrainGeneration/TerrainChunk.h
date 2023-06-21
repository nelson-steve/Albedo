#pragma once

#include "Albedo/Renderer/Texture.h"
#include "HeightmapGenerator.h"
#include "Albedo/Renderer/VertexArray.h"
#include "Albedo/Renderer/Shader.h"
#include "Albedo/Cameras/EditorCamera.h"

namespace Albedo {

	class TerrainChunk
	{
	public:
        TerrainChunk()
        {
            m_VAO = VertexArray::Create();
            m_Shader = Shader::Create("TerrainShader_GPU.glsl");

            m_TextureGrass = Texture2D::Create("Assets/Textures/Diluc.png", false);
            m_TextureStone = Texture2D::Create("Assets/Textures/Diluc.png", false);
        }

        void SetScale(const glm::vec3& position) { m_Position = position; }
        void SetPosition(const glm::vec3& scale) { m_Scale = scale; }
        void SetTessLevel (uint32_t tessLevel) { m_TessLevel = tessLevel; }
        void SetDepth(float depth) { m_Depth = depth; }
        Ref<Shader> GetShader() const { return m_Shader; }

        void SetShaderData(const EditorCamera& camera);

        const glm::vec3& GetScale() const { return m_Scale; }
        const glm::vec3& GetPosition() const { return m_Position; }
        uint32_t GetTessLevel() const { return m_TessLevel; }
        float GetDepth() const { return m_Depth; }
        Ref<VertexArray> GetVAO() const { return m_VAO; }

        void LoadGrassTexture(const std::string& path) { m_TextureGrass = Texture2D::Create(path, false); }
        void LoadStoneTexture(const std::string& path) { m_TextureStone = Texture2D::Create(path, false); }
        void LoadHeightmapTexture(const std::string& path) { m_TextureHeightmap = Texture2D::Create(path, false); }
        void LoadNormalmapTexture(const std::string& path) { m_TextureNormalmap = Texture2D::Create(path, false); }
        void LoadHeightmapTexture(int width, int height);
        //void LoadNormalmapTexture(int width, int height);
	private:
        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec3 m_Scale = glm::vec3(1.0f);
        Ref<VertexArray> m_VAO;
        Ref<Shader> m_Shader;
        Ref<Texture> m_TextureGrass;
        Ref<Texture> m_TextureStone;
        Ref<Texture> m_TextureHeightmap;
        Ref<Texture> m_TextureNormalmap;
        Ref<HeightmapGenerator> m_HeightmapGenerator;
        uint32_t m_MaxTessLevel = 8;
        uint32_t m_TessLevel = 8;
        float m_Depth = 0.05;
        unsigned* amount;
	};

}