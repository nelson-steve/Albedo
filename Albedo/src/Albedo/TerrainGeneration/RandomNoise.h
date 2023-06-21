#pragma once

namespace Albedo {

	class RandomNoise
	{
	public:
		void Init(uint32_t min, uint32_t max);
		void GenerateData(std::vector<char>& data, uint32_t width, uint32_t height, uint32_t depth);
	private:
		uint32_t m_Min;
		uint32_t m_Max;
	};

}