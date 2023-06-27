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
        TerrainChunk();

        void Init();

        void SetPosition(const glm::vec3& position) { m_Position = position; }
        void SetOffset(const glm::vec3& offset) { m_Offset = offset; }
        void SetScale(const glm::vec3& scale) { m_Scale = scale; }
        void SetMaxTessLevel (uint32_t maxTessLevel) { m_MaxTessLevel = maxTessLevel; }
        void SetTessLevel (uint32_t tessLevel) { m_TessLevel = tessLevel; }
        void SetDepth(float depth) { m_Depth = depth; }
        void SetShaderUniforms(const EditorCamera& camera);

        Ref<Shader> GetShader() const { return m_Shader; }
        Ref<HeightmapGenerator> GetHeightmapGenerator() const { return m_HeightmapGenerator; }
        Ref<Texture2D> GetTHeightMap() const { return m_Heightmap; }
        const glm::vec3& GetScale() const { return m_Scale; }
        const glm::vec3& GetPosition() const { return m_Position; }
        uint32_t GetTessLevel() const { return m_TessLevel; }
        float GetDepth() const { return m_Depth; }
        Ref<VertexArray> GetVAO() const { return m_VAO; }

        void Update();
        void Render(const EditorCamera& camera);

        void LoadGrassTexture(const std::string& path) { m_GrassTexture = Texture2D::Create(path, false); }
        void LoadStoneTexture(const std::string& path) { m_StoneTexture = Texture2D::Create(path, false); }
        void LoadHeightmapTexture(const std::string& path) { m_Heightmap = Texture2D::Create(path, false); }
        void LoadNormalmapTexture(const std::string& path) { m_Normalmap = Texture2D::Create(path, false); }
        void LoadHeightmapTexture(int width, int height);
        //void LoadNormalmapTexture(int width, int height);
        unsigned int terrainVAO, terrainVBO, terrainIBO;
        unsigned int texture;
	private:
        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec2 m_Offset = glm::vec3(0.0);
        glm::vec3 m_Scale = glm::vec3(1.0f);
        uint32_t m_MinTessLevel = 4;
        uint32_t m_MaxTessLevel = 32;
        uint32_t m_TessLevel = 8;
        float m_Depth = 0.05;
        unsigned int rez = 20;
        std::vector<float> m_Vertices;
        std::vector<unsigned int> m_Indices;
        Ref<Texture2D> m_GrassTexture;
        Ref<Texture2D> m_StoneTexture;
        Ref<Texture2D> m_Heightmap;
        Ref<Texture2D> m_Normalmap;
        Ref<HeightmapGenerator> m_HeightmapGenerator;
        Ref<VertexArray> m_VAO;
        Ref<VertexBuffer> m_VBO;
        Ref<IndexBuffer> m_IBO;
        Ref<Shader> m_Shader;
	};

}