#pragma once

#include "Albedo/Renderer/Texture.h"
#include "Albedo/Renderer/Shader.h"

#include <glm/glm.hpp>

namespace Albedo {

	class Terrain;

	class PerlinNoise
	{
	public:
		PerlinNoise()
		{
			m_Seed = std::random_device{}();
			GeneratePermutations();
		}

		int32_t GetSeed() const { return m_Seed; }
		/** @brief Just sets the seed, generate the permutations to see the diff. */
		void SetSeed(int32_t seed) { m_Seed = seed; }

		/** @brief Generate random lookup for permutations containing all numbers
		 * from 0..255 */
		void GeneratePermutations()
		{
			// TODO: Figure out how to get std::iota
			//std::iota(m_P.begin(), m_P.begin() + PerlinNoise::PERMUTATION_COUNT, 0);

			for (int i = 0; i < PerlinNoise::PERMUTATION_COUNT; i++)
			{
				m_P[i] = i;
			}

			std::default_random_engine rndEngine(m_Seed);
			std::shuffle(m_P.begin(), m_P.begin() + PerlinNoise::PERMUTATION_COUNT,
				rndEngine);

			for (uint32_t i = 0; i < PerlinNoise::PERMUTATION_COUNT; ++i)
				m_P[PerlinNoise::PERMUTATION_COUNT + i] = m_P[i];
		}

		/** @return Perlin 3D noise value in [-1,1] */
		float Noise(float x, float y, float z) const
		{
			int32_t X = (int32_t)glm::floor(x) & 255;   // Find unit cube
			int32_t Y = (int32_t)glm::floor(y) & 255;   // that contains a point.
			int32_t Z = (int32_t)glm::floor(z) & 255;
			x -= glm::floor(x);                         // Find relative x,y,z
			y -= glm::floor(y);                         // of point in cube.
			z -= glm::floor(z);
			float u = Fade(x);                              // Compute fade curves
			float v = Fade(y);                              // for each x,y,z.
			float w = Fade(z);
			// Hash coordinates of the 8 cube corners.
			uint32_t A = m_P[X] + Y;
			uint32_t AA = m_P[A] + Z;
			uint32_t AB = m_P[A + 1] + Z;
			uint32_t B = m_P[X + 1] + Y;
			uint32_t BA = m_P[B] + Z;
			uint32_t BB = m_P[B + 1] + Z;

			// And add blended results from 8 corners of the cube
			return Lerp(w, Lerp(v, Lerp(u, Grad(m_P[AA], x, y, z),
				Grad(m_P[BA], x - 1, y, z)),
				Lerp(u, Grad(m_P[AB], x, y - 1, z),
					Grad(m_P[BB], x - 1, y - 1, z))),
				Lerp(v, Lerp(u, Grad(m_P[AA + 1], x, y, z - 1),
					Grad(m_P[BA + 1], x - 1, y, z - 1)),
					Lerp(u, Grad(m_P[AB + 1], x, y - 1, z - 1),
						Grad(m_P[BB + 1], x - 1, y - 1, z - 1))));
		}

	private:
		// Fade fnc, qunitic fnc: 6t^5 - 15t^4 + 10t^3
		constexpr float Fade(float t) const
		{
			return t * t * t * (t * (t * (float)6 - (float)15) + (float)10);
		}

		constexpr float Lerp(float t, float a, float b) const
		{
			return a + t * (b - a);
		}

#ifndef CALC_GRAD_SWITCH
		constexpr float Grad(int hash, float x, float y, float z) const
		{
			// Convert LO 4 bits of hash code into 12 gradient directions
			int h = hash & 15;
			float u = h < 8 ? x : y;
			float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
			return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
		}
#else
		/** @see http://riven8192.blogspot.com/2010/08/calculate-perlinnoise-twice-as-fast.html */
		constexpr float Grad(int hash, float x, float y, float z) const
		{
			switch (hash & 0xF)
			{
			case 0x0: return  x + y;
			case 0x1: return -x + y;
			case 0x2: return  x - y;
			case 0x3: return -x - y;
			case 0x4: return  x + z;
			case 0x5: return -x + z;
			case 0x6: return  x - z;
			case 0x7: return -x - z;
			case 0x8: return  y + z;
			case 0x9: return -y + z;
			case 0xA: return  y - z;
			case 0xB: return -y - z;
			case 0xC: return  y + x;
			case 0xD: return -y + z;
			case 0xE: return  y - x;
			case 0xF: return -y - z;
			default: return 0; // never happens
			}
		}
#endif // CALC_GRAD_SWITCH

	private:
		static const size_t PERMUTATION_COUNT = 256;

		std::array<uint32_t, PERMUTATION_COUNT * 2> m_P;    ///< Permutations
		int32_t m_Seed{ 0 };
	};

	class FractalNoise
	{
	public:
		FractalNoise(const PerlinNoise& perlinNoise,
			float scale = (float)1,
			float offset = (float)0,
			uint32_t octaves = 6,
			float gain = (float)0.5,
			float lacunarity = (float)2)
			: perlinNoise(perlinNoise),
			scale(scale),
			offset(offset),
			octaveCount(octaves),
			gain(gain),
			lacunarity(lacunarity) {}

		/** @return 3D Fractal noise value in [0,1] */
		float Noise(float x, float y, float z)
		{
			float sum = 0;
			float max = (float)0;
			float frequency = (float)1;
			float amplitude = (float)1;

			for (uint32_t i = 0; i < octaveCount; ++i)
			{
				float noiseVal = perlinNoise.Noise(
					(x + offset) / scale * frequency,
					(y + offset) / scale * frequency,
					(z + offset) / scale * frequency); // * (T)2 - (T)1 ;

				sum += noiseVal * amplitude;
				max += amplitude;

				amplitude *= gain;
				frequency *= lacunarity;
			}

			sum = sum / max;
			return (sum + (float)1.0) / (float)2.0;
		}

	public:
		PerlinNoise perlinNoise;

		uint32_t octaveCount{ 0 };  ///< Number of octaves
		float scale{ 0 };               ///< Scales the sample
		float offset{ 0 };              ///< Offsets the sample
		float gain{ 0 };                ///< Scales amplitude, influence of each successive octave
		float lacunarity{ 0 };          ///< Scales frequency with each successive octave
	};

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
			PerlinNoise pNoise;
			m_FractalNoise = std::make_shared<FractalNoise>(pNoise);
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

		void GenerateValues();
		void UpdateTexture();
		
		void SetDimensions(uint32_t width, uint32_t height)
		{
			m_Width = width;
			m_Height = height;
		}

		void SetScale(float scale)
		{
			scale = glm::max<float>(0.001, scale);
			m_FractalNoise->scale = scale;
		}

		float GetMinValue() const { return m_MinValue; }
		float GetMaxValue() const { return m_MaxValue; }

		uint32_t GetWidth() const { return m_Width; }
		uint32_t GetHeight() const { return m_Height; }

		const auto& GetHeightMapData() const { return m_Values; }

		void Init();
		Ref<Texture2D> GenerateHeightmap(const Terrain& terrain);

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
		Ref<FractalNoise> m_FractalNoise;
		std::vector<float> m_Values;
		float m_MinValue{ 0.0 };
		float m_MaxValue{ 0.0 };

		PerlinNoiseData m_PerlinNoiseData;
		Ref<Texture2D> m_Texture;
		uint32_t m_Width{ 0 };
		uint32_t m_Height{ 0 };
		std::vector<char> m_Data;

		Ref<Shader> m_Shader;
		uint32_t m_UniformBuffer = 0;
		uint32_t m_Resolution = 512;

	};

}