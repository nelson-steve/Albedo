#pragma once

namespace Albedo {

	class SmoothNoise2D
	{
	public:
		void Init(uint32_t zoomLevel = 1);
		void Generate(std::vector<char>& data, uint32_t width, uint32_t height, uint32_t depth);
	private:
		double Turbulence(const std::vector<std::vector<double>>& pattern, uint32_t width, uint32_t height, double x, double y);
		double Smooth(const std::vector<std::vector<double>>& pattern, uint32_t width, uint32_t height, double x1, double y1);
	private:
		uint32_t m_Zoom;
	};

}