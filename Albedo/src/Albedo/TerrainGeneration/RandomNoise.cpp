#include "AlbedoPreCompiledHeader.h"

#include "RandomNoise.h"

namespace Albedo {

	void RandomNoise::Init(uint32_t min, uint32_t max)
	{
		m_Min = min;
		m_Max = max;
	}

	void RandomNoise::GenerateData(std::vector<char>& data, uint32_t width, uint32_t height, uint32_t depth)
	{
		uint32_t size = width * height * depth * 4;
		for (uint32_t i = 0; i < size; ++i)
			data[i] = (char)(rand() % m_Max) + m_Min;
	}

}