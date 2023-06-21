#pragma once

namespace Albedo {

	class PerlinNoise2D
	{
	public:
		PerlinNoise2D::PerlinNoise2D(double frequency, double amplitude, double persistance, uint32_t octaves,
			double multiplier, uint32_t offsetX, uint32_t offsetY);
		double Octave(float x, float y);
		void FillData(std::vector<unsigned char>& data, uint32_t width, uint32_t height, uint32_t depth);
	private:
		double m_Frequency, m_Amplitude, m_Persistance, m_Multiplier;
		uint32_t m_Octaves, m_OffsetX, m_OffsetY;
		uint32_t m_Width, m_Height, m_Depth;
	};

}