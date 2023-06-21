#include "AlbedoPreCompiledHeader.h"
#include "HeightmapGenerator.h"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

namespace Albedo {

	double HeightmapGenerator::OctavePerlin(double x, double y)
	{
		double total = 0;
		double freq = m_PerlinNoiseData.Frequency;
		double amp = m_PerlinNoiseData.Amplitude;
		double maxValue = 0;

		for (uint32_t i = 0; i < m_PerlinNoiseData.Octaves; ++i)
		{
			glm::vec2 p((x * freq + m_PerlinNoiseData.OffsetX) * 
				m_PerlinNoiseData.Multiplier, (y * freq + m_PerlinNoiseData.OffsetY) * m_PerlinNoiseData.Multiplier);

			total += ((glm::perlin(p) + 1.0) / 2.0) * amp;
			maxValue += amp;
			amp *= m_PerlinNoiseData.Persistence;
			freq *= 2;
		}
		return total / maxValue;
	}
	
	void HeightmapGenerator::InitPerlinNoiseTexture(double frequency, double amplitude, double persistance, uint32_t octaves,
		double mulitplier, uint32_t offsetX, uint32_t offsetY)
	{
		m_PerlinNoiseData.Frequency = frequency;
		m_PerlinNoiseData.Amplitude = amplitude;
		m_PerlinNoiseData.Persistence = persistance;
		m_PerlinNoiseData.Octaves = octaves;
		m_PerlinNoiseData.Multiplier = mulitplier;
		m_PerlinNoiseData.OffsetX = offsetX;
		m_PerlinNoiseData.OffsetY = offsetY;
	}

	void HeightmapGenerator::GeneratePerlinNoiseTexture(uint32_t depth)
	{
		Albedo_CORE_ASSERT(m_Width != 0 || m_Height != 0, "height or width is zero");
		m_Data.clear();
		m_Data.resize(m_Width * m_Height * 4);

		double xFactor = 1.0 / (m_Width - 1);
		double yFactor = 1.0 / (m_Height - 1);

		for (uint32_t w = 0; w < m_Width; ++w)
		{
			for (uint32_t h = 0; h < m_Height; ++h)
			{
				double x = xFactor * w;
				double y = yFactor * h;
				double perlin = OctavePerlin(x, y);
				char result = (char)(perlin * 255);
				uint32_t index = (w * m_Width + h) * 4;
				m_Data[index] = result;
				m_Data[index + 1] = result;
				m_Data[index + 2] = result;
				m_Data[index + 3] = result;
			}
		}

		if (!m_Data.empty())
			m_Texture->SetData(m_Data.data(), m_Data.size());
	}

	void HeightmapGenerator::InitSmoothNoiseTexture(uint32_t zoomLevel)
	{
		Albedo_CORE_ASSERT(false, "");
	}

	void HeightmapGenerator::GenerateSmoothNoiseTexture(uint32_t width, uint32_t height, uint32_t depth)
	{
		Albedo_CORE_ASSERT(false, "");
	}

	void HeightmapGenerator::InitRandomNoiseTexture(uint32_t min, uint32_t max)
	{
		Albedo_CORE_ASSERT(false, "");
	}

	void HeightmapGenerator::GenerateRandomNoiseTexture(uint32_t width, uint32_t height, uint32_t depth)
	{
		Albedo_CORE_ASSERT(false, "");
	}



}
