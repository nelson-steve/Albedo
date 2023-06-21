#include "AlbedoPreCompiledHeader.h"

#include "SmoothNoise2D.h"

namespace Albedo {

	void SmoothNoise2D::Init(uint32_t zoomLevel)
	{
        m_Zoom = zoomLevel;
	}

	void SmoothNoise2D::Generate(std::vector<char>& data, uint32_t width, uint32_t height, uint32_t depth)
	{
        std::vector<std::vector<double>> pattern;
        pattern.resize(width, std::vector<double>(height));
        for (uint32_t w = 0; w < width; ++w)
        {
            for (uint32_t h = 0; h < height; ++h)
                pattern[w][h] = (double)rand() / (RAND_MAX + 1.0);
        }

        for (uint32_t w = 0; w < width; ++w)
        {
            for (uint32_t h = 0; h < height; ++h)
            {
                uint32_t index = w * width * 4 + h * 4;
                char value = (char)Turbulence(pattern, width, height, w, h);

                data[index] = value;
                data[index + 1] = value;
                data[index + 2] = value;
                data[index + 3] = (char)255;
            }
        }
	}

    double SmoothNoise2D::Turbulence(const std::vector<std::vector<double>>& pattern, uint32_t width, uint32_t height, double x, double y)
    {
        double sum = 0.0, actZoom = m_Zoom;
        while (actZoom >= 1.0)
        {
            sum = sum + Smooth(pattern, width, height, x / actZoom, y / actZoom) * actZoom;
            actZoom = actZoom / 2.0;
        }
        sum = 128 * sum / m_Zoom;
        return sum;
    }

    double SmoothNoise2D::Smooth(const std::vector<std::vector<double>>& pattern, uint32_t width, uint32_t height, double x1, double y1)
    {
        double fractX = x1 - (int)x1;
        double fractY = y1 - (int)y1;
        int x2 = ((int)x1 + width + 1) % width;
        int y2 = ((int)y1 + height + 1) % height;

        double value = 0.0;
        value += (1 - fractX) * (1 - fractY) * pattern[(int)x1][(int)y1];
        value += (1 - fractX) * fractY * pattern[(int)x1][(int)y2];
        value += fractX * (1 - fractY) * pattern[(int)x2][(int)y1];
        value += fractX * fractY * pattern[(int)x2][(int)y2];
        return value;
    }

}