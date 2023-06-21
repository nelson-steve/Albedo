#include "AlbedoPreCompiledHeader.h"

#include "PerlinNoise2D.h"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

namespace Albedo {

	void PerlinNoise2D::Init(double frequency, double amplitude, double persistance, uint32_t octaves,
		double mulitplier, uint32_t offsetX, uint32_t offsetY)
	{
		m_Frequency = frequency;
		m_Amplitude = amplitude;
		m_Persistence = persistance;
		m_Octaves = octaves;
		m_Multiplier = mulitplier;
		m_OffsetX = offsetX;
		m_OffsetY = offsetY;
	}

	void PerlinNoise2D::GenerateData(std::vector<char>& data, uint32_t width, uint32_t height, uint32_t depth)
	{
		double xFactor = 1.0 / (width - 1);
		double yFactor = 1.0 / (height - 1);

		for (unsigned w = 0; w < width; ++w)
		{
			for (unsigned h = 0; h < height; ++h)
			{
				double x = xFactor * w;
				double y = yFactor * h;
				double perlin = OctavePerlin(x, y);
				char result = (char)(perlin * 255);
				unsigned index = (w * width + h) * 4;
				data[index] = result;
				data[index + 1] = result;
				data[index + 2] = result;
				data[index + 3] = result;
			}
		}
	}

	double PerlinNoise2D::OctavePerlin(double x, double y)
	{
		double total = 0;
		double freq = m_Frequency;
		double amp = m_Amplitude;
		double maxValue = 0;

		for (uint32_t i = 0; i < m_Octaves; ++i)
		{
			glm::vec2 p((x * freq + m_OffsetX) * m_Multiplier, (y * freq + m_OffsetY) * m_Multiplier);

			total += ((glm::perlin(p) + 1.0) / 2.0) * amp;
			maxValue += amp;
			amp *= m_Persistence;
			freq *= 2;
		}
		return total / maxValue;
	}

}