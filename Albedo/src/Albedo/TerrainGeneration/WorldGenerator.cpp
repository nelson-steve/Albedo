#include "AlbedoPreCompiledHeader.h"

#include "WorldGenerator.h"

#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

namespace Albedo {

    PerlinNoise2D::PerlinNoise2D(double frequency, double amplitude, double persistance, uint32_t octaves,
        double multiplier, uint32_t offsetX, uint32_t offsetY)
        : m_Frequency(frequency), m_Amplitude(amplitude), m_Persistance(persistance), m_Octaves(octaves), m_Multiplier(multiplier),
          m_OffsetX(offsetX), m_OffsetY(offsetY) 
    {}

    double PerlinNoise2D::Octave(float x, float y)
    {
        double total = 0;
        double freq = m_Frequency;
        double amp = m_Amplitude;
        double maxValue = 0;

        for (unsigned i = 0; i < m_Octaves; ++i)
        {
            glm::vec2 p((x * freq + m_OffsetX) * m_Multiplier, (y * freq + m_OffsetY) * m_Multiplier);

            total += ((glm::perlin(p) + 1.0) / 2.0) * amp;
            maxValue += amp;
            amp *= m_Persistance;
            freq *= 2;
        }
        return total / maxValue;
    }

    void PerlinNoise2D::FillData(std::vector<unsigned char>& data, uint32_t width, uint32_t height, uint32_t depth)
    {
        double xFactor = 1.0 / (width - 1);
        double yFactor = 1.0 / (height - 1);

        for (uint32_t w = 0; w < width; ++w)
        {
            for (uint32_t h = 0; h < height; ++h)
            {
                double x = xFactor * w;
                double y = yFactor * h;
                double perlin = Octave(x, y);
                char result = (unsigned char)(perlin * 255);
                uint32_t index = (w * width + h) * 4;
                data[index] = result;
                data[index + 1] = result;
                data[index + 2] = result;
                data[index + 3] = result;
            }
        }
    }
}