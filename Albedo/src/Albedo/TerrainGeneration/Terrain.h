#pragma once

#include "Albedo/Renderer/Texture.h"
#include "HeightmapGenerator.h"
#include "Albedo/Renderer/VertexArray.h"
#include "Albedo/Renderer/Shader.h"
#include "Albedo/Cameras/EditorCamera.h"

#include <glm/glm.hpp>

namespace Albedo {

	class Terrain
	{
	public:
        Terrain(const glm::uvec2& size, const std::vector<float>& heightMap);

        void Init();
        void Generate();
        void Update();
        void Render();

        void GenerateTexCoords();
        void GeneratePositions();
        void GenerateIndices();
        void GenerateNormals();
        void UpdateVAO();

        void GenerateFallOffMap();
        void ApplyFallOffMap();

        void SetPosition(const glm::vec3& position) { m_Position = position; }
        void SetOffset(const glm::vec3& offset) { m_Offset = offset; }
        void SetScale(const glm::vec3& scale) { m_Scale = scale; }
        void SetShaderUniforms(Ref<Shader> shader);

        Ref<HeightmapGenerator> GetHeightmapGenerator() const { return m_HeightmapGenerator; }
        //Ref<Texture2D> GetTHeightMap() const { return m_Heightmap; }
        const glm::vec3& GetScale() const { return m_Scale; }
        const glm::vec3& GetPosition() const { return m_Position; }
        const glm::vec2& GetOffset() const { return m_Offset; }

        glm::vec2 GetWorldSize() const {
            return glm::vec2((float)(m_Size.x - 1) * m_TileScale,
                (float)(m_Size.y - 1) * m_TileScale);
        }

        float GetTileScale() const { return m_TileScale; }
        float GetHeightScale() const { return m_HeightScale; }

        uint32_t GetVertexCount() const { return m_Size.x * m_Size.y; }
        uint32_t GetIndexCount() const { return m_Indices.size(); }
        uint32_t GetTriangleCount() const { return m_Indices.size() / 3; }

        float GetFallOffMapEdge0() const { return m_FallOffEdge0; }
        float GetFallOffMapEdge1() const { return m_FallOffEdge1; }

        Ref<VertexArray> GetVAO() const { return m_VAO; }

        inline float GetHeight(size_t index) const
        {
            return m_HeightMap[glm::min(index, m_HeightMap.size() - 1)];
        }

        inline float GetHeightScaled(size_t index) const
        {
            return GetHeight(index) * m_HeightScale;
        }

        //void LoadHeightmapTexture(const std::string& path) { m_Heightmap = Texture2D::Create(path, false); }
        //void LoadNormalmapTexture(const std::string& path) { m_Normalmap = Texture2D::Create(path, false); }
        void LoadHeightmapTexture(int width, int height);
        //void LoadNormalmapTexture(int width, int height);
	private:
        struct Vertex {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 texCoord;

            Vertex() : position(0), normal(0), texCoord(0) {}
        };

        constexpr float Smoothstep(float edge0, float edge1, float x) const {
            float t = glm::clamp((x - edge0) / (edge1 - edge0), 0.f, 1.f);
            return t * t * (3.f - 2.f * t);
        }
    private:
        const std::vector<float>& m_HeightMap;
        std::vector<glm::vec3> m_Positions;
        std::vector<glm::vec3> m_Normals;
        std::vector<glm::vec2> m_TexCoords;
        std::vector<uint32_t>  m_Indices;
        int m_IndicesSize;

        Ref<VertexArray> m_VAO;
        glm::uvec2 m_Size{ 5.0 };
        float m_TileScale{ 0.05 }; // Scaling factor of X and Z coord (per tile)
        float m_HeightScale{ 10.0 }; // Scaling factor of Y coord, the height

        std::vector<float> m_FallOffMap;
        bool m_UseFallOffMap{ true };

        float m_FallOffEdge0{ 0.0 };
        float m_FallOffEdge1{ 1.0 };

        Ref<HeightmapGenerator> m_HeightmapGenerator;
        glm::uvec2 m_TextureSize{ 512 };
        glm::vec3 m_Position = glm::vec3(0.0f);
        glm::vec2 m_Offset = glm::vec3(0.0);
        glm::vec3 m_Scale = glm::vec3(1.0f);
        //Ref<Texture2D> m_Heightmap;
        //Ref<Texture2D> m_Normalmap;
	};

}