#pragma once

#include "Terrain.h"

namespace Albedo {

	class TerrainManager
	{
	public:
		TerrainManager();

		void SetMaxTessLevel(uint32_t maxTessLevel) { m_MaxTessLevel = maxTessLevel; }
		void SetTessLevel(uint32_t tessLevel) { m_TessLevel = tessLevel; }
		void SetDepth(float depth) { m_Depth = depth; }

		uint32_t GetTessLevel() const { return m_TessLevel; }
		float GetDepth() const { return m_Depth; }
		Ref<VertexArray> GetVAO() const { return m_VAO; }
		Ref<Shader> GetShader() const { return m_Shader; }

		void Render(const EditorCamera& camera);
		void UpdateTerrain();

		void LoadGrassTexture(const std::string& path) { m_GrassTexture = Texture2D::Create(path, false); }
		void LoadStoneTexture(const std::string& path) { m_StoneTexture = Texture2D::Create(path, false); }
	private:
		enum TextureIndices {
			TEX_WATER,
			TEX_SAND,
			TEX_GRASS,
			TEX_STONY_GRASS,
			TEX_ROCKY,
			TEX_MOUNTAINS,
			TEX_SNOW
		};
		struct Region
		{
			float startHeight;
			std::string name;
			glm::vec3 tint;
			int texIndex;      ///< Index into texture paths
			float scale;
			float tintStrength;
			float blendStrength;

			Region();
			Region(int id);
			Region(const std::string& name);
			Region(float startHeight, const std::string& name, glm::vec3 tint,
				int texIndex, float scale, float tintStrength,
				float blendStrength);
		};

		std::vector<Region> m_Regions{
			{ 0.0, "Water Deep", glm::vec3(0.0, 0.0, 0.8), TEX_WATER, 2.0, 0.1, 0.2 },
			{ 0.1, "Water Shallow", glm::vec3(54, 103, 199) / 255.f, TEX_WATER, 2.0, 0.1, 0.2 },
			{ 0.15, "Sand", glm::vec3(210, 208, 125) / 255.f, TEX_SAND, 2.0, 0.1, 0.2 },
			{ 0.2, "Grass", glm::vec3(86, 152, 23) / 255.f, TEX_GRASS, 2.0, 0.1, 0.2 },
			{ 0.3, "Trees", glm::vec3(62, 107, 18) / 255.f, TEX_STONY_GRASS, 2.0, 0.1, 0.2 },
			{ 0.6, "Rock", glm::vec3(90, 69, 60) / 255.f, TEX_ROCKY, 2.0, 0.1, 0.2 },
			{ 0.8, "Higher Rock", glm::vec3(75, 60, 53) / 255.f, TEX_MOUNTAINS, 2.0, 0.1, 0.2 },
			{ 0.9, "Snow", glm::vec3(1.0, 1.0, 1.0), TEX_SNOW, 2.0, 0.1, 0.2 },
		};

		static const uint32_t s_kMaxRegionCount = 8;
		static constexpr glm::vec3 s_kDefaultColor{ 1.0 };

		struct RegionUBO
		{
			float texScale;
			int texIndex;
			float blendStrength;
			float startHeight;
			glm::vec3 tint;
			float tintStrength;
		};

		struct TerrainUBO
		{
			float minHeight;
			float maxHeight;
			int regionCount;
			alignas(16) RegionUBO regions[s_kMaxRegionCount];
		};

		TerrainUBO m_TerrainUBOData;
		Ref<UniformBuffer> m_TerrainUBO;
	private:
		Ref<Terrain> m_Terrain;
		Ref<Shader> m_Shader;
		Ref<VertexArray> m_VAO;
		Ref<VertexBuffer> m_VBO;
		std::vector<float> m_Vertices;
		Ref<Texture2D> m_GrassTexture;
		Ref<Texture2D> m_StoneTexture;

		Ref<HeightmapGenerator> m_NoiseMap;
		glm::uvec2 m_TextureSize{ 512 };
		bool m_TerrainChanged = true;
		//std::unique_ptr<Terrain> m_Terrain;
		//std::shared_ptr<Shader> m_TerrainShader;

		Ref<Texture2DArray> m_TexArray;
		int32_t m_TexArrayTexWidth = 512;
		int32_t m_TexArrayTexHeight = 512;

		unsigned int rez = 20;
		uint32_t m_TerrainSize = 0;
		uint32_t m_NoOfTerrainChunks = 0;
		uint32_t m_MinTessLevel = 4;
		uint32_t m_MaxTessLevel = 32;
		uint32_t m_TessLevel = 8;
		float m_Depth = 0.05;
	};

}