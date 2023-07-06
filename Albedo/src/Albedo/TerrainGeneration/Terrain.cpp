#include "AlbedoPreCompiledHeader.h"
#include "Platform/OpenGL/Utils.h"
#include "Terrain.h"

#include <stb_image.h>

#define TRIANGLES_PER_QUAD 2
#define INDICES_PER_TRIANGLE 3

namespace Albedo {

	Terrain::Terrain(const glm::uvec2& size,
		const std::vector<float>& heightMap)

		: m_HeightMap(heightMap),
		m_Size(size)
	{
		Generate();
	}

	void Terrain::Init()
	{
		//m_GrassTexture = Texture2D::Create("Assets/Textures/Diluc.png", false);
		//m_StoneTexture = Texture2D::Create("Assets/Textures/Diluc.png", false);
		//m_Normalmap = Texture2D::Create("Assets/Textures/Diluc.png", false);
		//m_Heightmap = m_HeightmapGenerator->GetTexture();
		//m_HeightmapGenerator->InitPerlinNoiseTexture();
		//m_HeightmapGenerator->GeneratePerlinNoiseTexture(10);
		m_HeightmapGenerator = std::make_shared<HeightmapGenerator>(512, 512);
		m_HeightmapGenerator->Init();
		//m_Heightmap = m_HeightmapGenerator->GenerateHeightmap(*this);
		//int width = m_Heightmap->getWidth();
		//int height = m_Heightmap->getHeight();
		uint32_t size = 4 * 512 * 512;
		//m_Heightmap->Bind();
		float* data = new float[size];
		//m_Heightmap->GetData(data, Texture::DataType::FLOAT);
		//glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_FLOAT, data);

		//m_Heightmap = Texture2D::Create(512, 512);
		//m_Heightmap->SetData(data, Texture::DataType::FLOAT);

		delete[] data;
		
	}

	void Terrain::Generate()
	{
		GenerateTexCoords();
		GeneratePositions();

		if (m_UseFallOffMap)
		{
			//GenerateFallOffMap();
			//ApplyFallOffMap();
		}

		GenerateIndices();
		GenerateNormals();

		UpdateVAO();
	}

	void Terrain::Render()
	{
		m_VAO->Bind();
		glDrawElements(GL_TRIANGLES, m_Indices.size(), GL_UNSIGNED_INT, 0);
	}

	void Terrain::GenerateTexCoords()
	{
		const glm::uvec2 kSize = m_Size;
		m_TexCoords.resize(GetVertexCount());

		for (uint32_t y = 0; y < kSize.y; ++y)
			for (uint32_t x = 0; x < kSize.x; ++x)
			{
				const uint32_t kIndex = y * kSize.x + x;
				m_TexCoords[kIndex].x = x / static_cast<float>(kSize.x - 1);
				m_TexCoords[kIndex].y = y / static_cast<float>(kSize.y - 1);
			}
	}

	void Terrain::GeneratePositions()
	{
		const glm::vec2 kSize = m_Size;

		// Account for tile scaling
		const glm::vec2 kWorldSize = GetWorldSize();
		const glm::vec2 kCenterOffset = kWorldSize * 0.5f;

		m_Positions.resize(GetVertexCount());

		for (uint32_t y = 0; y < kSize.y; ++y)
			for (uint32_t x = 0; x < kSize.x; ++x)
			{
				const uint32_t kIndex = y * kSize.x + x;
				auto& position = m_Positions[kIndex];

				position.x = m_TexCoords[kIndex].x * kWorldSize.x - kCenterOffset.x;
				position.z = m_TexCoords[kIndex].y * kWorldSize.y - kCenterOffset.y;
				position.y = GetHeightScaled(kIndex);
			}
	}

	void Terrain::GenerateIndices()
	{
		const glm::vec2 kSize = m_Size - 1U;

		m_Indices.resize(kSize.x * kSize.y *
			TRIANGLES_PER_QUAD * INDICES_PER_TRIANGLE);

		uint32_t index = 0;
		for (uint32_t y = 0; y < kSize.y; ++y)
			for (uint32_t x = 0; x < kSize.x; ++x)
			{
				const uint32_t kVertexIndex = y * m_Size.x + x;
				// Top triangle
				m_Indices[index++] = kVertexIndex;
				m_Indices[index++] = kVertexIndex + m_Size.x + 1;
				m_Indices[index++] = kVertexIndex + 1;
				// Bottom triangle
				m_Indices[index++] = kVertexIndex;
				m_Indices[index++] = kVertexIndex + m_Size.x;
				m_Indices[index++] = kVertexIndex + m_Size.x + 1;
			}
	}

	void Terrain::GenerateNormals()
	{
		m_Normals.resize(GetVertexCount(), glm::vec3(0.0f));

		const uint32_t kIndexCount = GetIndexCount();
		Albedo_CORE_ASSERT(kIndexCount % INDICES_PER_TRIANGLE == 0, "error");

			for (uint32_t i = 0; i < kIndexCount; i += INDICES_PER_TRIANGLE)
			{
				const auto kIdx1 = m_Indices[i + 0];
				const auto kIdx2 = m_Indices[i + 1];
				const auto kIdx3 = m_Indices[i + 2];

				const auto& v0 = m_Positions[kIdx1];
				const auto& v1 = m_Positions[kIdx2];
				const auto& v2 = m_Positions[kIdx3];

				const glm::vec3 kNormal = glm::normalize(
					glm::cross(v1 - v0, v2 - v0)
				);

				// Save for each vertex
				m_Normals[kIdx1] += kNormal;
				m_Normals[kIdx2] += kNormal;
				m_Normals[kIdx3] += kNormal;
			}

		// Normalize each vertex normal
		for (auto& normal : m_Normals)
		{
			normal = glm::normalize(normal);
		}
	}

	void Terrain::UpdateVAO()
	{
		m_VAO = VertexArray::Create();

		auto posVBO = VertexBuffer::Create(m_Positions, Utils::GetVertexSize(m_Positions));
		//m_MeshBufferData.m_ID->SetLayout({ {ShaderDataType::Int, "a_ID"} });
		posVBO->SetLayout({ {ShaderDataType::Float3, "a_Pos"} });
		m_VAO->AddVertexBuffer(posVBO);

		auto normVBO = VertexBuffer::Create(m_Normals, Utils::GetVertexSize(m_Normals));
		posVBO->SetLayout({ {ShaderDataType::Float3, "a_Normal"} });
		m_VAO->AddVertexBuffer(normVBO);
		
		auto uvVBO = VertexBuffer::Create(m_TexCoords, Utils::GetVertexSize(m_TexCoords));
		posVBO->SetLayout({ {ShaderDataType::Float2, "a_UV"} });
		m_VAO->AddVertexBuffer(uvVBO);

		//m_VAO->Bind();
		auto indicesVBO = IndexBuffer::Create(m_Indices.data(), m_Indices.size());
		m_VAO->SetIndexBuffer(indicesVBO);	

		//const auto kVertexCount = GetVertexCount();
		//std::vector<Vertex> vertices(kVertexCount);
		//
		//for (size_t i = 0; i < kVertexCount; ++i)
		//{
		//	vertices[i].position = m_Positions[i];
		//	vertices[i].normal = m_Normals[i];
		//	vertices[i].texCoord = m_TexCoords[i];
		//}
		//
		//// TODO recreate or reallocate?
		//
		//auto vbo = VertexBuffer::Create(
		//	vertices,
		//	vertices.size() * sizeof(Vertex)
		//);
		//
		//vbo->SetLayout({
		//	{ sgl::ElementType::Float3, "position" },
		//	{ sgl::ElementType::Float3, "normal" },
		//	{ sgl::ElementType::Float2, "texCoord" }
		//	});
		//
		//auto ibo = sgl::IndexBuffer::Create(
		//	m_Indices.data(),
		//	m_Indices.size()
		//);
		//
		//m_VAO.ClearVertexBuffers();
		//m_VAO.ClearIndexBuffer();
		//
		//m_VAO.AddVertexBuffer(vbo);
		//m_VAO.SetIndexBuffer(ibo);
	}

	void Terrain::GenerateFallOffMap()
	{
		m_FallOffMap.resize(m_Size.x * m_Size.y);

		for (uint32_t y = 0; y < m_Size.y; ++y)
			for (uint32_t x = 0; x < m_Size.x; ++x)
			{
				m_FallOffMap[y * m_Size.x + x] =
					Smoothstep(m_FallOffEdge0,
						m_FallOffEdge1,
						glm::max(
							glm::abs(x / static_cast<float>(m_Size.x) * 2.f - 1.f),
							glm::abs(y / static_cast<float>(m_Size.y) * 2.f - 1.f)
						)
					);
			}
	}

	void Terrain::ApplyFallOffMap()
	{
		for (uint32_t y = 0; y < m_Size.y; ++y)
			for (uint32_t x = 0; x < m_Size.x; ++x)
			{
				const uint32_t kIndex = y * m_Size.x + x;
				m_Positions[kIndex].y =
					glm::clamp(
						m_HeightMap[kIndex] - m_FallOffMap[kIndex],
						0.f,
						1.f)
					* m_HeightScale;
			}
	}

	void Terrain::SetShaderUniforms(Ref<Shader> shader)
	{
		shader->Bind();
		//m_Heightmap->Bind();
		//glBindTexture(GL_TEXTURE_2D, m_Heightmap->GetTextureID());

		//if(m_Heightmap)
		//m_Heightmap->Bind();
		//m_GrassTexture->Bind(1);
		//m_StoneTexture->Bind(2);
	}

	void Terrain::LoadHeightmapTexture(int width, int height)
	{
		//m_HeightmapGenerator = std::make_shared<HeightmapGenerator>(width, height);
		//m_HeightmapGenerator->InitPerlinNoiseTexture(5, 2, 10, 4, 3, 10, 20);
		//m_HeightmapGenerator->GeneratePerlinNoiseTexture(19);
		//m_TextureHeightmap = m_HeightmapGenerator->GetTexture();
	}

}

