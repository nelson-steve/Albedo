#include "AlbedoPreCompiledHeader.h"
#include "HeightmapGenerator.h"
#include "Terrain.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
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
	
	void HeightmapGenerator::GenerateValues()
	{
		m_Values.resize(m_Width * m_Height);
		m_MinValue = std::numeric_limits<float>::max();
		m_MaxValue = std::numeric_limits<float>::min();

		for (uint32_t y = 0; y < m_Height; ++y)
		{
			for (uint32_t x = 0; x < m_Width; ++x)
			{
				const uint32_t kIndex = y * m_Width + x;
				const float kValue = m_FractalNoise->Noise(x, y, 0);

				m_Values[kIndex] = kValue;

				m_MinValue = glm::min(m_MinValue, kValue);
				m_MaxValue = glm::max(m_MaxValue, kValue);
			}
		}
		Albedo_Core_INFO("generated values");
	}

	void HeightmapGenerator::UpdateTexture()
	{
		std::vector<glm::vec3> grayscaleValues;
		grayscaleValues.reserve(m_Values.size());
		std::transform(m_Values.begin(), m_Values.end(),
			std::back_inserter(grayscaleValues),
			[](float v) { return glm::vec3(v); });

		m_Texture->SetData(grayscaleValues.data(), Texture::DataType::FLOAT);
	}

	void HeightmapGenerator::Init()
	{
		TextureConfiguration config(Config::TextureType::Texture2D, Config::InternalFormat::RGBA32F, Config::TextureLayout::ClampToEdge,
			Config::MinMagFilters::LINEAR, Config::MinMagFilters::LINEAR, Config::DataType::FLOAT,
			Config::DataFormat::RGBA, true, false);
		config.m_Width = 512;
		config.m_Height = 512;
		m_Texture = Texture2D::Create(config);
		//m_Texture = Texture2D::Create(512, 512);
		
		m_Shader = Shader::Create("Assets/Shaders/TerrainCompute3DShader.glsl");

		glGenBuffers(1, &m_UniformBuffer);
		glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(MeshGenerationBuffer), NULL, GL_DYNAMIC_DRAW);

		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_UniformBuffer);
	}

	Ref<Texture2D> HeightmapGenerator::GenerateHeightmap(const Terrain& chunk)
	{
		m_Shader->Bind();
		//glUseProgram(mComputeProgram);

		glBindBuffer(GL_UNIFORM_BUFFER, m_UniformBuffer);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_UniformBuffer);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(MeshGenerationBuffer), NULL, GL_DYNAMIC_DRAW);

		MeshGenerationBuffer* generationProperties = (MeshGenerationBuffer*)glMapBufferRange(GL_UNIFORM_BUFFER, 0, sizeof(MeshGenerationBuffer), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

		glm::vec2 offset{ 10.0 };
		double scale{ 10.0 };
		double pad{ 5.0 };

		//generationProperties->gridDimensions = glm::ivec2(m_Resolution, m_Resolution);
		generationProperties->offset = chunk.GetOffset();
		//generationProperties->scale = scale;
		//generationProperties->pad = pad;
		//generationProperties->normalTransform = glm::mat4(1.0f);
		//generationProperties->pointTransform = glm::translate(glm::mat4(1.0f), glm::vec3(offset.x, 1.0, offset.y)) * glm::scale(glm::mat4(1.0), glm::vec3(scale, 1.0f, scale));

		glUnmapBuffer(GL_ARRAY_BUFFER);

		//glBindImageTexture(0, m_Texture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		glBindImageTexture(0, m_Texture->GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
		//glBindImageTexture(1, mpIntermediateTexture, 0, GL_FALSE, 0, GL_READ_WRITE, GL_R32F);

		//Verify that we are using texture with a resolution divible by 16 since the shader will not have proper coverage otherwise
		if (m_Resolution < 16 || m_Resolution % 16 != 0)
		{
			std::cout << "Texture resolution is not divisible by 16 this may cause problems with the shaders\n";
			throw - 1;
		}

		//Each local group in the shader is currently specified as 16x16 threads
		GLuint workGroupCount = (GLuint)(m_Resolution / 16);

		//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		glDispatchCompute(workGroupCount, workGroupCount, 1);

		//Wait for operations to finish
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		//glGenerateMipmap(GL_TEXTURE_2D);

		return m_Texture;
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
		m_Texture->SetData(m_Data.data(), Texture::DataType::FLOAT);
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
