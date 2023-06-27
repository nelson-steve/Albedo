#pragma once

#include "Albedo/Renderer/Texture.h"
#include "Albedo/Renderer/Shader.h"

namespace Albedo {

	struct PerlinNoiseData
	{
		double Frequency;
		double Amplitude;
		double Persistence;
		double Multiplier;
		uint32_t OffsetX;
		uint32_t OffsetY;
		uint32_t Octaves;
	};

	struct SmoothNoiseData
	{
		uint32_t Zoom;
	};

	struct RandomNoiseData
	{
		uint32_t Min;
		uint32_t Max;
	};

	class HeightmapGenerator
	{
	public:
		HeightmapGenerator() = default;
		HeightmapGenerator(int width, int height)
			:m_Width(width), m_Height(height)
		{
			m_Texture = Texture2D::Create(width, height);
			//m_Texture = Texture2D::Create("Assets/Textures/iceland_heightmap.png", true);
		}

		struct MeshGenerationBuffer
		{
			glm::vec2 offset;
			double scale;
			double pad;
			glm::mat4 normalTransform;
			glm::dmat4 pointTransform;
			glm::ivec2 gridDimensions;
		};

		void Init();
		Ref<Texture2D> GenerateHeightmap();

		void InitPerlinNoiseTexture(double pFrequency = 1, double pAmplitude = 1, double pPersistance = 1, uint32_t pOctaves = 4,
			double pMulitplier = 1, uint32_t pOffsetX = 0, uint32_t pOffsetY = 0);
		void GeneratePerlinNoiseTexture(uint32_t depth);

		void InitSmoothNoiseTexture(uint32_t zoomLevel = 1);
		void GenerateSmoothNoiseTexture(uint32_t width, uint32_t height, uint32_t depth);

		void InitRandomNoiseTexture(uint32_t min, uint32_t max);
		void GenerateRandomNoiseTexture(uint32_t width, uint32_t height, uint32_t depth);

		Ref<Texture2D> GetTexture() const { return m_Texture; }
		const std::vector<char> GetData() const { return m_Data; }
	private:
		double OctavePerlin(double x, double y);
	private:
		PerlinNoiseData m_PerlinNoiseData;
		Ref<Texture2D> m_Texture;
		uint32_t m_Width;
		uint32_t m_Height;
		std::vector<char> m_Data;

		Ref<Shader> m_Shader;
		uint32_t m_UniformBuffer = 0;
		uint32_t m_Resolution = 512;

	};

}