#pragma once

namespace Albedo {

	class PerlinNoise2D
	{
	public:
		void Init(double pFrequency = 1, double pAmplitude = 1, double pPersistance = 1, uint32_t pOctaves = 4, 
			double pMulitplier = 1, uint32_t pOffsetX = 0, uint32_t pOffsetY = 0);
		void GenerateData(std::vector<char>& data, uint32_t width, uint32_t height, uint32_t depth);
	private:
		double OctavePerlin(double x, double y);
	private:
		double m_Frequency;
		double m_Amplitude;
		double m_Persistence;
		double m_Multiplier;
		uint32_t m_OffsetX;
		uint32_t m_OffsetY;
		uint32_t m_Octaves;
	};

}